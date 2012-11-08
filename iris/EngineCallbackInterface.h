/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file EngineCallbackInterface.h
 * Interface provided to all engines.
 *
 *  Created on: 18-May-2009
 *  Created by: suttonp
 *  $Revision: 712 $
 *  $LastChangedDate: 2009-10-20 19:28:06 +0100 (Tue, 20 Oct 2009) $
 *  $LastChangedBy: lotzej $
 *
 */

#ifndef ENGINECALLBACKINTERFACE_H_
#define ENGINECALLBACKINTERFACE_H_

#include "irisapi/Event.h"

namespace iris
{

/** The EngineCallbackInterface interface is provided to all Engines by their owner
*
*	This interface allows an engine to pass events to their owner
*/
class EngineCallbackInterface{
public:
    virtual ~EngineCallbackInterface(){};
	virtual void activateEvent(Event &e) = 0;
};

} /* namespace iris */


#endif /* ENGINECALLBACKINTERFACE_H_ */
