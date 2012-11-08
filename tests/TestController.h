/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file TestController.h
 * An example Controller for testing
 *
 *  Created on: 19-May-2009
 *  Created by: suttonp
 *  $Revision: 1346 $
 *  $LastChangedDate: 2011-09-19 18:24:26 +0100 (Mon, 19 Sep 2011) $
 *  $LastChangedBy: suttonp $
 */

#ifndef TESTCONTROLLER_H_
#define TESTCONTROLLER_H_

#include "irisapi/Controller.h"

namespace iris
{

/** test implementation of a controller */
class TestController : public Controller
{
private:

public:
    TestController() 
		: Controller("testcontroller",  //name
		"This is just for a little bit of testing",  // description
		"Paul Sutton", //author
		"1.0") // version
    {
    }

    virtual ~TestController() {};

	virtual void initialize(){};
	virtual void processEvent(){};
	virtual void destroy(){};
	virtual void subscribeToEvents(){};
};

} /* namespace iris */

#endif /* TESTCONTROLLER_H_ */
