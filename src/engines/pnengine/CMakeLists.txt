########################################################################
# Setup Dependencies
########################################################################
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

########################################################################
# Build the library from source files
########################################################################
SET(pnengine_sources
	PNComponentManager.cpp
	PNDataBuffer.h
	PNComponentManager.h
	PNEngine.cpp

)
ADD_LIBRARY(pnengine STATIC ${pnengine_sources})
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
  SET_TARGET_PROPERTIES(pnengine PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
TARGET_LINK_LIBRARIES(pnengine sharedlibrary)


