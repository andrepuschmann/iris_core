/**
 * @file ParameterTypeInfo.h
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
 * Templates providing ids and properties for parameter types supported
 * by Iris.
 */

#ifndef PARAMETERTYPEINFO_H_
#define PARAMETERTYPEINFO_H_

#include <string>
#include <limits.h>
#include <boost/cstdint.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/push_back.hpp>

#include <irisapi/TypeInfo.h>


namespace iris
{

/** Traits class for checking whether a type can be used as a XML parameter
 *  exposed to the engine. Should be used as ParameterTypeInfo<T>::isAllowed , which
 *  evaluates to true or false. New types can be added with the MAKE_IS_IRIS_PARAMETER
 *  macro.
 */
template <typename T>
struct ParameterTypeInfo
{
    //! whether the type is allowed as a Component parameter
    IRIS_STATIC_CONSTANT(bool, isAllowed = false);
    //! The identifier number for this parameter type (can be used for comparisons)
    IRIS_STATIC_CONSTANT(int, identifier = -1);
    //! The name of the type
    static std::string name() { return "unknown"; }
};

/** Macro to add a new parameter type to the list of allowed ones. Calls
 *  to this macro should be made inside this header file only. Suitable
 *  types must be convertible to a string in some way, and back to the
 *  original type in order to allow specifying them in the XML file.
 *
 *  It will also push all types in a meta-container in order to be used by
 *  other algorithms (to have only one point to change for new parameters).
 */
#define MAKE_PARAMETER_TYPE(partype, identifier_) \
template <> \
struct ParameterTypeInfo<partype> \
{ \
    IRIS_STATIC_CONSTANT(bool, isAllowed = true); \
    IRIS_STATIC_CONSTANT(int, identifier = identifier_); \
    static const std::string name() { return BOOST_PP_STRINGIZE(partype); } \
}; \
namespace detail { \
typedef boost::mpl::push_back<parameter_types_tmp ## identifier_,partype>::type \
    BOOST_PP_CAT(parameter_types_tmp , BOOST_PP_INC(identifier_) ); \
}

namespace detail {
//! initial metavector for parameter types
typedef boost::mpl::vector<> parameter_types_tmp0;
}

// add new parameter types here, give them an unique id
// NOTE: for some reason, char, unsigned char, and signed char are three different
// types in VC++, while char is the same as either signed or unsigned char with g++
// Also, on 32bit systems, long and int are both 32 bit, but will be treated as different
// types be the compiler, so we need both types in the definition. (Same for int64_t and long
// on 64bit systems - both are 64 bit in that case.)
MAKE_PARAMETER_TYPE(bool, 0);
MAKE_PARAMETER_TYPE(char, 1);
MAKE_PARAMETER_TYPE(unsigned char, 2);
MAKE_PARAMETER_TYPE(unsigned short, 3);
MAKE_PARAMETER_TYPE(unsigned int, 4);
MAKE_PARAMETER_TYPE(unsigned long, 5);
MAKE_PARAMETER_TYPE(signed char, 6);
MAKE_PARAMETER_TYPE(signed short, 7);
MAKE_PARAMETER_TYPE(signed int, 8);
MAKE_PARAMETER_TYPE(signed long, 9);
MAKE_PARAMETER_TYPE(float, 10);
MAKE_PARAMETER_TYPE(double, 11);
MAKE_PARAMETER_TYPE(long double, 12);
MAKE_PARAMETER_TYPE(std::string, 13);
// if we are on a 64bit machine, long is 64bit, so no need for this
// (in fact, we will get an error)
#  if ULONG_MAX != 18446744073709551615U // 2**64 - 1

MAKE_PARAMETER_TYPE(uint64_t, 14);
MAKE_PARAMETER_TYPE(int64_t, 15);

// Note: the number after parameter_types_tmp has to be the number of times the
// MAKE_PARAMETER_TYPE macro has been called.
//! boost::mpl::vector of all types supported for Component parameters
typedef detail::parameter_types_tmp16 IrisParameterTypes;

#else

//! boost::mpl::vector of all types supported for Component parameters
typedef detail::parameter_types_tmp14 IrisParameterTypes;

#  endif



#undef MAKE_PARAMETER_TYPE

}

#endif /* PARAMETERTYPEINFO_H_ */
