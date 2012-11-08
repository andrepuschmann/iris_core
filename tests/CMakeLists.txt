########################################################################
# unit test suite
########################################################################


########################################################################
# Add includes and dependencies
########################################################################
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

########################################################################
# Build the test library used by SharedLibrary_test.cpp
########################################################################
ADD_LIBRARY(testlibrary SHARED TestLibrary.cpp)
INSTALL(TARGETS testlibrary DESTINATION ${DATA_DIR}/tests)
CONFIGURE_FILE(config_tests.h.in ${CMAKE_CURRENT_BINARY_DIR}/config_tests.h)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

########################################################################
# Build executables, register as tests, and install
########################################################################
# Build each test and link to libraries
SET(test_sources
	CommandPrison_test.cpp
	Component_test.cpp
	ControllerManager_test.cpp
	Controller_test.cpp
	DataBuffer_test.cpp
	DataTypes_test.cpp
	EngineManager_test.cpp
	Logging_test.cpp
	MemoryManager_test.cpp
	PNEngine_test.cpp
	RadioRepresentation_test.cpp
	ReconfigurationManager_test.cpp
	SharedLibrary_test.cpp
	StackEngine_test.cpp
	System_test.cpp
	XmlParser_test.cpp
)

#turn each test cpp file into an executable with an int main() function
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK -DBOOST_TEST_MAIN)

#for each source: build an executable, register it as a test, and install
FOREACH(test_source ${test_sources})
    GET_FILENAME_COMPONENT(test_name ${test_source} NAME_WE)
    ADD_EXECUTABLE(${test_name} ${test_source})
    TARGET_LINK_LIBRARIES(${test_name} iris_lib)
    ADD_TEST(${test_name} ${test_name})
    INSTALL(TARGETS ${test_name} RUNTIME DESTINATION ${DATA_DIR}/tests)
ENDFOREACH(test_source)

