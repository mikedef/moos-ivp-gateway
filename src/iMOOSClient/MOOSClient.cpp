/************************************************************/
/*    NAME: mikedef                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: MOOSClient.cpp                                        */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "MOOSClient.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

MOOSClient::MOOSClient()
{
}

//---------------------------------------------------------
// Destructor

MOOSClient::~MOOSClient()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool MOOSClient::OnNewMail(MOOSMSG_LIST &NewMail)
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
        // Forward the message to the relay
        if (msg.IsDouble()){
          handleMsgsToRelayDouble(key, dval);
        }
        else if (msg.IsString()){
          handleMsgsToRelayString(key, sval);
        }
      }
    }
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool MOOSClient::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool MOOSClient::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!

  m_io.poll();

  if (!m_client->connected()) {
    m_relay_connected = false;
    // Re-Connect to Relay
    connectToRelay();
  }

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool MOOSClient::OnStartUp()
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
    if(param == "relay_port") {
      handled = setPosUIntOnString(m_relay_port, value);
    }
    else if(param == "relay_ip") {
      handled = handleConfigRelayIp(value);
    }
    else if(param == "forward_to_relay") {
      handled = setConfigForwardToRelay(value);
    }
    else if(param == "robot_id") {
      m_robot_id = value;
      handled = true;
    }
      //else if(param == "block_from_relay")

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }

  // Connect to Relay
  connectToRelay();

  // Define handles for msg types
  defineIncomingInterfaceMsgs();

  registerVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables()

void MOOSClient::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  for (auto it : m_fwd_keys) {
    Register(it, 0);
    MOOSTrace("Registering the variable " + it + "\n");
  }
  // Register("FOOBAR", 0);
}


//-----------------------------------------------------------
// Procedure: handleConfigRelayIp()

bool MOOSClient::handleConfigRelayIp(std::string value) {
  bool all_ok = true;

  // Check if string contains an IP address
  boost::system::error_code ec;
  asio::ip::address relay_ip = asio::ip::address::from_string(value, ec);

  if (ec.value() != 0)
  {
    // IP invalid. Break!
    std::cout << "Failed to parse the relay IP address. Error code = "
	      << ec.value() << ". Message: " << ec.message() << " \n";
    reportConfigWarning("Failed to parse the relay IP address.");
    all_ok = false;
  } else {
    all_ok = true;
    m_relay_ip_str = value;
  }
  return all_ok;
}

//-----------------------------------------------------------
// Procedure: connectToRelay()
void MOOSClient::connectToRelay() {
  bool loop_error = false;
  MOOSTrace("Connecting to the Relay... \n");
  cout << "Connecting to the relay \n" << endl;
  m_client->connect(m_relay_ip_str, m_relay_port);
  while (!m_client->connected())
  {
    m_client->connect(m_relay_ip_str, m_relay_port);
    usleep(10000);
    try
    {
      m_io.poll();
    }
    catch(boost::system::error_code & ec)
    {
      if (!loop_error)
      {
	std::cout << "Looping until connected to Relay. Error = " << ec << "\n";
	loop_error = true;
      }
    }
  }

  // Send kickoff message to relay
  moos::gateway::FromGateway msg;
  msg.set_gateway_time(MOOSTime());
  msg.set_gateway_robot_id("moos_init");
  msg.set_gateway_key("MOOS_INIT");
  msg.set_gateway_double(666);

  if (m_client->connected()) {
    MOOSTrace("Sending Kickoff Msg");
    cout << "Sending Kickoff Msg: " << msg.ShortDebugString().c_str() << endl;
    m_client->write(msg);
  }
  m_relay_connected = true;
  MOOSTrace("Connected to Relay. \n");
}

//-----------------------------------------------------------
// Procedure: setConfigForwardToRelay()
bool MOOSClient::setConfigForwardToRelay(std::string str) {
  bool all_ok = true;

  vector<std::string> keys = parseString(str, ',');
  for(unsigned int i=0; i<keys.size(); i++) {
    std::string key = stripBlankEnds(keys[i]);
    // Should contain at least 2 characters
    if(key.length() > 2)
      m_fwd_keys.insert(key);
    else
      all_ok = false;
  }

  // Appcasting Keys
  stringstream ss;
  for(auto it : m_fwd_keys){
    ss << it << ", ";
  }
  m_keys = ss.str();

  return(all_ok);
}

//------------------------------------------------------------
// Procedure: defineIncomingInterfaceMsgs()
void MOOSClient::defineIncomingInterfaceMsgs() {
  m_client->read_callback<moos::gateway::ToGateway>(
    [this](const moos::gateway::ToGateway& msg,
           const boost::asio::ip::tcp::endpoint& ep)
      {
        if (!m_relay_connected){
          m_end_point.reset(new asio::ip::tcp::endpoint(ep));
          m_relay_connected = true;
        }
        cout << "Data rcvd from client: " << ep << endl;
        cout << "Msg: " << msg.ShortDebugString() << endl;
        // Notify raw msg to DB
        Notify("CLIENT_RAW_IN", msg.ShortDebugString());
        m_last_msg = msg.ShortDebugString();

        // Sorting the data provided by the client
        if(!msg.client_key().empty())
        {
          handleMsgsFromRelay(msg);
        }
        else{
          std::cout << "Invalid message! No MOOS var." << std::endl;
        }

      });
}

//------------------------------------------------------------
// Procedure: handleMsgsFromRelay()
void MOOSClient::handleMsgsFromRelay(const moos::gateway::ToGateway& msg) {
  std::string serialized;

  // TODO: check for blocked msgs
  // if blocked ..., else
  if (msg.client_string().size() > 0) {
    Notify(msg.client_key(), msg.client_string());
  }
  else
    Notify(msg.client_key(), msg.client_double());
}

//------------------------------------------------------------
// Procedure: handleMsgsToRelayDouble()
void MOOSClient::handleMsgsToRelayDouble(std::string key, double dval) {
  moos::gateway::FromGateway from_gateway;
  from_gateway.set_gateway_time(MOOSTime());
  from_gateway.set_gateway_robot_id(m_robot_id);
  from_gateway.set_gateway_key(key);
  from_gateway.set_gateway_double(dval);
  if(m_client->connected()){
    m_client->write(from_gateway);
  }
}

//------------------------------------------------------------
// Procedure: handleMsgsToRelayString
void MOOSClient::handleMsgsToRelayString(std::string key, std::string sval) {
  moos::gateway::FromGateway from_gateway;
  from_gateway.set_gateway_time(MOOSTime());
  from_gateway.set_gateway_robot_id(m_robot_id);
  from_gateway.set_gateway_key(key);
  from_gateway.set_gateway_string(sval);
  if(m_client->connected()){
    m_client->write(from_gateway);
  }
}

//------------------------------------------------------------
// Procedure: buildReport()

bool MOOSClient::buildReport()
{
  m_msgs << "============================================" << endl;
  m_msgs << "File:                                       " << endl;
  m_msgs << "============================================" << endl;

  ACTable actab(4);
  actab << "Connected: " << m_client->connected(); //m_relay_connected;
  actab << "Robot: " << m_robot_id;
  actab << "IP: " << m_relay_ip_str;
  actab << "Port: " << m_relay_port;
  actab << "Last Msg: " << m_last_msg;
  actab << "Keys: " << m_keys;
  m_msgs << actab.getFormattedString();

  return(true);
}




