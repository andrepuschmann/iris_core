/**
 * \file DataTypes_test.cpp
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
 * Main test file for DataTypes.
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

# if ULONG_MAX != 18446744073709551615U // 2**64 - 1
    BOOST_STATIC_ASSERT( ParameterTypeInfo<int64_t>::isAllowed == true );
    BOOST_STATIC_ASSERT( ParameterTypeInfo<uint64_t>::isAllowed == true);
# endif

}

