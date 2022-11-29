#--------------------------------------------------------
# The CMakeLists.txt for:               lib_gateway
# Author(s):				mikedef                               
#--------------------------------------------------------

file(GLOB PROTOS *.proto)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS "${IVP_EXTEND_INC_DIR}/lib_gateway_protobuf" ${PROTOS})

file(GLOB SRC *.cpp *.c)

add_library(gateway_protobuf ${BUILD_STATIC_OR_SHARED_LIBRARIES} ${SRC} ${PROTO_SRCS} ${PROTO_HDRS})

target_link_libraries(gateway_protobuf 
  ${PROTOBUF_LIBRARIES}
)
set_target_properties(gateway_protobuf PROPERTIES VERSION "${PROJECT_VERSION}" SOVERSION "${PROJECT_SOVERSION}")
