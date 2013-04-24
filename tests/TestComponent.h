 /**
 * \file TestComponent.h
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
 * A basic component used for testing.
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

  // format:      (name,   description,   default,   dynamic, parameter, list/Interval)
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
