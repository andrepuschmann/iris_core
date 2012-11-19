/**
 * \file Component_test.cpp
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
 * Main test file for Component class
 */

#define BOOST_TEST_MODULE Component

#include <boost/test/unit_test.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <algorithm>
#include <string>

#include "TestComponent.h"

using namespace std;
using namespace iris;

BOOST_AUTO_TEST_SUITE (Component)

BOOST_AUTO_TEST_CASE(ComponentInformation)
{
    ComponentBase* comp = NULL;
    comp = new TestComponent;
    
    BOOST_REQUIRE(comp != NULL);

    BOOST_CHECK_EQUAL(comp->getName(), "Test");
    BOOST_CHECK_EQUAL(comp->getType(), "TestComponent");
    BOOST_CHECK_EQUAL(comp->getDescription(), "This is just for a little bit of testing");
    BOOST_CHECK_EQUAL(comp->getAuthor(), "Jorg Lotze");
    BOOST_CHECK_EQUAL(comp->getVersion(), "1.0");

    delete comp;
}

BOOST_AUTO_TEST_CASE(ComponentGetParameterInfo)
{
    ComponentBase* comp = NULL;
    BOOST_CHECK_NO_THROW(comp = new TestComponent);
    
    BOOST_REQUIRE(comp != NULL);

    string xml = comp->getDefaultXML();
    BOOST_CHECK_EQUAL(xml.substr(0,11), "\t<parameter");

    BOOST_CHECK_EQUAL(comp->getNumParameters(), (size_t)5);

    BOOST_CHECK_EQUAL(comp->getParameterDataType("debug"), "bool");
    BOOST_CHECK_EQUAL(comp->getParameterDataType("number"), "signed int");
    BOOST_CHECK_EQUAL(comp->getParameterDataType("hello"), "std::string");
    BOOST_CHECK_EQUAL(comp->getParameterDataType("fraction"), "float");
    BOOST_CHECK_EQUAL(comp->getParameterDataType("range"), "float");

    BOOST_CHECK_EQUAL(comp->getParameterDefaultValue("debug"), comp->getValue("debug"));
    BOOST_CHECK_EQUAL(comp->getParameterDefaultValue("number"), comp->getValue("number"));
    BOOST_CHECK_EQUAL(comp->getParameterDefaultValue("range"), comp->getValue("range"));
    BOOST_CHECK_EQUAL(comp->getParameterDefaultValue("hello"), comp->getValue("hello"));

    string fracstr = comp->getParameterDefaultValue("fraction");
    stringstream sstr(fracstr);
    float fraction;
    sstr >> fraction;
    float fraction2;
    comp->getValue("fraction", &fraction2);
    BOOST_CHECK_EQUAL(fraction, fraction2);

    BOOST_CHECK_EQUAL(comp->getParameterDescription("debug"), "whether to output debug messages");
    BOOST_CHECK_EQUAL(comp->isParameterDynamic("debug"), true);


    delete comp;
}



//! test function for parameterised tests in testcase below
template <typename T>
void parTest (ComponentBase* comp, string name, T val)
{
   comp->setValue(name, val);

   T val1;
   comp->getValue(name, &val1);
   BOOST_CHECK_EQUAL(val1, val);
}



BOOST_AUTO_TEST_CASE (ComponentParametersGetSet)
{
    ComponentBase* comp = NULL;
    BOOST_CHECK_NO_THROW(comp = new TestComponent);
    
    BOOST_REQUIRE(comp != NULL);

    // Test Get/Set of a lot of floats
    float fvalues[10000];
    for (int i = 0; i < 10000; i++)
    {
        fvalues[i] = (float(i) * 21.0f - i) / 23.234f;

    }

    // create a function object for parTest(float val) (bind first par to comp, second to "fraction")
    boost::function<void (float)> testf = boost::bind(&parTest<float>, comp, "fraction", _1);

    // there is something wrong with the parameterised test - have to do it with for_each
    std::for_each(fvalues, fvalues+10000, testf);


   BOOST_CHECK_THROW(comp->setValue("bla", 12), ParameterNotFoundException);

   // create a function object for parTest(int val) (bind first par to comp, second to "number")
   boost::function<void (int)> inttester = boost::bind(&parTest<int>, comp, "number", _1);
   int testvals[] = {0, 5, 7, 9};

   std::for_each(testvals, testvals+4, inttester);

   BOOST_CHECK_THROW(inttester(3), ParameterOutOfRangeException);
   BOOST_CHECK_THROW(comp->setValue("number", 4.3), InvalidDataTypeException);

   parTest<bool>(comp, "debug", false);
   parTest<bool>(comp, "debug", true);

   boost::function<void (float)> rangetester = boost::bind(&parTest<float>, comp, "range", _1);

   float fvalues2[] = {0.0f, 2.0f, 2.34f, 5.2f, 6.0f, 10.0f};
   for_each(fvalues2, fvalues2+6, rangetester);

   BOOST_CHECK_THROW(rangetester(-0.5), ParameterOutOfRangeException);

   delete comp;
}

BOOST_AUTO_TEST_CASE (ComponentParametersExceptions)
{
    ComponentBase* comp = NULL;
    
    BOOST_CHECK_NO_THROW(comp = new TestComponent);

    BOOST_REQUIRE(comp != NULL);

    BOOST_CHECK_THROW(comp->getValue("dummy"), ParameterNotFoundException);


    int intt;
    BOOST_CHECK_THROW(comp->getValue("dummy", &intt), ParameterNotFoundException);
    BOOST_CHECK_THROW(comp->setValue("dummy", intt), ParameterNotFoundException);
    BOOST_CHECK_THROW(comp->setValue("debug", intt), InvalidDataTypeException);
    BOOST_CHECK_THROW(comp->setValue("range", 5), InvalidDataTypeException);
    BOOST_CHECK_THROW(comp->setValue("range", -12.0f), ParameterOutOfRangeException);

    BOOST_CHECK_THROW(comp->setValue("number", 2), ParameterOutOfRangeException);
    BOOST_CHECK_THROW(comp->setValue("number", "12"), ParameterOutOfRangeException);
    BOOST_CHECK_THROW(comp->setValue("debug", "hello"), InvalidDataTypeException);
    BOOST_CHECK_THROW(comp->setValue("range", "-2"), ParameterOutOfRangeException);
    BOOST_CHECK_THROW(comp->setValue("number", "2"), ParameterOutOfRangeException);
    BOOST_CHECK_THROW(comp->getParameterDefaultValue("dummy"), ParameterNotFoundException);

    BOOST_CHECK_THROW(comp->getParameterDescription("dummy"), ParameterNotFoundException);
    BOOST_CHECK_THROW(comp->getParameterDataType("dummy"), ParameterNotFoundException);
    BOOST_CHECK_THROW(comp->isParameterDynamic("dummmy"), ParameterNotFoundException);

    BOOST_CHECK_THROW(comp->getValue("debug", &intt), InvalidDataTypeException);
    BOOST_CHECK_THROW(comp->getValue("dummy", &intt), ParameterNotFoundException);

    //parTest(comp, "number", (short)12);

    delete comp;
}


BOOST_AUTO_TEST_SUITE_END()
