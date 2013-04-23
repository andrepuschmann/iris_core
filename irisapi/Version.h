/**
 * \file Version.h
 * \version 1.0
 *
 * \section COPYRIGHT
 *
 * Copyright 2012-2013 The Iris Project Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution
 * and at http://www.softwareradiosystems.com/iris/copyright.html.
 *
 * \section LICENSE
 *
 * This file is part of the Iris Project.
 *
 * Iris is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Iris is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 * \section DESCRIPTION
 *
 * A simple version number system to ensure core and modules are built
 * against the same API.
 */

#ifndef IRISAPI_VERSION_H_
#define IRISAPI_VERSION_H_

namespace iris
{

/** Used to get the version number of this Iris API.
 *
 * Whenever an Iris module is loaded by the core, the version number
 * of the API it was built against is compared with this version number
 * to ensure compatibility. The version number is updated whenever the
 * API changes.
 */
class Version
{
public:
  static const char* getApiVersion()
  {
    return "0.0.19";
  };
};

} // namespace iris

#endif // IRISAPI_VERSION_H_

