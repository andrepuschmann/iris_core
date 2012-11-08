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
 * Main test file for MemoryManager element
 *
 *  Created on: 01-Dec-2008
 *  Created by: suttonp
 *  $Revision: 1346 $
 *  $LastChangedDate: 2011-09-19 18:24:26 +0100 (Mon, 19 Sep 2011) $
 *  $LastChangedBy: suttonp $
 */


#define BOOST_TEST_MODULE MemoryManagerTest

#include <boost/test/unit_test.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/timer.hpp>
#include <boost/random.hpp>

#include <algorithm>
#include <string>

#include "iris/MemoryManager.h"

using namespace std;
using namespace iris;

BOOST_AUTO_TEST_SUITE(MemoryManagerTest)

BOOST_AUTO_TEST_CASE(MemoryManagerBasics)
{
    MemoryManager* memManager = NULL;
    memManager = new MemoryManager;
    
    BOOST_REQUIRE(memManager != NULL);

    BOOST_CHECK_EQUAL(memManager->getTotalMemoryUsed(), 0);
    BOOST_CHECK_EQUAL(memManager->getTotalMemoryRequested(), 0);
    BOOST_CHECK_EQUAL(memManager->getTotalMemoryLocked(), 0);

    delete memManager;
}

//ToDo: Add test cases

BOOST_AUTO_TEST_SUITE_END()
