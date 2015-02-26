/**
 * \file ControllerCallbackInterface.h
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
 * Interface provided to Controllers (by the ControllerManager).
 */

#ifndef IRISAPI_CONTROLLERCALLBACKINTERFACE_H_
#define IRISAPI_CONTROLLERCALLBACKINTERFACE_H_

#include "irisapi/ReconfigurationDescriptions.h"
#include "irisapi/Command.h"
#include "iris/RadioRepresentation.h"

namespace iris
{

class Controller;

/** ControllerCallbackInterface is used by controllers to access functions on their owner
*
*  This interface allows the controller to subscribe to events on and pass
*  reconfigurations to its owner (usually a ControllerManager).
*/
class ControllerCallbackInterface{
public:
  virtual ~ControllerCallbackInterface(){};

  virtual void reconfigureRadio(ReconfigSet reconfigs) = 0;
  virtual void postCommand(Command command) = 0;
  virtual std::string getParameterValue(std::string paramName, std::string componentName) = 0;
  virtual std::string getParameterName(std::string componentName, int paramIndex, std::string &paramValue) = 0;
  virtual void subscribeToEvent(std::string eventName, std::string componentName, Controller *cont) = 0;
  virtual void activateEvent(Event &e) = 0;
  virtual std::string getEngineName(std::string componentName, int *engineIndex, int *compIndex) = 0;
  virtual int getNrEngines() = 0;
  virtual int getNrComponents() = 0;
  virtual std::string getEngineNameFromIndex(int index) = 0;
  virtual std::string getComponentName(int index) = 0;
  virtual int getNrParameters(std::string componentName) = 0;
};

} /* namespace iris */


#endif /* IRISAPI_CONTROLLERCALLBACKINTERFACE_H_ */
