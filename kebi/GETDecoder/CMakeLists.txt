# CMakeLists.txt for event package. It creates a library with dictionary and a main program
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(GETDecoder)

# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS Core RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics MathCore Thread)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

include_directories(
${ROOT_INCLUDE_DIRS}
${CMAKE_SOURCE_DIR}/source/GETCore
${CMAKE_SOURCE_DIR}/source/GETFormat
${CMAKE_SOURCE_DIR}/source/GETUtil
)
add_definitions(${ROOT_CXX_FLAGS})

ROOT_GENERATE_DICTIONARY(G__GETFormatDict 
${CMAKE_SOURCE_DIR}/source/GETFormat/GETBasicFrame.hh
${CMAKE_SOURCE_DIR}/source/GETFormat/GETBasicFrameHeader.hh
${CMAKE_SOURCE_DIR}/source/GETFormat/GETCoboFrame.hh
${CMAKE_SOURCE_DIR}/source/GETFormat/GETHeaderBase.hh
${CMAKE_SOURCE_DIR}/source/GETFormat/GETLayerHeader.hh
${CMAKE_SOURCE_DIR}/source/GETFormat/GETLayeredFrame.hh
${CMAKE_SOURCE_DIR}/source/GETFormat/GETTopologyFrame.hh
${CMAKE_SOURCE_DIR}/source/GETFormat/GETMutantFrame.hh
LINKDEF ${CMAKE_SOURCE_DIR}/source/GETFormat/libGETFormatLinkDef.h)

ROOT_GENERATE_DICTIONARY(G__GETUtilDict 
${CMAKE_SOURCE_DIR}/source/GETUtil/GETFileChecker.hh
${CMAKE_SOURCE_DIR}/source/GETUtil/GETMath.hh
LINKDEF ${CMAKE_SOURCE_DIR}/source/GETUtil/libGETUtilLinkDef.h)

ROOT_GENERATE_DICTIONARY(G__GETCoreDict 
${CMAKE_SOURCE_DIR}/source/GETCore/GETDecoder.hh
${CMAKE_SOURCE_DIR}/source/GETCore/GETFrameInfo.hh
LINKDEF ${CMAKE_SOURCE_DIR}/source/GETCore/libGETCoreLinkDef.h)

#---Create a shared library with geneated dictionary
add_library(GETDecoder SHARED
${CMAKE_SOURCE_DIR}/source/GETFormat/GETBasicFrame.cc
${CMAKE_SOURCE_DIR}/source/GETFormat/GETBasicFrameHeader.cc
${CMAKE_SOURCE_DIR}/source/GETFormat/GETCoboFrame.cc
${CMAKE_SOURCE_DIR}/source/GETFormat/GETHeaderBase.cc
${CMAKE_SOURCE_DIR}/source/GETFormat/GETLayerHeader.cc
${CMAKE_SOURCE_DIR}/source/GETFormat/GETLayeredFrame.cc
${CMAKE_SOURCE_DIR}/source/GETFormat/GETTopologyFrame.cc
${CMAKE_SOURCE_DIR}/source/GETFormat/GETMutantFrame.cc
G__GETFormatDict.cxx

${CMAKE_SOURCE_DIR}/source/GETUtil/GETFileChecker.cc
${CMAKE_SOURCE_DIR}/source/GETUtil/GETMath.cc
G__GETUtilDict.cxx

${CMAKE_SOURCE_DIR}/source/GETCore/GETDecoder.cc
${CMAKE_SOURCE_DIR}/source/GETCore/GETFrameInfo.cc
G__GETCoreDict.cxx
)
target_link_libraries(GETDecoder ${ROOT_LIBRARIES})
