/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file Interval.h
 * Declaration of class Interval
 *
 *  Created on: 21-Nov-2008
 *  Created by: jlotze
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
 */

#ifndef INTERVAL_H_
#define INTERVAL_H_

#include <boost/assert.hpp>
#include <boost/numeric/conversion/bounds.hpp>
#include <limits>
#include <iostream>
#include <irisapi/TypeInfo.h>

namespace iris
{

/** Represents an interval: [Interval<T>().min, Interval<T>().max]
 */
template <typename T>
struct Interval
{
    //! Make sure we use a numeric type here
    BOOST_STATIC_ASSERT( TypeInfo<T>::isNumeric );
    //! The lower bound of the interval
    T minimum;
    //! The upper bound of the interval
    T maximum;

    //! Default constructor, initialises the values with min and max from
    //! std::numeric_limits.
    //! In case of floats, the min is set to -max.
    Interval()
	    : minimum(boost::numeric::bounds<T>::lowest()),
	      maximum(boost::numeric::bounds<T>::highest())
    {
    };

    //! Constructor giving the min and max explicitly
    Interval(T min, T max) : minimum(min), maximum(max) {  }

    //! Copy constructor
    Interval(const Interval& other) : minimum(other.minimum), maximum(other.maximum) {}

    //! Assignment
    Interval operator=(const Interval& other)
    {
        minimum = other.minimum;
        maximum = other.maximum;
        return *this;
    }

    //! compare to another Interval
    bool operator==(const Interval& other) const
    {
        return minimum==other.minimum && maximum == other.maximum;
    }

    //! check if number is in interval
    bool isIn(const T& num) const
    {
        return (num >= minimum) && (num <= maximum);
    }

};

//! specialisation for bool to get rid of warning
template<>
inline Interval<bool>::Interval() : minimum(false), maximum(true) {}

//! output Interval on ostream
template <typename T>
std::ostream& operator<<(std::ostream& os, const Interval<T> i)
{
    os << "[" << i.minimum << ", " << i.maximum << "]";
    return os;
}

} // end of iris namespace

#endif /* INTERVAL_H_ */
