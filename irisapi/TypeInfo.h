/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file TypeInfo.h
 * Declaration of class TypeInfo
 *
 *  Created on: 21-Nov-2008
 *  Created by: jlotze
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
 */

#ifndef TYPEINFO_H_
#define TYPEINFO_H_

#include <complex>
#include <string>
#include <limits>
#include <boost/type_traits.hpp>
#include <boost/cstdint.hpp>
#include <boost/assign.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/size.hpp>

// we have problems with static const members => revert to the enum
#define IRIS_STATIC_CONSTANT(type_, assignment_) enum { assignment_ }

// definitions of type properties in boost namespace
// will be used by TypeInfo<T> class
namespace boost
{
//! make complex doubles also floats (is_float should be true)
//! \todo shall we really do that??
//! probably bad practise to change to boost defined behaviour
template<>
struct is_float< std::complex<float> > : public true_type {};
template<>
struct is_float< std::complex<double> > : public true_type {};
template<>
struct is_float< std::complex<long double> > : public true_type {};

//! make all floats signed
template<>
struct is_signed< float > : public true_type {};
template<>
struct is_signed< double > : public true_type {};
template<>
struct is_signed< long double > : public true_type {};

//! make the complex types signed
template<>
struct is_signed< std::complex<float> > : public true_type {};
template<>
struct is_signed< std::complex<double> > : public true_type {};
template<>
struct is_signed< std::complex<long double> > : public true_type {};

}

namespace iris
{

// bring all the int types into iris namespace
using boost::uint8_t;
using boost::int8_t;
using boost::uint16_t;
using boost::int16_t;
using boost::uint32_t;
using boost::int32_t;
using boost::uint64_t;
using boost::int64_t;


//! namespace for internal use, for functions which are not part of the
//! interface
namespace detail
{
/** Base class used internally, simply for avoiding retyping too much
 *  common behaviour.
 */
template <typename T>
struct _TypeInfo_Base
{
    //! whether we have a numeric type
    IRIS_STATIC_CONSTANT(bool, isNumeric = boost::is_arithmetic<T>::value);
    //! is the type a floating point type?
    IRIS_STATIC_CONSTANT(bool, isFloat = boost::is_float<T>::value);
    //! plain old integer types
    IRIS_STATIC_CONSTANT(bool, isInteger = std::numeric_limits<T>::is_integer);
    //! signed types (include floats and complex)
    IRIS_STATIC_CONSTANT(bool, isSigned = boost::is_signed<T>::value);
    //! whether type is a pointer
    IRIS_STATIC_CONSTANT(bool, isPointer = boost::is_pointer<T>::value);
    //! whether type is a class
    IRIS_STATIC_CONSTANT(bool, isClass = boost::is_class<T>::value);
    //! whether type is a reference
    IRIS_STATIC_CONSTANT(bool, isReference = boost::is_reference<T>::value);
    //! whether type is std::complex<sometype>
    IRIS_STATIC_CONSTANT(bool, isComplex = boost::is_complex<T>::value);
    //! whether type is a const type
    IRIS_STATIC_CONSTANT(bool, isConst = boost::is_const<T>::value);
    //! fundamental types are all plain old data types built into C++
    IRIS_STATIC_CONSTANT(bool, isFundamental = boost::is_fundamental<T>::value);
    //! whether type is polymorphic (base class pointer or ref to virtual)
    IRIS_STATIC_CONSTANT(bool, isPolymorphic = boost::is_polymorphic<T>::value);
    //! whether type is unsigned
    IRIS_STATIC_CONSTANT(bool, isUnsigned = !boost::is_signed<T>::value);
    //! whether type is void
    IRIS_STATIC_CONSTANT(bool, isVoid = boost::is_void<T>::value);

};

} // end namespace detail

/** The general template to get a type id.
 *  All supported types for IRIS should have a template
 *  specialisation with a different id field.
 */
template <typename T>
struct TypeInfo : public detail::_TypeInfo_Base<T>
{
    //! whether the type is supported by IRIS for data flow
    IRIS_STATIC_CONSTANT(bool, isIrisSupported = false);
    //! identifier number for supported IRIS data flow types for comparisons
    IRIS_STATIC_CONSTANT(int, identifier = -1);
    //! name of the data type (only for IRIS supported data flow types)
    static std::string name() { return "unknown"; }
};


/** Macro to define new data types to be supported by IRIS components.
 * It sets TypeInfo<dattype>::isIrisSupported to true, gives it the
 * id given as a macro parameter, and sets the name() function to return
 * the name of the type.
 */
#define MAKE_DATA_TYPE(dattype, identifier_) \
    template <> \
    struct TypeInfo<dattype> : public detail::_TypeInfo_Base<dattype> { \
        IRIS_STATIC_CONSTANT(bool, isIrisSupported = true ); \
        IRIS_STATIC_CONSTANT(int, identifier = identifier_ ); \
        static std::string name() { return BOOST_PP_STRINGIZE(dattype); } \
    }; \
    namespace detail { \
    typedef boost::mpl::push_back<data_types_tmp ## identifier_, dattype>::type BOOST_PP_CAT(data_types_tmp, BOOST_PP_INC(identifier_)); \
    }

namespace detail {
//! initial empty vector of data types for macro
typedef boost::mpl::vector<> data_types_tmp0;
}

// register new data types here
MAKE_DATA_TYPE(uint8_t, 0);
MAKE_DATA_TYPE(uint16_t, 1);
MAKE_DATA_TYPE(uint32_t, 2);
MAKE_DATA_TYPE(uint64_t, 3);
MAKE_DATA_TYPE(int8_t, 4);
MAKE_DATA_TYPE(int16_t, 5);
MAKE_DATA_TYPE(int32_t, 6);
MAKE_DATA_TYPE(int64_t, 7);
MAKE_DATA_TYPE(float, 8);
MAKE_DATA_TYPE(double, 9);
MAKE_DATA_TYPE(long double, 10);
MAKE_DATA_TYPE(std::complex<float>, 11);
MAKE_DATA_TYPE(std::complex<double>, 12);
MAKE_DATA_TYPE(std::complex<long double>, 13);

// Note: the number after data_types_tmp is the number types supported (number of calls
// to MAKE_DATA_TYPE macro)
//! boost::mpl::vector containing all data types supported for IRIS data flow.
typedef detail::data_types_tmp14 IrisDataTypes;

// undefine to avoid conflicts
#undef MAKE_DATA_TYPE

//! Return the number of iris data types supported
static inline int getNumIrisDataTypes()
{
    return boost::mpl::size<IrisDataTypes>::value;
}

} // end of iris namespace



#endif /* TYPEINFO_H_ */
