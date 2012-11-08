/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file TestComponent.h
 * Declaration of class TestComponent
 *
 *  Created on: 23-Nov-2008
 *  Created by: jlotze
 *  $Revision: 1346 $
 *  $LastChangedDate: 2011-09-19 18:24:26 +0100 (Mon, 19 Sep 2011) $
 *  $LastChangedBy: suttonp $
 */

#ifndef TESTCOMPONENT_H_
#define TESTCOMPONENT_H_

#include "irisapi/ComponentBase.h"

namespace iris
{

/** test implementation of a component */
class TestComponent : public ComponentBase
{
private:
    bool x_debug;
    int x_number;
    std::string x_hello;
    float x_fraction;
    float x_range;

public:

    // metavector of types supported by component
    //typedef NumericDataTypes SupportedTypes;
    //typedef IntegerDataTypes SupportedTypes;
    //typedef signed_integer_data_types SupportedTypes;
    //typedef float_data_types SupportedTypes;
    //typedef complex_data_types SupportedTypes;
    //typedef boost::mpl::vector<float, int, long double>::type SupportedTypes;

    TestComponent() :
    ComponentBase("Test",  //name
     "TestComponent",   //type
     "This is just for a little bit of testing",  // description
     "Jorg Lotze", //author
     "1.0") // version
    {
        using namespace std;

    // format:          (name,   description,     default,   dynamic, parameter, list/Interval)
       registerParameter("debug", "whether to output debug messages", "false", true, x_debug);

       int number_array[] = {0, 5, 7, 9};
       list<int> num_allowed(number_array, number_array+4);

       registerParameter("number", "a number", "0", false, x_number, num_allowed);
       registerParameter("hello", "a stupid string", "hello world", false, x_hello);
       registerParameter("fraction", "a fraction for testing floats", "-1.3252e3", false, x_fraction);
       registerParameter("range", "testing a range of allowed values", "1", true, x_range, Interval<float>(0, 10));

	   registerEvent("testevent", "a simple event for testing", TypeInfo<uint32_t>::identifier);
    }

	void testEvents()
	{
		uint32_t x = 0;
		activateEvent("testevent", x); 
	}

    virtual ~TestComponent() {};
};

}

#endif /* TESTCOMPONENT_H_ */
