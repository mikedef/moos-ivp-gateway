#=======================================================================
# FILE:  moos-ivp-gateway/CMakeLists.txt
# DATE:  2022-11-24
# INFO:  Top-level CMakeLists.txt file for the moos-ivp-gateway project
# NAME:  Maintained by Supun Randeni, Mike DeFilippo - Original setup by Christian Convey
#        Chris Gagner, and tips borrowed from Dave Billin
#=======================================================================

CMAKE_MINIMUM_REQUIRED(VERSION 3.3)
PROJECT( moos_ivp_gateway )
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")

set (CMAKE_CXX_STANDARD 11)

#=======================================================================
# Set the output directories for the binary and library files
#=======================================================================

GET_FILENAME_COMPONENT(IVP_EXTEND_BIN_DIR "${CMAKE_SOURCE_DIR}/bin"  ABSOLUTE )
GET_FILENAME_COMPONENT(IVP_EXTEND_LIB_DIR "${CMAKE_SOURCE_DIR}/lib"  ABSOLUTE )
GET_FILENAME_COMPONENT(IVP_EXTEND_INC_DIR "${CMAKE_SOURCE_DIR}/include" ABSOLUTE)

SET( LIBRARY_OUTPUT_PATH      "${IVP_EXTEND_LIB_DIR}" CACHE PATH "" )
SET( ARCHIVE_OUTPUT_DIRECTORY "${IVP_EXTEND_LIB_DIR}" CACHE PATH "" )
SET( LIBRARY_OUTPUT_DIRECTORY "${IVP_EXTEND_LIB_DIR}" CACHE PATH "" )

SET( EXECUTABLE_OUTPUT_PATH    "${IVP_EXTEND_BIN_DIR}" CACHE PATH "" )
SET( RUNTIME_OUTPUT_DIRECTORY "${IVP_EXTEND_BIN_DIR}"  CACHE PATH "" )

include_directories(${IVP_EXTEND_INC_DIR})

#=======================================================================
# Find Protocol buffers
#=======================================================================

# protocol buffers - http://code.google.com/apis/protocolbuffers/
find_package(ProtobufLocal REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

if(PROTOC_VERSION VERSION_LESS "2.5.0")
  message("libprotobuf < 2.5.0")
else()
  message("libprotobuf >= 2.5.0")
  set(PROTOBUF_ALLOW_ALIAS "option allow_alias = true;")
endif()

#=======================================================================
# Find MOOS
#=======================================================================
find_package(MOOS 10.0)
INCLUDE_DIRECTORIES(${MOOS_INCLUDE_DIRS})

#=============================================================================
# FINDING MOOSGeodesy' HEADERS AND LIBRARIES...
#=============================================================================
find_package(MOOSGeodesy)
include_directories(${MOOSGeodesy_INCLUDE_DIRS})
link_directories(${MOOSGeodesy_LIBRARY_PATH})

message("+++++++++++++++++++++++++++++++++++++++++")
message("MOOSGeodesy_INCLUDE_DIRS:" ${MOOSGeodesy_INCLUDE_DIRS})
message("MOOSGeodesy_LIB_PATH:"     ${MOOSGeodesy_LIBRARY_PATH})
message("+++++++++++++++++++++++++++++++++++++++++")

#=======================================================================
# Locate moos-ivp (prefer explicit env var; optionally fall back to pkg-config)
#=======================================================================

# Allow users to pass -DMOOSIVP_SOURCE_TREE_BASE=... or set the env var
if(NOT MOOSIVP_SOURCE_TREE_BASE AND DEFINED ENV{MOOSIVP_SOURCE_TREE_BASE})
  set(MOOSIVP_SOURCE_TREE_BASE "$ENV{MOOSIVP_SOURCE_TREE_BASE}")
endif()

# Optional: try pkg-config as a hint if no env var/preset dir is given
if(NOT MOOSIVP_SOURCE_TREE_BASE)
  find_package(PkgConfig QUIET)
  if(PkgConfig_FOUND)
    pkg_check_modules(IVP QUIET moosivp)
    # If a moosivp.pc exists, IVP_INCLUDE_DIRS / IVP_LIBRARY_DIRS become available
  endif()
endif()

#======================================================================
# Where to find IvP headers/libs (source tree OR pkg-config)
#======================================================================

if(MOOSIVP_SOURCE_TREE_BASE)
  file(GLOB IVP_INCLUDE_DIRS "${MOOSIVP_SOURCE_TREE_BASE}/ivp/src/lib_*")
  include_directories(${IVP_INCLUDE_DIRS})

  file(GLOB IVP_LIBRARY_DIRS "${MOOSIVP_SOURCE_TREE_BASE}/lib")
  link_directories(${IVP_LIBRARY_DIRS})

elseif(IVP_FOUND)
  # Use pkg-config hints only if source-tree base is not provided
  include_directories(${IVP_INCLUDE_DIRS})
  link_directories(${IVP_LIBRARY_DIRS})
  # (Optionally you can also use IVP_LIBRARIES with target_link_libraries later.)

else()
  message(FATAL_ERROR
    "moos-ivp not located. Set MOOSIVP_SOURCE_TREE_BASE (env or -DMOOSIVP_SOURCE_TREE_BASE=...), "
    "or ensure a 'moosivp.pc' is installed and visible to pkg-config.")
endif()

#=======================================================================
# Find the "Boost"
#=======================================================================

# boost - http://www.boost.org/
#find_package(Boost 1.55.0 REQUIRED date_time thread system program_options filesystem regex serializati
#on)

#include_directories(${Boost_INCLUDE_DIRS})



#=======================================================================
# Find the "B64"
#=======================================================================

# b64 for the interface
find_package(B64 REQUIRED)
set(B64_DOC_STRING "Enable base64 functionality (requires libb64-dev: http://libb64.sourceforge.net/")
if(B64_FOUND)
  option(enable_b64 ${B64_DOC_STRING} ON)
else()
  option(enable_b64 ${B64_DOC_STRING} OFF)
  message(">> setting enable_b64 to OFF ... if you need this functionality: 1) install libb64-dev; 2) ru
n cmake -Denable_b64=ON")
endif()
include_directories(${B64_INCLUDE_DIRS})


#======================================================================
# Specify Compiler Flags
#======================================================================
IF( ${WIN32} )
   #---------------------------------------------
   # Windows Compiler Flags
   #---------------------------------------------
   IF(MSVC)
      # Flags for Microsoft Visual Studio
      SET( WALL_ON OFF CACHE BOOL
         "tell me about all compiler warnings (-Wall) ")
      IF(WALL_ON)
         SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
      ENDIF(WALL_ON)
   ELSE(MSVC)
      # Other Windows compilers go here
   ENDIF(MSVC)

ELSE( ${WIN32} )
   #---------------------------------------------
   # Linux and Apple Compiler Flags
   #---------------------------------------------
   # Force -fPIC because gcc complains when we don't use it with x86_64 code.
   # Note sure why: -fPIC should only be needed for shared objects, and
   # AFAIK, CMake gets that right when building shared objects. -CJC
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -g -Wdeprecated-declarations")
   IF(CMAKE_COMPILER_IS_GNUCXX)
      # Flags for the GNU C++ Compiler
      SET( WALL_ON OFF CACHE BOOL
         "tell me about all compiler warnings (-Wall) ")
      IF(WALL_ON)
         SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" -C++11)
      ENDIF( WALL_ON)
   ELSE(CMAKE_COMPILER_IS_GNUCXX)

   ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ENDIF( ${WIN32} )

#=======================================================================
# Add Subdirectories
#=======================================================================
ADD_SUBDIRECTORY( src )
