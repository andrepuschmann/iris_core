/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file ControllerCallbackInterface.h
 * Interface provided to Controllers (by the ControllerManager)
 *
 *  Created on: 19-May-2009
 *  Created by: suttonp
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
 *
 */

#ifndef CONTROLLERCALLBACKINTERFACE_H_
#define CONTROLLERCALLBACKINTERFACE_H_

#include "irisapi/ReconfigurationDescriptions.h"
#include "irisapi/Command.h"

namespace iris
{

class Controller;

/** ControllerCallbackInterface is used by controllers to access functions on their owner
*
*	This interface allows the controller to subscribe to events on its owner and pass
*	reconfigurations to its owner.
*/
class ControllerCallbackInterface{
public:
    virtual ~ControllerCallbackInterface(){};

	virtual void reconfigureRadio(ReconfigSet reconfigs) = 0;
	virtual void postCommand(Command command) = 0;
	virtual std::string getParameterValue(std::string paramName, std::string componentName) = 0;
	virtual void subscribeToEvent(std::string eventName, std::string componentName, Controller *cont) = 0;
};

} /* namespace iris */


#endif /* CONTROLLERCALLBACKINTERFACE_H_ */
