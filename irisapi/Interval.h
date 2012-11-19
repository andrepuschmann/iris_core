/**
 * \file Interval.h
 * \version 1.0
 *
 * \section COPYRIGHT
 *
 * Copyright 2012 The Iris Project Developers. See the
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
 *  Represents an interval: [Interval<T>().min, Interval<T>().max]
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
