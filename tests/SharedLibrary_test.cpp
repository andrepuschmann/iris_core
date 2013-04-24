/**
 * \file SharedLibrary_test.cpp
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
 * Main test file for SharedLibrary class.
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
