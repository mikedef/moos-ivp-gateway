#--------------------------------------------------------
# The CMakeLists.txt for:                       iMOOSClient
# Author(s):                              mikedef
#--------------------------------------------------------

SET(SRC
  MOOSClient.cpp
  MOOSClient_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(iMOOSClient ${SRC})

TARGET_LINK_LIBRARIES(iMOOSClient
   ${MOOS_LIBRARIES}
   ${Boost_LIBRARIES}
   apputil
   mbutil
   m
   pthread
   gateway_protobuf
   gateway_tcp)

