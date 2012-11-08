/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file DataTypesTest.cpp
 * Tests for the DataTypes subsystem
 *
 *  Created on: 21-Nov-2008
 *  Created by: jlotze
 *  $Revision: 1346 $
 *  $LastChangedDate: 2011-09-19 18:24:26 +0100 (Mon, 19 Sep 2011) $
 *  $LastChangedBy: suttonp $
 */

#define BOOST_TEST_MODULE DataTypes

#include <boost/test/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/static_assert.hpp>
#include <iostream>

#include "irisapi/TypeInfo.h"
#include "irisapi/TypeVectors.h"
#include "irisapi/ParameterTypeInfo.h"

using namespace boost;
using namespace boost::unit_test;
using namespace std;
using namespace iris;


BOOST_AUTO_TEST_CASE (type_information)
{

   BOOST_STATIC_ASSERT( TypeInfo<string>::identifier == -1 ); // not an iris type
   BOOST_STATIC_ASSERT( TypeInfo<uint8_t>::isUnsigned == true );
   BOOST_STATIC_ASSERT( TypeInfo<uint16_t>::isUnsigned == true );
   BOOST_STATIC_ASSERT( TypeInfo<uint32_t>::isUnsigned == true );
   BOOST_STATIC_ASSERT( TypeInfo<uint64_t>::isUnsigned == true );
   BOOST_STATIC_ASSERT( TypeInfo<uint8_t>::isSigned == false );
   BOOST_STATIC_ASSERT( TypeInfo<uint8_t>::isFloat == false );
   BOOST_STATIC_ASSERT( TypeInfo<int8_t>::isSigned == true );
   BOOST_STATIC_ASSERT( TypeInfo<int16_t>::isSigned == true );
   BOOST_STATIC_ASSERT( TypeInfo<int32_t>::isSigned == true );
   BOOST_STATIC_ASSERT( TypeInfo<int64_t>::isSigned == true );
   BOOST_STATIC_ASSERT( TypeInfo<int8_t>::identifier >= 0 );
   BOOST_STATIC_ASSERT( TypeInfo<uint16_t>::isComplex == false );
   BOOST_STATIC_ASSERT( TypeInfo<uint32_t>::isInteger == true );
   BOOST_STATIC_ASSERT( TypeInfo<uint64_t>::isNumeric == true );

   BOOST_STATIC_ASSERT( TypeInfo<uint8_t>::isIrisSupported == true );
   BOOST_STATIC_ASSERT( TypeInfo< complex<double> >::identifier >= 0 );
   BOOST_STATIC_ASSERT( TypeInfo<void>::isIrisSupported == false );
   BOOST_STATIC_ASSERT( TypeInfo< float>::isIrisSupported == true );
   BOOST_STATIC_ASSERT( TypeInfo<double>::isFloat == true );
   BOOST_STATIC_ASSERT( TypeInfo<void>::isFloat == false );
   BOOST_STATIC_ASSERT( TypeInfo< complex<double> >::isNumeric == true );
   BOOST_STATIC_ASSERT( TypeInfo< complex<float> >::isComplex == true );
   BOOST_STATIC_ASSERT( TypeInfo< complex<float> >::isFloat == true );
//   cout << "is complex a float? " << TypeInfo<complex<double> >::is_float << endl;
}

BOOST_AUTO_TEST_CASE(parameter_type_info)
{

    BOOST_CHECK_EQUAL( ParameterTypeInfo<unsigned char>::name(), "unsigned char");

    BOOST_STATIC_ASSERT( ParameterTypeInfo<char>::isAllowed == true );
    BOOST_STATIC_ASSERT( ParameterTypeInfo<int>::isAllowed == true);
    BOOST_STATIC_ASSERT( ParameterTypeInfo<string>::isAllowed == true );
    BOOST_STATIC_ASSERT( ParameterTypeInfo<double>::isAllowed == true );
    BOOST_STATIC_ASSERT( ParameterTypeInfo<uint8_t>::identifier != -1 );

    BOOST_STATIC_ASSERT( ParameterTypeInfo<signed char>::isAllowed == true );
    BOOST_STATIC_ASSERT( ParameterTypeInfo<unsigned char>::isAllowed == true);
    BOOST_STATIC_ASSERT( ParameterTypeInfo<signed short>::isAllowed == true );
    BOOST_STATIC_ASSERT( ParameterTypeInfo<unsigned short>::isAllowed == true);
    BOOST_STATIC_ASSERT( ParameterTypeInfo<signed int>::isAllowed == true );
    BOOST_STATIC_ASSERT( ParameterTypeInfo<unsigned int>::isAllowed == true);
    BOOST_STATIC_ASSERT( ParameterTypeInfo<signed long>::isAllowed == true );
    BOOST_STATIC_ASSERT( ParameterTypeInfo<unsigned long>::isAllowed == true);
    BOOST_STATIC_ASSERT( ParameterTypeInfo<int64_t>::isAllowed == true );
    BOOST_STATIC_ASSERT( ParameterTypeInfo<uint64_t>::isAllowed == true);

}

