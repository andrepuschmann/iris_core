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

# - Find dl library
# Find the native DL includes and library
#
#  DL_INCLUDE_DIR - where to find dlfcn.h, etc.
#  DL_LIBRARIES   - List of libraries when using dl.
#  DL_FOUND       - True if dl found.


IF (DL_INCLUDE_DIR)
  # Already in cache, be silent
  SET(DL_FIND_QUIETLY TRUE)
ENDIF (DL_INCLUDE_DIR)

FIND_PATH(DL_INCLUDE_DIR dlfcn.h)

SET(DL_NAMES dl libdl ltdl libltdl)
FIND_LIBRARY(DL_LIBRARY NAMES ${DL_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set DL_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DL DEFAULT_MSG DL_LIBRARY DL_INCLUDE_DIR)

IF(DL_FOUND)
  SET( DL_LIBRARIES ${DL_LIBRARY} )
ELSE(DL_FOUND)
  SET( DL_LIBRARIES )
ENDIF(DL_FOUND)

MARK_AS_ADVANCED( DL_LIBRARY DL_INCLUDE_DIR )

