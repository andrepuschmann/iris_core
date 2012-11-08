/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/*
 * file EngineManagerTest.cpp
 * Main test file for EngineManager element
 *
 *  Created on: 18-Dec-2008
 *  Created by: suttonp
 *  $Revision: 1367 $
 *  $LastChangedDate: 2011-11-11 20:13:58 +0000 (Fri, 11 Nov 2011) $
 *  $LastChangedBy: suttonp $
 */


#define BOOST_TEST_MODULE EngineManagerTest

#include <boost/test/unit_test.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include "boost/filesystem.hpp"

#include <algorithm>
#include <string>

#include "iris/EngineManager.h"

using namespace std;
using namespace iris;

BOOST_AUTO_TEST_SUITE (EngineManagerTest)


BOOST_AUTO_TEST_CASE(EngineManagerBasic)
{
    //Not really tests - just being called to check compilation
    EngineManager e;
}

BOOST_AUTO_TEST_SUITE_END()
