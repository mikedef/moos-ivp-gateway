/***************************************************************/
/*    NAME: Michael DeFilippo and Dr. Supun Randeni            */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA  */
/*    FILE: MOOSClient.h                                       */
/*    DATE: 2025-03-17                                         */
/* Copyright MIT and author/s of software.                     */
/* This is unreleased BETA code. no permission is granted or   */
/* implied to use, copy, modify, and distribute this software  */
/* except by the author(s), or those designated by the author. */
/***************************************************************/

#ifndef MOOSClient_HEADER
#define MOOSClient_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "lib_gateway_protobuf/gateway.pb.h"
#include "lib_gateway_tcp/tcp_server.h"
#include "lib_gateway_tcp/tcp_client.h"

#include <sstream>

using namespace boost;

class MOOSClient : public AppCastingMOOSApp
{
 public:
   MOOSClient();
   ~MOOSClient();

 protected: // Standard MOOSApp functions to overload
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload
   bool buildReport();

 protected:
  void registerVariables();
  bool handleConfigRelayIp(std::string value);
  void connectToRelay();
  bool setConfigForwardToRelay(std::string);
  void defineIncomingInterfaceMsgs();
  void handleMsgsFromRelay(const moos::gateway::ToGateway&);
  void handleMsgsToRelayDouble(std::string, double);
  void handleMsgsToRelayString(std::string, std::string);

 private: // Configuration variables

  std::string m_last_msg;
  std::string m_robot_id;  // Robot name
  std::string m_keys;  // For appcasting
  std::set<std::string> m_fwd_keys;

  // Client Interface
  boost::asio::io_service m_io;
  std::shared_ptr<gateway::tcp_client> m_client{gateway::tcp_client::create(m_io)};
  std::shared_ptr<asio::ip::tcp::endpoint> m_end_point;
  unsigned int m_relay_port = 1024;
  std::string m_relay_ip_str = "localhost";
  bool m_relay_connected = false;

 private: // State variables
};

#endif
