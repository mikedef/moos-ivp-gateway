# Repo for the iMOOSGateway application

## MOOS-IvP
* Please review the MOOS-IvP (MIT 2.680) class labs for further information about MOOS-IvP
  * https://oceanai.mit.edu/ivpman/pmwiki/pmwiki.php?n=Lab.HomePage2680

* Don't forget to add the path to this repo to the bashrc file!!
  * PATH=$PATH:~/moos-ivp-gateway/bin
  * export PATH

## iMOOSGateway

* MOOS application to connect to a client application, if used with the ROS node protobuf_client a ROS based client, to bridge key-value pairs between MOOS-IvP and a non-MOOS system
  * see the `mikedef/protobuf_client` or the `mikedef/protobuf_client_ros2` repository for the ROS based client node
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
                                                                                                        
  tcp_port = 9501                                                                                       
                                                                                                        
  // comma separated moos variables to send to client                                                   
  forward_to_client = DESIRED_HEADING, DESIRED_SPEED, DESIRED_RUDDER, DESIRED_THRUST, DEPLOY, IVPHELM_ST
ATE, IVPHELM_ALLSTOP, IVPHELM_MODESET, DB_TIME                                                         
                                                                                                        
  // comma separated moos variables to block from client                                                
  block_from_client = DEPLOY                                                                            
}  
```

`iMOOSGateway` runs a TCP server using `lib_gateway_tcp` (adopted from https://github.com/GobySoft/netsim/tree/master/src/lib/tcp), which allows the ROS based client (protobuf_client ROS node) to connect to as a TCP client, using the same library.

When `iMOOSGateway` receives a MOOS variable that is configured to be forwarded in the `forward_to_client` parameter of the `iMOOSGateway` configuration block, depending on its type; i.e. string or double, the variable name and the content with be put into a `FromGateway` protobuf message, and sent over to the client using the TCP connection. 

When the client sends a `ToGateway` type protobuf message over the TCP link, unless if it's a blocked message, the `iMOOSGateway` application will publish it to the MOOSDB. The content of the incoming message could be a string or a double and will be posted to the MOOSDB as a `KEY-VALUE` pair.

Data to be sent from the MOOSDB to the client (protobuf_client) will be sent automatically. Data received from the client will be published to the MOOSDB automatically. 

See `moos-ivp-gateway/src/lib/lib_gateway_protobuf` for more information into the gateway.proto message definition. Please note, the `gateway.proto` msg is defined in both the gateway and the client applications and must be identical. 

## Dependencies                                                                                         
* Google protocol buffers                                                                               
  * `sudo apt install protobuf-compiler`                                                                
* Base64 encode/decode                                                                                  
  * `sudo apt install libb64-dev` 

### Client Test: `protobuf_client` 
#### ROS 1
* `protobuf_client:` https://github.com/mikedef/protobuf_client

#### ROS 2
Sample test for when the Gateway is in use with the ROS 2 node `protobuf_client` https://github.com/mikedef/protobuf_client_ros2

* Set the tcp_port to the desired port address in the iMOOSGateway config block
* Launch the MOOS mission in 'moos-ivp-gateway/missions/moos-gateway-alpha'
  * `$ ./launch.sh`
* Set the `gateway_port` and `gateway_ip` in protobuf_client to match the iMOOSGateway settings. `gateway_port` must match `tcp_port`
* Launch the gateway client application in ROS 2
  * `$ ros2 run protobuf_client protobuf_client_node`
