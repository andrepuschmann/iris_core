/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file ComponentCallbackInterface.h
 * Interface provided to components by their owner
 *
 *  Created on: 19-May-2009
 *  Created by: suttonp
 *  $Revision: 712 $
 *  $LastChangedDate: 2009-10-20 19:28:06 +0100 (Tue, 20 Oct 2009) $
 *  $LastChangedBy: lotzej $
 *
 */

#ifndef COMPONENTCALLBACKINTERFACE_H_
#define COMPONENTCALLBACKINTERFACE_H_

#include <string>
#include "irisapi/Event.h"

namespace iris
{

/** The ComponentCallbackInterface interface is provided to all components by their owner
*	
*	This interface is used by components to pass events to their owner
*/
class ComponentCallbackInterface{
public:
    virtual ~ComponentCallbackInterface(){};

	virtual void activateEvent(Event &e) = 0;
};

} /* namespace iris */


#endif /* COMPONENTCALLBACKINTERFACE_H_ */
