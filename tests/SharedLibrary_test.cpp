/*
 * This file is part of Iris 2.
 *
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin.
 * All rights reserved.
 *
 */

/*
 * file SharedLibrary_test.cpp
 * Main test file for SharedLibrary element
 *
 *  Created on: 21-Nov-2011
 *  Created by:
 *  $Revision: 1380 $
 *  $LastChangedDate: 2011-11-23 13:03:24 +0000 (Wed, 23 Nov 2011) $
 *  $LastChangedBy: suttonp $
 */


#define BOOST_TEST_MODULE SharedLibraryTest

#include <boost/test/unit_test.hpp>
#include "boost/filesystem.hpp"
#include <iostream>

#include "iris/SharedLibrary.h"
#include "config_tests.h"

using namespace std;
using namespace iris;
namespace bfs=boost::filesystem;

typedef string (*TESTFUNCTION)();

BOOST_AUTO_TEST_SUITE (SharedLibraryTest)

BOOST_AUTO_TEST_CASE(SharedLibraryBasic)
{
	string s("testlibrary");
	s = SharedLibrary::getSystemPrefix() + s + SharedLibrary::getSystemExtension();
	bfs::path p(s);

	//Try to find the library
	bool found = false;
	bfs::path path;
	path = TESTLIB_DIR;
	path /= s;
	if(bfs::exists(path))
		found = true;
	else
	{
		path = ".";
		path /= s;
		if(bfs::exists(path))
			found = true;
	}

	BOOST_CHECK_MESSAGE(found, "Could not find testlibrary, SharedLibrary tests incomplete.");

	if(found)
	{
		cout << "Attempting to open " << path << endl;
		SharedLibrary lib;
		BOOST_REQUIRE_NO_THROW(lib.open(path));

		if(lib.isLoaded())
		{
			TESTFUNCTION f;
			BOOST_REQUIRE_NO_THROW(f = (TESTFUNCTION)lib.getSymbol("GetName"));
			BOOST_CHECK(f() == "TestLibrary");
		}
	}
}

BOOST_AUTO_TEST_SUITE_END()
