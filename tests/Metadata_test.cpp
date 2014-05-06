/**
 * \file Metadata_test.cpp
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
 * Main test file for the Metadata class.
 */

#define BOOST_TEST_MODULE MetadataTest

#include <boost/test/unit_test.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#include <algorithm>
#include <string>
#include <iostream>

#include "irisapi/StackDataBuffer.h"
#include "iris/DataBuffer.h"

using namespace std;
using namespace iris;

BOOST_AUTO_TEST_SUITE (MetadataTest)

BOOST_AUTO_TEST_CASE(MetadataBasicTest)
{
    StackDataSet* stackDataSource = NULL;
    BOOST_CHECK_NO_THROW(stackDataSource = new StackDataSet);
    BOOST_REQUIRE(stackDataSource != NULL);

    BOOST_CHECK_EQUAL(stackDataSource->metadata.hasMetadata(), true);
    BOOST_CHECK_EQUAL(stackDataSource->metadata.hasMetadata("timeStamp"), true);
    BOOST_CHECK_EQUAL(stackDataSource->metadata.hasMetadata("sampleRate"), true);
    BOOST_CHECK_EQUAL(stackDataSource->metadata.hasMetadata("deadbeef"), false);

    int i = 11;
    stackDataSource->metadata.setMetadata("integer", i);

    BOOST_CHECK_EQUAL(stackDataSource->metadata.hasMetadata(), true);
    BOOST_CHECK_EQUAL(stackDataSource->metadata.hasMetadata("integer"), true);
    BOOST_CHECK_EQUAL(stackDataSource->metadata.hasMetadata("float"), false);

    int out;
    stackDataSource->metadata.getMetadata("integer", out);
    BOOST_CHECK_EQUAL(out, i);

    float f = 3.14;
    stackDataSource->metadata.setMetadata("float", f);
    BOOST_CHECK_EQUAL(stackDataSource->metadata.hasMetadata("float"), true);

    float out2;
    bool ret = stackDataSource->metadata.getMetadata("notthere", out2);
    BOOST_CHECK_EQUAL(ret, false);

    ret = stackDataSource->metadata.getMetadata("float", out2);
    BOOST_CHECK_EQUAL(ret, true);
    BOOST_CHECK_EQUAL(out2, f);

    // Check if vectors are working
    std::vector<int> int_vec_in;
    int_vec_in.push_back(10);
    int_vec_in.push_back(20);
    int_vec_in.push_back(30);
    stackDataSource->metadata.setMetadata("intvector", int_vec_in);
    BOOST_CHECK_EQUAL(stackDataSource->metadata.hasMetadata("intvector"), true);

    std::vector<int> int_vec_out;
    ret = stackDataSource->metadata.getMetadata("intvector", int_vec_out);
    BOOST_CHECK_EQUAL(int_vec_out.size(), 3);

    // Check if the data is equal
    for(int i = 0; i < 3; i++) {
        BOOST_CHECK_EQUAL(int_vec_in.at(i), int_vec_out.at(i));
    }

    delete stackDataSource;
}

BOOST_AUTO_TEST_CASE(MetadataStackDataSetCopyTest)
{
    // First create two StackDataSet objects
    StackDataSet* stackDataSource = NULL;
    BOOST_CHECK_NO_THROW(stackDataSource = new StackDataSet);
    BOOST_REQUIRE(stackDataSource != NULL);

    StackDataSet* stackDataDest = NULL;
    BOOST_CHECK_NO_THROW(stackDataDest = new StackDataSet);
    BOOST_REQUIRE(stackDataDest != NULL);

    // Both should have the default metadata timeStamp and sampleRate
    BOOST_CHECK_EQUAL(stackDataSource->metadata.hasMetadata("timeStamp"), true);
    BOOST_CHECK_EQUAL(stackDataSource->metadata.hasMetadata("sampleRate"), true);
    BOOST_CHECK_EQUAL(stackDataDest->metadata.hasMetadata("timeStamp"), true);
    BOOST_CHECK_EQUAL(stackDataDest->metadata.hasMetadata("sampleRate"), true);

    // Create artificial metadata and attach it to source
    float f = 3.14;
    stackDataSource->metadata.setMetadata("float", f);
    std::vector<int> int_vec;
    int_vec.push_back(10);
    int_vec.push_back(20);
    int_vec.push_back(30);
    stackDataSource->metadata.setMetadata("intvector", int_vec);

    // Now simply copy the metadata to the destionation
    stackDataDest->metadata = stackDataSource->metadata;

    // Check if this looks alright (two are there by default, plus the two we added)
    BOOST_CHECK_EQUAL(stackDataDest->metadata.getMetadataCount(), 4);

    float out;
    stackDataDest->metadata.getMetadata("float", out);
    BOOST_CHECK_EQUAL(out, f);

    std::vector<int> int_vec_out;
    bool ret = stackDataSource->metadata.getMetadata("intvector", int_vec_out);

    delete stackDataDest;
    delete stackDataSource;
}


BOOST_AUTO_TEST_CASE(MetadataDataSetCopyTest)
{
    StackDataSet* stackDataSource = NULL;
    BOOST_CHECK_NO_THROW(stackDataSource = new StackDataSet);
    BOOST_REQUIRE(stackDataSource != NULL);

    uint64_t tx_time = 12000223112;
    stackDataSource->metadata.setMetadata("tx_time", tx_time);

    DataSet< uint8_t >* dataset = new DataSet< uint8_t >;
    dataset->metadata = stackDataSource->metadata;

    uint64_t tx_time_out;
    dataset->metadata.getMetadata("tx_time", tx_time_out);
    BOOST_CHECK_EQUAL(tx_time, tx_time_out);

    delete dataset;
    delete stackDataSource;
}

BOOST_AUTO_TEST_SUITE_END()

