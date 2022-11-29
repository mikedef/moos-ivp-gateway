# Repo for the iMOOSGateway application

## iMOOSGateway

* MOOS application to connect to a client application, if used with the ROS node protobuf_client a ROS based client, to bridge key-value pairs between MOOS-IvP and a non-MOOS system
  * see the `AUVLab/protobuf_cient` repository for the ROS based client node
* `iMOOSGateway` allows for non-MOOS systems to publish MOOS variables into the MOOSDB
* `iMOOSGateway` allows for the MOOSDB to publish MOOS variables into non-MOOS systems

`iMOOSGatway` is a generalized application. The user can configure which MOOS variables are to be forwarded to the non-MOOS client application, as shown in the ProcessConfig block below. The user can also specifically block MOOS variables from the client application as needed.

```
//------------------------------------------                                                            
// iMOOSGateway config block                                                                            
                                                                                                        
ProcessConfig = iMOOSGateway                                                                            
{                                                                                                       
  AppTick   = 4                                                                                         
  CommsTick = 4                                                                                         
                                                                                                        
  tcp_port = 1024                                                                                       
                                                                                                        
  // comma separated moos variables to send to client                                                   
  forward_to_client = DESIRED_HEADING, DESIRED_SPEED, DESIRED_RUDDER, DESIRED_THRUST, DEPLOY, IVPHELM_ST
ATE, IVPHELM_ALLSTOP, IVPHELM_MODESET, DB_TIME                                                         
                                                                                                        
  // comma separated moos variables to block from client                                                
  block_from_client = DEPLOY                                                                            
}  
```

`iMOOSGateway` runs a TCP server using `lib_gateway_tcp` (adopted from https://github.com/GobySoft/netsim/tree/master/src/lib/tcp), which allows the ROS based client (protobuf_client ROS node) to connect to as a TCP client, using the same library.

When `iMOOSGateway` receives a MOOS variable that is configured to be forwarded, depending on its type; i.e. string or double, the variable name and the content with be put into a `FromGateway` protobuf message, and sent over to the client using the TCP connection

When the client sends a `ToGateway` type protobuf message over the TCP link, unless if it's a blocked message, the `iMOOSGateway` application will publish it to the MOOSDB. The content of the incoming message could be a string or a double and will be posted to the MOOSDB as a `KEY-VALUE` pair. 


