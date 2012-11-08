/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/*
 * file TestMain.cpp
 * Main test file for Controller element
 *
 *  Created on: 19-May-2008
 *  Created by: suttonp
 *  $Revision: 1346 $
 *  $LastChangedDate: 2011-09-19 18:24:26 +0100 (Mon, 19 Sep 2011) $
 *  $LastChangedBy: suttonp $
 */


#define BOOST_TEST_MODULE Controller

#include <boost/test/unit_test.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <string>

#include "TestController.h"

using namespace std;
using namespace iris;

BOOST_AUTO_TEST_SUITE (ControllerTests)

BOOST_AUTO_TEST_CASE(ControllerBasic)
{
}

BOOST_AUTO_TEST_SUITE_END()
