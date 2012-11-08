/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/*
 * file ControllerManagerTest.cpp
 * Main test file for ControllerManager element
 *
 *  Created on: 19-May-2009
 *  Created by: suttonp
 *  $Revision: 1367 $
 *  $LastChangedDate: 2011-11-11 20:13:58 +0000 (Fri, 11 Nov 2011) $
 *  $LastChangedBy: suttonp $
 */


#define BOOST_TEST_MODULE ControllerManagerTest

#include <boost/test/unit_test.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include "boost/filesystem.hpp"

#include <algorithm>
#include <string>

#include "iris/ControllerManager.h"

using namespace std;
using namespace iris;

BOOST_AUTO_TEST_SUITE (ControllerManagerTest)


BOOST_AUTO_TEST_CASE(ControllerManagerBasic)
{
	ControllerManager c;
}

BOOST_AUTO_TEST_SUITE_END()
