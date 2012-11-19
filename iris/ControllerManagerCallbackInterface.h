/**
 * \file ControllerManagerCallbackInterface.h
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
 * Interface provided to the ControllerManager.
 */

#ifndef CONTROLLERMANAGERCALLBACKINTERFACE_H_
#define CONTROLLERMANAGERCALLBACKINTERFACE_H_

#include "irisapi/ReconfigurationDescriptions.h"
#include "irisapi/Command.h"

namespace iris
{

/**
 * The ControllerManagerCallbackInterface interface is provided to the ControllerManager by its owner
 * This interface allows a ControllerManager to pass reconfigurations to its owner
 */
class ControllerManagerCallbackInterface{
public:
    virtual ~ControllerManagerCallbackInterface(){};

    virtual void reconfigureRadio(ReconfigSet reconfigs) = 0;

    virtual void postCommand(Command command) = 0;

    virtual std::string getParameterValue(std::string paramName, std::string componentName) = 0;
};

} /* namespace iris */


#endif /* CONTROLLERMANAGERCALLBACKINTERFACE_H_ */
