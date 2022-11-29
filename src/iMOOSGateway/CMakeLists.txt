#--------------------------------------------------------
# The CMakeLists.txt for:                       iMOOSGateway
# Author(s):                              mikedef
#--------------------------------------------------------

SET(SRC
  MOOSGateway.cpp
  MOOSGateway_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(iMOOSGateway ${SRC})

TARGET_LINK_LIBRARIES(iMOOSGateway
   ${MOOS_LIBRARIES}
   ${Boost_LIBRARIES}
   apputil
   mbutil
   m
   pthread
   gateway_protobuf
   gateway_tcp)

