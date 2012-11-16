/**
 * @file Controller_test.cpp
 * @version 1.0
 *
 * @section COPYRIGHT
 *
 * Copyright 2012 The Iris Project Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution
 * and at http://www.softwareradiosystems.com/iris/copyright.html.
 *
 * @section LICENSE
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
 * @section DESCRIPTION
 *
 * Main test file for Controller class.
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
