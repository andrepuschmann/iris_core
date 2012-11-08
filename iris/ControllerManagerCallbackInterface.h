/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file ControllerManagerCallbackInterface.h
 * Interface provided to the ControllerManager
 *
 *  Created on: 19-May-2009
 *  Created by: suttonp
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
 *
 */

#ifndef CONTROLLERMANAGERCALLBACKINTERFACE_H_
#define CONTROLLERMANAGERCALLBACKINTERFACE_H_

#include "irisapi/ReconfigurationDescriptions.h"
#include "irisapi/Command.h"

namespace iris
{

/** The ControllerManagerCallbackInterface interface is provided to the ControllerManager by its owner
*
*	This interface allows a ControllerManager to pass reconfigurations to its owner
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
