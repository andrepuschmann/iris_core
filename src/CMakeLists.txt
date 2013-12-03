#
# Copyright 2012-2013 The Iris Project Developers. See the
# COPYRIGHT file at the top-level directory of this distribution
# and at http://www.softwareradiosystems.com/iris/copyright.html.
#
# This file is part of the Iris Project.
#
# Iris is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# Iris is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# A copy of the GNU Lesser General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

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
# Build the libraries from source files
########################################################################
SET(iris_core_sources
    ${PROJECT_SOURCE_DIR}/irisapi/Version.h
    ${PROJECT_SOURCE_DIR}/irisapi/Command.h
    ${PROJECT_SOURCE_DIR}/irisapi/CommandPrison.h
    ${PROJECT_SOURCE_DIR}/irisapi/ComponentBase.h
    ${PROJECT_SOURCE_DIR}/irisapi/ComponentCallbackInterface.h
    ${PROJECT_SOURCE_DIR}/irisapi/ComponentEvents.h
    ${PROJECT_SOURCE_DIR}/irisapi/ComponentInformation.h
    ${PROJECT_SOURCE_DIR}/irisapi/ComponentPorts.h
    ${PROJECT_SOURCE_DIR}/irisapi/ModuleParameters.h
    ${PROJECT_SOURCE_DIR}/irisapi/Controller.h
    ${PROJECT_SOURCE_DIR}/irisapi/ControllerCallbackInterface.h
    ${PROJECT_SOURCE_DIR}/irisapi/DataBufferInterfaces.h
    ${PROJECT_SOURCE_DIR}/irisapi/StackDataBuffer.h
    ${PROJECT_SOURCE_DIR}/irisapi/Event.h
    ${PROJECT_SOURCE_DIR}/irisapi/Exceptions.h
    ${PROJECT_SOURCE_DIR}/irisapi/Interval.h
    ${PROJECT_SOURCE_DIR}/irisapi/LibraryDefs.h
    ${PROJECT_SOURCE_DIR}/irisapi/LinkDescription.h
    ${PROJECT_SOURCE_DIR}/irisapi/MessageQueue.h
    ${PROJECT_SOURCE_DIR}/irisapi/TypeInfo.h
    ${PROJECT_SOURCE_DIR}/irisapi/TypeVectors.h
    ${PROJECT_SOURCE_DIR}/irisapi/ParameterTypeInfo.h
    ${PROJECT_SOURCE_DIR}/irisapi/PhyComponent.h
    ${PROJECT_SOURCE_DIR}/irisapi/StackComponent.h
    ${PROJECT_SOURCE_DIR}/irisapi/TemplatePhyComponent.h
    ${PROJECT_SOURCE_DIR}/irisapi/ReconfigurationDescriptions.h
    ${PROJECT_SOURCE_DIR}/irisapi/Logging.h

    ${PROJECT_SOURCE_DIR}/iris/XmlParser.h
    ${PROJECT_SOURCE_DIR}/iris/SharedLibrary.h
    ${PROJECT_SOURCE_DIR}/iris/MemoryManager.h
    ${PROJECT_SOURCE_DIR}/iris/DataBuffer.h
    ${PROJECT_SOURCE_DIR}/iris/ControllerManager.h
    ${PROJECT_SOURCE_DIR}/iris/ControllerManagerCallbackInterface.h
    ${PROJECT_SOURCE_DIR}/iris/RadioRepresentation.h
    ${PROJECT_SOURCE_DIR}/iris/EngineManager.h
    ${PROJECT_SOURCE_DIR}/iris/EngineInterface.h
    ${PROJECT_SOURCE_DIR}/iris/EngineCallbackInterface.h
    ${PROJECT_SOURCE_DIR}/iris/ReconfigurationManager.h
    ${PROJECT_SOURCE_DIR}/iris/System.h
    ${PROJECT_SOURCE_DIR}/iris/PhyEngine.h
    ${PROJECT_SOURCE_DIR}/iris/StackEngine.h
    ${PROJECT_SOURCE_DIR}/iris/Iris.h

    QtWrapper.h
    MemoryManager.cpp
    XmlParser.cpp
    ControllerManager.cpp
    RadioRepresentation.cpp
    EngineManager.cpp
    ReconfigurationManager.cpp
    System.cpp
)

SET(qt_sources)
IF(QT4_FOUND AND QWT_FOUND)
    QT4_WRAP_CPP(qt_sources QtWrapper.h)
ENDIF(QT4_FOUND AND QWT_FOUND)

# Static library (linked by testcode, shared lib and iris executable)
ADD_LIBRARY(iris_lib STATIC ${iris_core_sources})
TARGET_LINK_LIBRARIES(iris_lib ticpp phyengine stackengine ${Boost_LIBRARIES})
IRIS_SET_PIC(iris_lib)

# Shared library (can be used by alternative launchers)
ADD_LIBRARY(iris_core SHARED Iris.cpp)
TARGET_LINK_LIBRARIES(iris_core iris_lib)
INSTALL(TARGETS iris_core
    LIBRARY DESTINATION ${LIBRARY_DIR} # .so file
    ARCHIVE DESTINATION ${LIBRARY_DIR} # .lib file
    RUNTIME DESTINATION ${RUNTIME_DIR} # .dll file
)

# Console executable 
ADD_EXECUTABLE (iris Launcher.cpp IrisStateMachine.cpp Iris.cpp ${qt_sources})
IF(QT4_FOUND AND QWT_FOUND)
    TARGET_LINK_LIBRARIES (iris iris_lib ${QT_LIBRARIES} ${QWT_LIBRARIES})
ELSE(QT4_FOUND AND QWT_FOUND)
    TARGET_LINK_LIBRARIES (iris iris_lib)
ENDIF(QT4_FOUND AND QWT_FOUND)

INSTALL(TARGETS iris RUNTIME DESTINATION ${RUNTIME_DIR})
