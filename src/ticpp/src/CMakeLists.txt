########################################################################
# Setup Dependencies
########################################################################
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/ticpp/include)

########################################################################
# Build the library from source files
########################################################################
SET(ticpp_sources
	ticpp.cpp
	tinyxml.cpp
	tinyxmlparser.cpp
	tinystr.cpp
	tinyxmlerror.cpp
)
ADD_LIBRARY(ticpp STATIC ${ticpp_sources})
SET_TARGET_PROPERTIES(ticpp PROPERTIES COMPILE_FLAGS "-DTIXML_USE_TICPP")
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
  SET_TARGET_PROPERTIES(ticpp PROPERTIES COMPILE_FLAGS "-DTIXML_USE_TICPP -fPIC")
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
