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
 * Main test file for DataBuffer element
 *
 *  Created on: 24-Nov-2008
 *  Created by: suttonp
 *  $Revision: 1346 $
 *  $LastChangedDate: 2011-09-19 18:24:26 +0100 (Mon, 19 Sep 2011) $
 *  $LastChangedBy: suttonp $
 */


#define BOOST_TEST_MODULE DataBufferTest

#include <boost/test/unit_test.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#include <algorithm>
#include <string>

#include "iris/DataBuffer.h"

using namespace std;
using namespace iris;

BOOST_AUTO_TEST_SUITE (DataBufferTest)

BOOST_AUTO_TEST_CASE(DataBufferBasic)
{
    //Check for construction of DataBuffer
    DataBuffer<float>* myBuffer = NULL;
    BOOST_CHECK_NO_THROW(myBuffer = new DataBuffer<float>(10)); 
    BOOST_REQUIRE(myBuffer != NULL);
    BOOST_CHECK_EQUAL(myBuffer->getTypeIdentifier(), int(TypeInfo<float>::identifier));
    BOOST_CHECK_EQUAL(myBuffer->hasData(), false);

    //Check getting and releasing DataSets
    DataSet<float>* theSet = NULL;
    myBuffer->getWriteData(theSet, 100);
    BOOST_REQUIRE(theSet != NULL);
    myBuffer->releaseWriteData(theSet);
    BOOST_REQUIRE(theSet == NULL);

    BOOST_CHECK_EQUAL(myBuffer->hasData(), true);

    myBuffer->getReadData(theSet);
    BOOST_REQUIRE(theSet != NULL);
    myBuffer->releaseReadData(theSet);
    BOOST_REQUIRE(theSet == NULL);

    BOOST_CHECK_EQUAL(myBuffer->hasData(), false);

    delete myBuffer;
}

BOOST_AUTO_TEST_CASE(DataBufferDataTypes)
{
    //Check for invalid data type in constructor
    BOOST_CHECK_THROW(DataBuffer<string> myBuffer(10), InvalidDataTypeException);

    //Check all valid data types
    BOOST_CHECK_NO_THROW(DataBuffer<uint8_t> myBuffer(10));
    BOOST_CHECK_NO_THROW(DataBuffer<uint16_t> myBuffer(10));
    BOOST_CHECK_NO_THROW(DataBuffer<uint32_t> myBuffer(10));
    BOOST_CHECK_NO_THROW(DataBuffer<uint64_t> myBuffer(10));
    BOOST_CHECK_NO_THROW(DataBuffer<int8_t> myBuffer(10));
    BOOST_CHECK_NO_THROW(DataBuffer<int16_t> myBuffer(10));
    BOOST_CHECK_NO_THROW(DataBuffer<int32_t> myBuffer(10));
    BOOST_CHECK_NO_THROW(DataBuffer<int64_t> myBuffer(10));
    BOOST_CHECK_NO_THROW(DataBuffer<float> myBuffer(10));
    BOOST_CHECK_NO_THROW(DataBuffer<double> myBuffer(10));
    BOOST_CHECK_NO_THROW(DataBuffer<long double> myBuffer(10));
    BOOST_CHECK_NO_THROW( DataBuffer<std::complex<float> > myBuffer(10));
    BOOST_CHECK_NO_THROW( DataBuffer<std::complex<double> > myBuffer(10));
    BOOST_CHECK_NO_THROW( DataBuffer<std::complex<long double> > myBuffer(10));
}

BOOST_AUTO_TEST_CASE(DataBufferErrorChecks)
{
    //Check for non-release of DataSets
    DataBuffer<float> myBuffer(10);
    DataSet<float>* theSet = NULL;
    myBuffer.getWriteData(theSet, 100);
    BOOST_CHECK_THROW(myBuffer.getWriteData(theSet, 100), DataBufferReleaseException);
    myBuffer.releaseWriteData(theSet);

    myBuffer.getReadData(theSet);
    BOOST_CHECK_THROW(myBuffer.getReadData(theSet), DataBufferReleaseException);
    myBuffer.releaseReadData(theSet);
    
}

//! Loop which reads data from a DataBuffer
template <typename T>
static void Reader(ReadBuffer<T>* buf, int numRuns, std::vector<T>* storage)
{
    DataSet<T>* setPtr;
	for(int i=0;i<numRuns;i++)
	{
        buf->getReadData(setPtr);
        (*storage)[i] = setPtr->data[0];
        buf->releaseReadData(setPtr);
	}
}


//! Loop which writes data to a DataBuffer
template <typename T>
static void Writer(WriteBuffer<T>* buf, int numRuns, std::vector<T>* storage)
{
    DataSet<T>* setPtr;
	for(int i=0;i<numRuns;i++)
	{
        buf->getWriteData(setPtr, 1);
        setPtr->data[0] = (*storage)[i];
        buf->releaseWriteData(setPtr);
	}
}

BOOST_AUTO_TEST_CASE(DataBufferDataCheck)
{
    //Check for data integrity
    DataBuffer<int32_t> myBuffer(10);

    int numRuns = 10000;
    std::vector<int32_t> storage1(numRuns);
    std::vector<int32_t> storage2(numRuns);
    for(int i=0;i<numRuns;i++)
    {
        storage1[i] = i;
    }

    //Start a writer and reader thread
    boost::thread thrd1(boost::bind(&Writer<int32_t>, &myBuffer, numRuns, &storage1));
    boost::thread thrd2(boost::bind(&Reader<int32_t>, &myBuffer, numRuns, &storage2));

    thrd1.join();
    thrd2.join();

    //Check the data
    for(int i=0;i<numRuns;i++)
    {
        BOOST_CHECK_EQUAL(storage1[i], storage2[i]);
    }
}

BOOST_AUTO_TEST_SUITE_END()
