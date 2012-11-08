/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file TypeVectors.h
 * Define a couple of useful vectors of types, e.g. intTypes, floatTypes, etc. for using
 * them in Components
 *
 *  Created on: 21-Nov-2008
 *  Created by: jlotze
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
 */

#ifndef TYPEVECTORS_H_
#define TYPEVECTORS_H_

#include <string>
#include <complex>
#include <boost/type_traits.hpp>
#include <boost/cstdint.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/mpl/accumulate.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/at.hpp>

#include <irisapi/ParameterTypeInfo.h>
#include <irisapi/TypeInfo.h>

// put in boost::mpl to avoid all the nasty namespace derefs everywhere
// use using directive below to bring into our namespace
namespace boost {
namespace mpl {

//! wrapper struct for iris TypeInfo member to conform with boost::mpl types,
//! which have to have members like value, type, value_type, etc.
template <typename T>
struct isIrisNumeric : public bool_< iris::TypeInfo<T>::isNumeric> { };

//! Vector of all numeric data types in in IrisDataTypes
typedef accumulate<
iris::IrisDataTypes,               // vector of all allowed data types
vector<>,                          // initialise to empty output vector
if_< isIrisNumeric<_2>, push_back<_1,_2>, _1 > // if cond for type, add to output vector, otherwise leave as is
>::type NumericDataTypes;        // define the new vector data type name

//! wrapper struct for iris TypeInfo member to conform with boost::mpl types,
//! which have to have members like value, type, value_type, etc.
template <typename T>
struct isIrisFloat : public bool_<iris::TypeInfo<T>::isFloat> { };

//! Vector of all floating point data types in in iris_data_types
typedef accumulate<
iris::IrisDataTypes,
vector<>,
if_<isIrisFloat<_2>, push_back<_1,_2>, _1 >
>::type FloatDataTypes;

//! wrapper struct for iris TypeInfo member to conform with boost::mpl types,
//! which have to have members like value, type, value_type, etc.
template <typename T>
struct isIrisInteger : public bool_<iris::TypeInfo<T>::isInteger> { };

//! Vector of all integer data types in iris_data_types
typedef accumulate<
iris::IrisDataTypes,
vector<>,
if_< isIrisInteger<_2>, push_back<_1,_2>, _1 >
>::type IntegerDataTypes;


//! wrapper struct for iris TypeInfo member to conform with boost::mpl types,
//! which have to have members like value, type, value_type, etc.
template <typename T>
struct isIrisSigned : public bool_<iris::TypeInfo<T>::isSigned> { };

//! Vector of all signed integer data types in iris_data_types
typedef accumulate<
iris::IrisDataTypes,
vector<>,
if_< and_<isIrisSigned<_2>, isIrisInteger<_2> >, push_back<_1,_2>, _1 >
>::type SignedIntegerDataTypes;

//! Vector of all signed data types in iris_data_types
typedef accumulate<
iris::IrisDataTypes,
vector<>,
if_< isIrisSigned<_2>, push_back<_1,_2>, _1 >
>::type SignedDataTypes;


//! wrapper struct for iris TypeInfo member to conform with boost::mpl types,
//! which have to have members like value, type, value_type, etc.
template <typename T>
struct isIrisComplex : public bool_<iris::TypeInfo<T>::isComplex> { };

//! Vector of all signed integer data types in iris_data_types
typedef accumulate<
iris::IrisDataTypes,
vector<>,
if_< isIrisComplex<_2>, push_back<_1,_2>, _1 >
>::type ComplexDataTypes;

}}  // end of namespace boost::mpl

namespace iris
{

using boost::mpl::NumericDataTypes;
using boost::mpl::FloatDataTypes;
using boost::mpl::IntegerDataTypes;
using boost::mpl::SignedIntegerDataTypes;
using boost::mpl::SignedDataTypes;
using boost::mpl::ComplexDataTypes;

namespace detail
{
    //! helper meta-functor which iterates through all types in the boost::mpl::vector
    //! Tvect and adds the type identifiers into a std::vector
    template <class Tvect, int N = boost::mpl::size<Tvect>::value >
    struct convertToTypeIdVectorHelper
    {
        static void EXEC(std::vector<int>& vect)
        {
            typedef typename boost::mpl::at_c<Tvect, N-1>::type currentType;
            BOOST_STATIC_ASSERT( TypeInfo<currentType>::identifier != -1 );
            vect.push_back( TypeInfo<currentType>::identifier );
            convertToTypeIdVectorHelper<Tvect, N-1>::EXEC(vect);
        }
    };

    //! specialisation to end recursion
    template <class Tvect>
    struct convertToTypeIdVectorHelper<Tvect, 0>
    {
        static void EXEC(std::vector<int>&) {}
    };
}

//! convert a type vector into a std::vector of type identifiers
template <class Tvect>
static inline std::vector<int> convertToTypeIdVector()
{
    std::vector<int> tmp;
    detail::convertToTypeIdVectorHelper<Tvect>::EXEC(tmp);
    return tmp;
}

} // end of namespace iris


#endif /* TYPEVECTORS_H_ */
