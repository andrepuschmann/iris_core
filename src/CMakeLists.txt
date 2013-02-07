#
# Copyright 2012 The Iris Project Developers. See the
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
ADD_LIBRARY(iris_lib STATIC ${iris_core_sources})
TARGET_LINK_LIBRARIES(iris_lib ticpp phyengine stackengine ${Boost_LIBRARIES})
IRIS_SET_PIC(iris_lib)

# Shared library (gets installed)
ADD_LIBRARY(iris_core SHARED Iris.cpp)
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
