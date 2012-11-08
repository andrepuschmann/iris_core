/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file Event.h
 * The event object used to pass events within the IRIS2.0 system
 *
 *  Created on: 06-Aug-2009
 *  Created by: suttonp
 *  $Revision: 712 $
 *  $LastChangedDate: 2009-10-20 19:28:06 +0100 (Tue, 20 Oct 2009) $
 *  $LastChangedBy: lotzej $
 */

#ifndef EVENT_H_
#define EVENT_H_

#include <vector>
#include <boost/any.hpp>

namespace iris
{

struct Event
{
	//! the data passed with the event
	std::vector<boost::any> data;

	//! type of data passed with event
	int typeId;

	//! name of the event
    std::string eventName;

	//! name of the component which triggered the event
	std::string componentName;
};


} /* namespace iris */

#endif /* EVENT_H_ */

