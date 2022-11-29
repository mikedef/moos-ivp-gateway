add_library(gateway_tcp SHARED
  tcp_client.cpp
  tcp_server.cpp
  tcp_session.cpp)

target_link_libraries(gateway_tcp 
  ${Boost_LIBRARIES}
  protobuf 
  ${B64_LIBRARIES})


set_target_properties(gateway_tcp PROPERTIES VERSION "${PROJECT_VERSION}" SOVERSION "${PROJECT_SOVERSION}")

