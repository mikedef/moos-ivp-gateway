/***************************************************************/                                       
/*    NAME: Supun Randeni and Michael DeFilippo                */                                       
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA  */                                       
/*    FILE: MOOSGateway.h                                      */                                       
/*    DATE: 2022-11-04                                         */
/* This is unreleased BETA code. no permission is granted or   */
/* implied to use, copy, modify, and distribute this software  */
/* except by the author(s), or those designated by the author. */
/***************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "MOOSGateway.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

MOOSGateway::MOOSGateway()
{
}

//---------------------------------------------------------
// Destructor

MOOSGateway::~MOOSGateway()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool MOOSGateway::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
        
    for (auto it : m_fwd_keys){
      if (key == it) {
	cout << "Received a forward to client message: " << it << endl;

	// Forward the message to the payload
	if (msg.IsDouble()){
	  handleMsgsToClientDouble(key, dval);
	}
	else if (msg.IsString()){
	  handleMsgsToClientString(key, sval);
	}
      }
    }
  }
	
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool MOOSGateway::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool MOOSGateway::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!

  m_io.poll();
  
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool MOOSGateway::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "tcp_port" && isNumber(value)) {
      m_tcp_port = atoi(value.c_str());
      MOOSTrace(" Gateway TCP Port = %i \n", m_tcp_port);
      m_server.reset(new gateway::tcp_server(m_io, m_tcp_port));
      MOOSTrace(" Gateway is established. \n");
      handled = true;
    }
    else if(param == "forward_to_client") {
      handled = setConfigForwardToClient(value);
    }
    else if(param == "block_from_client") {
      handled = setConfigBlockFromClient(value);
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }

  // Define handles for message types coming through the interface
  defineIncomingInterfaceMsgs();
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables()

void MOOSGateway::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  for (auto it : m_fwd_keys){
    Register(it, 0);
    MOOSTrace("Registering the variable " + it + "\n");
  }
  // Register("FOOBAR", 0);
}

//----------------------------------------------------------
// Procedure: setConfigForwardToClient()
bool MOOSGateway::setConfigForwardToClient(string str)
{
  bool all_ok = true;

  vector<string> keys = parseString(str, ',');
  for(unsigned int i=0; i<keys.size(); i++) {
    string key = stripBlankEnds(keys[i]);
    // Should at least contain 2 characters
    if(key.length() > 2)
      m_fwd_keys.insert(key);
    else
      all_ok = false;
  }

  // Appcasting Keys
  stringstream ss;
  for (auto it : m_fwd_keys){
    ss << it << ", ";
  }
  m_keys = ss.str();

  return(all_ok);
}

//------------------------------------------------------------
// Procedure: setConfigBlockFromClient()
bool MOOSGateway::setConfigBlockFromClient(string str)
{
  bool all_ok = true;

  vector<string> keys = parseString(str, ',');
  for(unsigned int i=0; i<keys.size(); i++) {
    string key = stripBlankEnds(keys[i]);
    // Should at least contain 2 characters
    if(key.length() > 2)
      m_blocked_keys.insert(key);
    else
      all_ok = false;
  }

  return(all_ok);
}

//-----------------------------------------------------------
// Procedure: defineIncomingInterfaceMsgs()
void MOOSGateway::defineIncomingInterfaceMsgs()
{
  m_server->read_callback<moos::gateway::ToGateway>(
    [this](const moos::gateway::ToGateway& msg,
	   const boost::asio::ip::tcp::endpoint& ep)
      {
	if (!m_client_connected){
	  m_end_point.reset(new asio::ip::tcp::endpoint(ep));
	  m_client_connected = true;
	}
	cout << "Data rcvd from client: " << ep << endl;
	cout << "Msg: " << msg.ShortDebugString() << endl;
	// Notify raw msg to DB
	Notify("CLIENT_RAW_IN", msg.ShortDebugString());
	m_last_msg = msg.ShortDebugString();

	// Sorting the data provided by the client
	if(msg.has_client_key())
	{
	  handleMsgsFromClient(msg);
	}
	else{
	  std::cout << "Invalid message! No MOOS var." << std::endl;
	}
	
      });
}

//-----------------------------------------------------------
// Procedure: handleMsgsFromClient()
void MOOSGateway::handleMsgsFromClient(const moos::gateway::ToGateway& msg)
{
  std::string serialized;
  
  // Check for blocked message
  cout << "Handle msg from client: " << endl;

  if(m_blocked_keys.count(msg.client_key()) >= 1){
    reportRunWarning("Received a blocked message from the client: " + msg.client_key());
  }
  else{
    // gateway::util::serialize_for_moos_message(&serialized, msg);
    //Notify(msg.client_key(), serialized);
    Notify(msg.client_key(), msg.client_string());
  }
}

//------------------------------------------------------------
// Procedure: handleMsgsToClientDouble()
void MOOSGateway::handleMsgsToClientDouble(std::string key, double dval)
{
  moos::gateway::FromGateway from_gateway;
  from_gateway.set_gateway_time(MOOSTime());
  from_gateway.set_gateway_key(key);
  from_gateway.set_gateway_double(dval);

  if(m_client_connected){
    m_server->write(from_gateway, *m_end_point);
  }
}

//------------------------------------------------------------
// Procedure: handleMsgsToClientString()
void MOOSGateway::handleMsgsToClientString(std::string key, std::string sval)
{
  moos::gateway::FromGateway from_gateway;
  from_gateway.set_gateway_time(MOOSTime());
  from_gateway.set_gateway_key(key);
  from_gateway.set_gateway_string(sval);

  if(m_client_connected){
    m_server->write(from_gateway, *m_end_point);
  }
}


//------------------------------------------------------------
// Procedure: buildReport()

bool MOOSGateway::buildReport() 
{
  m_msgs << "============================================" << endl;
  m_msgs << "File:                                       " << endl;
  m_msgs << "============================================" << endl;

  ACTable actab(2);
  //actab << "Alpha | Bravo | Charlie | Delta";
  //actab.addHeaderLines();
  actab << "Connected: " << m_client_connected;
  actab << "Last Msg: " << m_last_msg;
  actab << "Keys: " << m_keys;
  m_msgs << actab.getFormattedString();

  return(true);
}




