/***************************************************************/
/*    NAME: Supun Randeni and Michael DeFilippo                */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA  */
/*    FILE: MOOSGateway.h                                      */
/*    DATE: 2022-11-04                                         */
/* This is unreleased BETA code. no permission is granted or   */                                       
/* implied to use, copy, modify, and distribute this software  */                                       
/* except by the author(s), or those designated by the author. */ 
/***************************************************************/

#ifndef MOOSGateway_HEADER
#define MOOSGateway_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
//#include "util/protobuf_support_for_moos.h"
#include "lib_gateway_protobuf/gateway.pb.h"
#include "lib_gateway_tcp/tcp_server.h"

#include <sstream>

using namespace boost;

class MOOSGateway : public AppCastingMOOSApp
{
 public:
   MOOSGateway();
   ~MOOSGateway();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();
  
 protected:
   void registerVariables();
  bool setConfigForwardToClient(std::string);
  bool setConfigBlockFromClient(std::string);
  void defineIncomingInterfaceMsgs();
  void handleMsgsFromClient(const moos::gateway::ToGateway&);
  void handleMsgsToClientDouble(std::string, double);
  void handleMsgsToClientString(std::string, std::string);

 private: // Configuration variables

 private: // State variables
  std::string m_last_msg;

  // Interface related
  boost::asio::io_service m_io;
  int m_tcp_port = 1024;
  std::shared_ptr<gateway::tcp_server> m_server;
  std::shared_ptr<asio::ip::tcp::endpoint> m_end_point;
  bool m_client_connected = false;

  std::string m_keys;
  std::set<std::string> m_fwd_keys;    // m_fwd_msgs
  std::set<std::string> m_blocked_keys; // m_blocked_msgs
};

#endif 
