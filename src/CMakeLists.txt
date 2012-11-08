########################################################################
# Add the subdirectories
########################################################################
# Recurse into subdirectories. This does not actually cause another cmake 
# executable to run. The same process will walk through the project's 
# entire directory structure.
ADD_SUBDIRECTORY(ticpp)
ADD_SUBDIRECTORY(sharedlibrary)
ADD_SUBDIRECTORY(engines)

########################################################################
# Add includes and dependencies
########################################################################
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/ticpp/include)

########################################################################
# Build the library from source files
########################################################################
SET(iris_core_sources
	XmlParser.cpp
    MemoryManager.cpp
	ControllerManager.cpp
	RadioRepresentation.cpp
	EngineManager.cpp
	ReconfigurationManager.cpp
	System.cpp
)

# Static library (test code can link against this)
ADD_LIBRARY(iris_lib ${iris_core_sources})
TARGET_LINK_LIBRARIES(iris_lib ticpp pnengine stackengine ${Boost_LIBRARIES})
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
  SET_TARGET_PROPERTIES(iris_lib PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )

# Shared library (gets installed)
ADD_LIBRARY(iris_core SHARED IRISV2.cpp)
TARGET_LINK_LIBRARIES(iris_core iris_lib)

INSTALL(TARGETS iris_core
    LIBRARY DESTINATION ${LIBRARY_DIR} # .so file
    ARCHIVE DESTINATION ${LIBRARY_DIR} # .lib file
    RUNTIME DESTINATION ${RUNTIME_DIR} # .dll file
)

########################################################################
# Add executables and link to libraries
########################################################################
ADD_EXECUTABLE (iris Launcher.cpp IrisStateMachine.cpp)
TARGET_LINK_LIBRARIES (iris iris_core)

INSTALL(TARGETS iris RUNTIME DESTINATION ${RUNTIME_DIR})
