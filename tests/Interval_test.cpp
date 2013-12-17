/**
 * \file Interval_test.cpp
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
 * Main test file for Interval class.
 */

#define BOOST_TEST_MODULE IntervalTest

#include <boost/test/unit_test.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#include <algorithm>
#include <string>

#include "irisapi/Interval.h"

using namespace std;
using namespace iris;

BOOST_AUTO_TEST_SUITE (IntervalTest)

BOOST_AUTO_TEST_CASE(IntervalBasic)
{
  //Check for construction of Interval
  Interval<int> i1;
  Interval<int> i2(0,9);
  Interval<int> i3(0,9,2);
  Interval<float> i4;
  Interval<float> i5(0.0,0.9);
  Interval<float> i6(0.0,0.9,0.2);
  Interval<double> i7;
  Interval<double> i8(0.0,0.9);
  Interval<double> i9(0.0,0.9,0.2);

  BOOST_REQUIRE(i1.isIn(0));
  BOOST_REQUIRE(i2.isIn(0));
  BOOST_REQUIRE(i2.isIn(9));
  BOOST_REQUIRE(i2.isIn(5));
  BOOST_REQUIRE(!i2.isIn(10));
  BOOST_REQUIRE(i3.isIn(0));
  BOOST_REQUIRE(i3.isIn(9));
  BOOST_REQUIRE(i3.isIn(2));
  BOOST_REQUIRE(!i3.isIn(1));
  BOOST_REQUIRE(i4.isIn(0.1));
  BOOST_REQUIRE(i5.isIn(0.0));
  BOOST_REQUIRE(i5.isIn(0.9));
  BOOST_REQUIRE(i5.isIn(0.5));
  BOOST_REQUIRE(!i5.isIn(1.0));
  BOOST_REQUIRE(i6.isIn(0.0));
  BOOST_REQUIRE(i6.isIn(0.9));
  BOOST_REQUIRE(i6.isIn(0.2));
  BOOST_REQUIRE(!i6.isIn(0.1));
  BOOST_REQUIRE(i7.isIn(0.1));
  BOOST_REQUIRE(i8.isIn(0.0));
  BOOST_REQUIRE(i8.isIn(0.9));
  BOOST_REQUIRE(i8.isIn(0.5));
  BOOST_REQUIRE(!i8.isIn(1.0));
  BOOST_REQUIRE(i9.isIn(0.0));
  BOOST_REQUIRE(i9.isIn(0.9));
  BOOST_REQUIRE(i9.isIn(0.2));
  BOOST_REQUIRE(!i9.isIn(0.1));
}

BOOST_AUTO_TEST_SUITE_END()
