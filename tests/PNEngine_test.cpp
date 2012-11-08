/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/*
 * file PNEngineTest.cpp
 * Main test file for XmlParser element
 *
 *  Created on: 18-Dec-2008
 *  Created by: suttonp
 *  $Revision: 1346 $
 *  $LastChangedDate: 2011-09-19 18:24:26 +0100 (Mon, 19 Sep 2011) $
 *  $LastChangedBy: suttonp $
 */


#define BOOST_TEST_MODULE PNEngineTest

#include <boost/test/unit_test.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#include <algorithm>
#include <string>

#include "iris/PNEngine.h"

using namespace std;
using namespace iris;

BOOST_AUTO_TEST_SUITE (PNEngineTest)


BOOST_AUTO_TEST_CASE(PNEngineBasic)
{
    PNEngine thePNEngine("MyEngine", ".");
}

BOOST_AUTO_TEST_SUITE_END()
