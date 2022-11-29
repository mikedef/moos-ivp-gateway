##############################################################################
# FILE:        moos-ivp-gateway/src/CMakeLists.txt
# DATE:        2022-11-24
# NAME:	       mikedef
# DESCRIPTION: CMakeLists.txt file for the moos-ivp-gateway source directory
##############################################################################


#============================================================================
# Add the libraries in the current directory to the include path
#============================================================================
FILE(GLOB LOCAL_LIBRARY_DIRS ./lib_*)
INCLUDE_DIRECTORIES(${LOCAL_LIBRARY_DIRS})

#============================================================================
# List the subdirectories to build...
#============================================================================
#
ADD_SUBDIRECTORY(lib) # build your library first
ADD_SUBDIRECTORY(iMOOSGateway)

##############################################################################
#                           END of CMakeLists.txt
##############################################################################
