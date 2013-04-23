/**
 * \file Event.h
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
 * The Event object used to pass events within the Iris system
 */

#ifndef IRISAPI_EVENT_H_
#define IRISAPI_EVENT_H_

#include <vector>
#include <boost/any.hpp>

namespace iris
{

/** Event objects which can be activated by Components.
 *
 * Events can be registered and activated by Components to notify
 * Controllers that something has happened. This permits a Controller
 * to take action such as reconfiguring the radio.
 */
struct Event
{
  std::vector<boost::any> data;   ///< The data passed with the event.
  int typeId;                     ///< The type of data being passed.
  std::string eventName;          ///< The name of this event.
  std::string componentName;      ///< The name of the component which created it.
};

} /* namespace iris */

#endif /* IRISAPI_EVENT_H_ */

