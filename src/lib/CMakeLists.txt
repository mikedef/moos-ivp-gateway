##############################################################################
# FILE:        moos-ivp-ssci/src/lib/CMakeLists.txt
# DATE:        2022-08-23
# NAME:        mikedef
# DESCRIPTION: CMakeLists.txt file for the moos-ivp-ssci src/lib directory
##############################################################################

## copy headers to /include                                                                             
file(GLOB_RECURSE INCLUDE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h *.proto *.hpp)    
foreach(I ${INCLUDE_FILES})
  configure_file(${I} ${IVP_EXTEND_INC_DIR}/${I} @ONLY)                
endforeach()


#============================================================================
# Add the libraries in the current directory to the include path
#============================================================================
FILE(GLOB LOCAL_LIBRARY_DIRS ./lib_*)
INCLUDE_DIRECTORIES(${LOCAL_LIBRARY_DIRS})

#============================================================================
# List the subdirectories to build...
#============================================================================
ADD_SUBDIRECTORY(lib_gateway_tcp)
ADD_SUBDIRECTORY(lib_gateway_protobuf)



##############################################################################
#                           END of CMakeLists.txt
##############################################################################

