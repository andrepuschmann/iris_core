########################################################################
# Setup Dependencies
########################################################################
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

########################################################################
# Build the library from source files
########################################################################
SET(stackengine_sources
	StackOutTranslator.cpp
	StackOutTranslator.h
	StackInTranslator.cpp
	StackInTranslator.h
	StackComponentManager.cpp
	StackComponentManager.h
	StackEngine.cpp
)
ADD_LIBRARY(stackengine STATIC ${stackengine_sources})
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
  SET_TARGET_PROPERTIES(stackengine PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
TARGET_LINK_LIBRARIES(stackengine sharedlibrary)


