########################################################################
# Setup Dependencies
########################################################################
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

########################################################################
# Build the library from source files
########################################################################
IF(WIN32)
  ADD_LIBRARY(sharedlibrary SharedLibrary_windows.cpp)
ELSE(WIN32)
  FIND_PACKAGE(DL REQUIRED)
  INCLUDE_DIRECTORIES(${DL_INCLUDE_DIRS})
  ADD_LIBRARY(sharedlibrary STATIC SharedLibrary_posix.cpp)
  IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
    SET_TARGET_PROPERTIES(sharedlibrary PROPERTIES COMPILE_FLAGS -fPIC)
  ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
  TARGET_LINK_LIBRARIES(sharedlibrary ${DL_LIBRARIES})
ENDIF(WIN32)
