/**
 * \file ComponentCallbackInterface.h
 * \version 1.0
 *
 * \section COPYRIGHT
 *
 * Copyright 2012 The Iris Project Developers. See the
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
 * Interface provided to Iris Components by their owner.
 */

#ifndef COMPONENTCALLBACKINTERFACE_H_
#define COMPONENTCALLBACKINTERFACE_H_

#include <string>
#include "irisapi/Event.h"

namespace iris
{

/** The ComponentCallbackInterface interface is provided to all components by their owner
*  
*  This interface is used by components to pass events to their owner
*/
class ComponentCallbackInterface{
public:
  virtual ~ComponentCallbackInterface(){};

  virtual void activateEvent(Event &e) = 0;
};

} /* namespace iris */


#endif /* COMPONENTCALLBACKINTERFACE_H_ */
