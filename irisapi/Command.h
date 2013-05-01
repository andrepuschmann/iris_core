/**
 * \file Command.h
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
 * The command object used to pass commands within the Iris system
 */

#ifndef IRISAPI_COMMAND_H_
#define IRISAPI_COMMAND_H_

#include <vector>
#include <boost/any.hpp>

namespace iris
{

struct Command
{
  std::vector<boost::any> data;   ///< Data passed with this command.
  int typeId;                     ///< Type of the data.
  std::string commandName;        ///< Name of this command.
  std::string componentName;      ///< Target component.
  std::string engineName;         ///< Target engine.
};


} // namespace iris

#endif // IRISAPI_COMMAND_H_

