/**
 * \file ComponentParameters.h
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
 * Contains classes for the parameter interface of a component (parameters
 * adjustable from XML or controller).
 */

#ifndef IRISAPI_COMPONENTPARAMETERS_H_
#define IRISAPI_COMPONENTPARAMETERS_H_

#include <string>
#include <map>
#include <list>
#include <boost/mpl/at.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/size.hpp>
#include <boost/any.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/noncopyable.hpp>

#include <irisapi/TypeInfo.h>
#include <irisapi/ParameterTypeInfo.h>
#include <irisapi/Interval.h>
#include <irisapi/Exceptions.h>


namespace iris
{

/** Represents a parameter of a component child class. Will be created by
 *  the registerParameter function in the ComponentParameters class.
 */
struct Parameter
{
  boost::any parameter;     ///< Pointer to actual parameter
  std::string description;
  std::string defaultValue;
  bool isDynamic;           ///< Can this parameter be dynamically changed?
  int identifier;           ///< Unique ID.
  int typeIdentifier;       ///< ParameterTypeInfo<Type>::id
  std::string typeName;

  /// Whether a list of possibilities was given (true), or an interval (false) for allowedValues
  bool isList;

  /// holds either the list<T> or the Interval<T> struct (min,max)
  boost::any allowedValues;

  /// default constructor - initialises all values to 0/false/empty, and identifier to -1
  Parameter() :
    parameter(), description(""), defaultValue(""), isDynamic(false), identifier(-1),
    typeIdentifier(0), typeName(""), isList(false)
  {
  }

  /** Constructs a new Parameter object.
   * identifier is initialised to -1 and the allowedValues field is empty.
   *
   * \param parameter A reference to the actual parameter in the child class
   * \param description Description of the parameter
   * \param defaultValue Default value, represented as a string
   * \param isDynamic Whether the parameter can be changed dynamically
   */
  template<typename T>
  Parameter(T& parameter, std::string description, std::string defaultValue = "",
      bool isDynamic = false) :
    parameter(&parameter), description(description), defaultValue(defaultValue),
    isDynamic(isDynamic), identifier(-1)
  {
  }

};


/** An interface to the parameters of a component
 *
 * The ComponentParameters class permits components to register parameters of different types along with
 * permitted values. These parameters may be accessed through setValue and getValue functions.
 */
class ComponentParameters
  : boost::noncopyable
{
public:

  /// Constructs an instance of ComponentParameters
  ComponentParameters()
    : parameterMap_()
  {}

  virtual ~ComponentParameters() {}

  /** Copies the parameters by value from other into this.
   *
   * \param other The ComponentParameters object to copy from.
   */
  virtual ComponentParameters& assignParameters(const ComponentParameters& other);


  /** Returns the default XML for all parameters.
   *
   * \return A std::string (with newlines) with all parameters, e.g.
   *  \code
   *   <parameter name="example" value="1.234" />
   *   <parameter name="debug" value="false" />
   *  \endcode
   */
  std::string getDefaultXML() const
  {
    using namespace std;

    map<string,Parameter>::const_iterator it;
    stringstream ret;

    for (it = parameterMap_.begin(); it != parameterMap_.end(); ++it)
    {
      ret << "\t<parameter name=\"" << it->first << "\" " <<
          "value=\"" << it->second.defaultValue << "\" />" << endl;
    }

    return ret.str();
  }

  /// returns the number of registered parameters
  size_t getNumParameters() const
  { return parameterMap_.size(); }

  /** Get the value of parameter 'name' in a std::string.
   *
   * \return The current value as a string.
   */
  std::string getValue(std::string name) const;

  /** Get the value of a registered parameter
   *
   * \param name    Parameter name
   * \param value   Pointer to a variable where the result should be stored.
   */
  template<typename T>
  inline void getValue(std::string name, T* value) const;

  /** Changes the value of parameter "name" to value "val".
   *
   * \param name    Name of parameter to change.
   * \param value   New value for the parameter.
   */
  template <typename T>
  inline void setValue(std::string name, T value);

  /** Get the description of the parameter.
   *
   * \param name Name of the parameter
   * \return Description of the parameter
   */
  std::string getParameterDescription(std::string name) const
  {
    //Convert parameter name to lower case
    boost::to_lower(name);
    return getParameterReference(name).description;
  }

  /** Returns the default value of a parameter as a string.
   *
   * \return Default value as string.
   * \throw ParameterNotFoundException if parameter with name name does not exist.
   */
  std::string getParameterDefaultValue(std::string name) const
  {
    //Convert parameter name to lower case
    boost::to_lower(name);
    return getParameterReference(name).defaultValue;
  }

  /** Returns the data type of a registered parameter.
   *
   * \param name  The parameter name.
   */
  std::string getParameterDataType(std::string name) const
  {
    //Convert parameter name to lower case
    boost::to_lower(name);
    return getParameterReference(name).typeName;
  }

  /** Returns true if the parameter with the given name is dynamically
   * reconfigurable.
   *
   * @param name    The parameter name.
   * @return  True if parameter is dynamically reconfigurable.
   */
  bool isParameterDynamic(std::string name) const
  {
    //Convert parameter name to lower case
    boost::to_lower(name);
    return getParameterReference(name).isDynamic;
  }

  /** Called to tell a component that one of its parameters has been reconfigured.
   *
   * \param name Name of the parameter
   */
  virtual void parameterHasChanged(std::string name){}

protected:
  /** Registers a parameter of a child class. All parameters registered
   * with this function can be accessed from the XML description and from the Controller.
   *
   * \param name Name of the parameter
   * \param description Description of the parameter
   * \param defaultValue  Default value as a string
   * \param isDynamic  Whether the parameter is dynamically changeable at run-time.
   * \param parameter  The parameter itself. A pointer to it will be stored.
   * \throw InvalidDataTypeException If allowedInterval.min > allowedInterval.max.
   */
  template<typename T>
  inline void registerParameter(std::string name, std::string description, std::string defaultValue,
      bool isDynamic, T& parameter);

  /** Registers a parameter of a child class. All parameters registered
   * with this function can be accessed from the XML description and from the Controller.
   *
   * \param name Name of the parameter
   * \param description Description of the parameter
   * \param defaultValue  Default value as a string
   * \param isDynamic  Whether the parameter is dynamically changeable at run-time.
   * \param parameter  The parameter itself. A pointer to it will be stored.
   * \param allowedInterval Interval for allowed values. Defaults to [min, max] of the data type.
   * \throw InvalidDataTypeException If allowedInterval.min > allowedInterval.max.
   */
  template<typename T>
  inline void registerParameter(std::string name, std::string description, std::string defaultValue,
      bool isDynamic, T& parameter, Interval<T> allowedInterval);

  /** Registers a parameter of a child class. All parameters registered
   * with this function can be accessed from the XML description and from the Controller.
   *
   * \param name Name of the parameter
   * \param description Description of the parameter
   * \param defaultValue  Default value as a string
   * \param isDynamic  Whether the parameter is dynamically changeable at run-time.
   * \param parameter  The parameter itself. A pointer to it will be stored.
   * \param allowedValues std::list of allowed values.
   * \throw InvalidDataTypeException If allowedValues is empty.
   */
  template<typename T>
  inline void registerParameter(std::string name, std::string description, std::string defaultValue,
      bool isDynamic, T& parameter, std::list<T> allowedValues);


private:

  /// Helper method called by the registerParameter methods
  template<typename T>
  void registerParameterHelper(std::string name, std::string description,
      std::string defaultValue, bool isDynamic, T& parameter);

  /** Return a const reference to a Parameter object.
   *
   * @param name    The parameter name.
   * @return  Const reference to the Parameter object.
   */
  const Parameter& getParameterReference(std::string name) const
  {
    std::map<std::string, Parameter>::const_iterator it = parameterMap_.find(name);
    if (it == parameterMap_.end())
      throw ParameterNotFoundException(std::string("Parameter ") + name + " does not exist.");
    return it->second;
  }

  /** Return a reference to a Parameter object.
   *
   * @param name    The parameter name.
   * @return  Reference to the parameter object.
   */
  Parameter& getParameterReference(std::string name)
  {
    std::map<std::string, Parameter>::iterator it = parameterMap_.find(name);
    if (it == parameterMap_.end())
      throw ParameterNotFoundException(std::string("Parameter ") + name + " does not exist.");
    return it->second;
  }

  /// Map holding all registered parameters. The key is the parameter name.
  std::map<std::string, Parameter> parameterMap_;

}; // class ComponentParameter


/*************************************************
* Template method Implementations
*************************************************/

template <typename T>
inline void ComponentParameters::getValue(std::string name, T* value) const
{
  //Convert parameter name to lower case
  boost::to_lower(name);

  const Parameter& par = getParameterReference(name);

  T** x = boost::any_cast<T*>(const_cast<boost::any*>(&(par.parameter)));
  if (x == NULL)
    throw InvalidDataTypeException(std::string("Parameter ") + name + ": is not of type " +
      ParameterTypeInfo<T>::name() + ", the stored type is " + par.typeName);
  else
    *value = *boost::any_cast<T*>(par.parameter);
}


template <typename T>
inline void ComponentParameters::setValue(std::string name, T value)
{
  BOOST_STATIC_ASSERT( ParameterTypeInfo<T>::isAllowed );

  //Convert parameter name to lower case
  boost::to_lower(name);

  Parameter& par = getParameterReference(name);
  /// \todo check if parameter is convertible to the parameter type with boost::is_convertible<from,to>
  ///     and convert if possible
  T** x = boost::any_cast<T*>(&par.parameter);
  if (x == NULL)
  {
    throw InvalidDataTypeException(std::string("Invalid data type used. The stored value type is ")
        + par.typeName + " you requested: " + ParameterTypeInfo<T>::name());
  }
  if (par.isList)
  {
    std::list<T>* tmp = boost::any_cast< std::list<T> >(&par.allowedValues);
    if (find(tmp->begin(), tmp->end(), value) == tmp->end())  // value not allowed
    {
      std::stringstream sstr;
      sstr << "Setting value of " << name << " to " << value << " is not in allowed.";
      throw ParameterOutOfRangeException(sstr.str());
    }
  }
  else
  {
    Interval<T>* tmp = boost::any_cast< Interval<T> >(&par.allowedValues);
    if ( (value < tmp->minimum) || (value > tmp->maximum) )
    {
      std::stringstream sstr;
      sstr << "Setting value of " << name << " to " << value << " is not in allowed. Allowed interval: " << *tmp;
      throw ParameterOutOfRangeException(sstr.str());
    }
  }

  **x = value;
}


namespace internal
{

/** Struct for metaprogramming loop to go through all iris types and try to set the value
 *  of the parameter to the given one (as a string)
 */
template <int N = boost::mpl::size<IrisParameterTypes>::value>
struct setParameterValue
{
  static bool EXEC(ComponentParameters* compPar, std::string name, const Parameter& par, const std::string& newval)
  {
    using namespace std;

    typedef typename boost::mpl::at_c<IrisParameterTypes,N-1>::type ParT;

    // In case of  ParT==std::string, go on with next value,
    // otherwise will call setValue(string,string) again, causing an endless recursion
    // note: if the parameter is indeed std::string, this function will not get called
    if (boost::is_same<std::string, ParT>::value)
      return setParameterValue<N-1>::EXEC(compPar, name, par, newval);

    ParT** x = boost::any_cast<ParT*>(const_cast<boost::any*>(&(par.parameter)));
    if (x == NULL) // wrong type
    {
      return setParameterValue<N-1>::EXEC(compPar, name, par, newval);
    }

    stringstream sstr(newval);
    ParT value;
    sstr >> value;
    compPar->setValue<ParT>(name, value);
    return true;

  }
};

template <>
struct setParameterValue<0>
{
  static bool EXEC(ComponentParameters*, std::string, const Parameter&, const std::string&)
  {
    return false;
  }
};

/// convert an arbitrary type into a string
template <typename T>
inline std::string TypeToString(const T& val)
{
  std::stringstream sstr;
  sstr << val;
  return sstr.str();
}

/// boolean template specialisation -> output true/false instead of 1/0
template <>
inline std::string TypeToString<bool>(const bool& val)
{
  return val ? "true" : "false";
}

/// specialisation for strings -> we can simply return the value
template <>
inline std::string TypeToString<std::string>(const std::string& val)
{
  return val;
}

/// template class to extract the value of a boost::any field into a string,
/// using a metaprogramming loop (will be unrolled by compiler)
template <int N>
struct GetStringParTmp {

  static std::string EXEC(const boost::any& val)
{
  typedef typename boost::mpl::at_c<IrisParameterTypes,N-1>::type ParT;
  ParT** ret = boost::any_cast<ParT*>(const_cast<boost::any*>(&val));
  if (ret != NULL)
  {
    return TypeToString(**ret);
  }
  else // not the right type, go on with the next one
  {
    return GetStringParTmp<N-1>::EXEC(val);
  }
}

};

/// specialisation for exit condition of the recursion
template<>
struct GetStringParTmp<0>
{
static std::string EXEC(const boost::any& val)
{
  throw InvalidDataTypeException("could not resolve type of parameter");
  return "";
}
};

/// Function to convert the value held by the given boost::any into a string.
/// Checks all iris data types by using the GetStringParTmp template class.
inline std::string getStringParameter(const boost::any& val)
{
  return GetStringParTmp< boost::mpl::size<IrisParameterTypes>::value>::EXEC(val);
}

} // end of internal namespace

inline std::string ComponentParameters::getValue(std::string name) const
{
  //Convert parameter name to lower case
  boost::to_lower(name);

  const Parameter& par = getParameterReference(name);

  return internal::getStringParameter(par.parameter);
}

// template specialisation if the value is given as a string -> perform string conversion to
// the real parameter type, using the meta-programming loop above (in internal namespace)
template <>
inline void ComponentParameters::setValue< std::string >(std::string name, std::string value)
{
  //Convert parameter name to lower case
  boost::to_lower(name);

  // throw if parameter does not exist
  Parameter& par = getParameterReference(name);

  if (par.identifier == -1)
  {
    throw ParameterNotFoundException(std::string("Invalid parameter name: ") + name);
  }

  if (par.typeIdentifier == ParameterTypeInfo<bool>::identifier)
  {
    boost::to_lower(value);
    if ( (value == "yes") || (value == "true") || (value == "on") || (value == "1") )
      *boost::any_cast<bool*>(par.parameter) = true;
    else if ( (value == "no") || (value == "false") || (value == "off") || (value == "0") )
      *boost::any_cast<bool*>(par.parameter) = false;
    else
      throw InvalidDataTypeException(std::string("Parameter ") + name + ": " + value + " could not be converted to bool");
  } else if (par.typeIdentifier == ParameterTypeInfo<std::string>::identifier)
  {
    *boost::any_cast<std::string*>(par.parameter) = value;
  } else
  {
    const Parameter& par = getParameterReference(name);
    if (!internal::setParameterValue<>::EXEC(this, name, par, value))
    {
      throw InvalidDataTypeException(std::string("Parameter ") + name + ": could not resolve type");
    }
  }
}

// template specialisation for const char* -> call the string version
template <>
inline void ComponentParameters::setValue< const char* >(std::string name, const char* value)
{
  //Convert parameter name to lower case
  boost::to_lower(name);

  setValue(name, std::string(value));
}


template <typename T>
inline void ComponentParameters::registerParameterHelper(std::string name, std::string description,
    std::string defaultValue, bool isDynamic, T& parameter)
{
  // make sure the parameter type is supported (at compile time)
  BOOST_STATIC_ASSERT(ParameterTypeInfo<T>::isAllowed);
  BOOST_STATIC_ASSERT(ParameterTypeInfo<T>::identifier >= 0);

  Parameter par(parameter, description, defaultValue, isDynamic);
  par.identifier = (int)parameterMap_.size();
  par.typeIdentifier = ParameterTypeInfo<T>::identifier;
  par.typeName = ParameterTypeInfo<T>::name();
  parameterMap_[name] = par;

}

template<typename T>
inline void ComponentParameters::registerParameter(std::string name, std::string description,
    std::string defaultValue, bool isDynamic, T& parameter)
{
  Interval<T> allowedInterval;
  if (allowedInterval.minimum > allowedInterval.maximum)
  {
    throw InvalidDataTypeException(std::string("Parameter ") + name
        + ": In given interval, min should be less or equal than max");
  }

  //Convert parameter name to lower case
  boost::to_lower(name);

  registerParameterHelper(name, description, defaultValue, isDynamic, parameter);
  parameterMap_[name].isList = false;
  parameterMap_[name].allowedValues = allowedInterval;

  // set parameter to its default value
  this->setValue(name, defaultValue);
}

/** Specialistaion for std::string - intervals don't make sense here.
 *
 * \todo It should be possible for string parameters to give a list of allowed values,
 *     but an interval does not make sense (and string parameters without an allowedValues
 *     list are of course possible)
 *
 * \param name Name of the parameter
 * \param description Description of the parameter
 * \param defaultValue  Default value as a string
 * \param isDynamic  Whether the parameter is dynamically changeable at run-time.
 * \param parameter  The parameter itself. A pointer to it will be stored.
 */
template <>
inline void ComponentParameters::registerParameter(std::string name, std::string description,
    std::string defaultValue, bool isDynamic, std::string& parameter)
{
  //Convert parameter name to lower case
  boost::to_lower(name);

  registerParameterHelper(name, description, defaultValue, isDynamic, parameter);

  // set parameter to its default value
  this->setValue(name, defaultValue);
}

template<typename T>
inline void ComponentParameters::registerParameter(std::string name, std::string description,
    std::string defaultValue, bool isDynamic, T& parameter, Interval<T> allowedInterval)
{
  if (allowedInterval.minimum > allowedInterval.maximum)
  {
    throw InvalidDataTypeException(std::string("Parameter ") + name
        + ": In given interval, min should be less or equal than max");
  }

  //Convert parameter name to lower case
  boost::to_lower(name);

  registerParameterHelper(name, description, defaultValue, isDynamic, parameter);
  parameterMap_[name].isList = false;
  parameterMap_[name].allowedValues = allowedInterval;

  // set parameter to its default value
  this->setValue(name, defaultValue);
}

template<typename T>
inline void ComponentParameters::registerParameter(std::string name, std::string description,
    std::string defaultValue, bool isDynamic, T& parameter, std::list<T> allowedValues)
{
  if (allowedValues.empty())
  {
    throw InvalidDataTypeException(std::string("Parameter ") + name +
        ": allowed_values should contain at least one element.");
  }

  //Convert parameter name to lower case
  boost::to_lower(name);

  registerParameterHelper(name, description, defaultValue, isDynamic, parameter);
  parameterMap_[name].isList = true;
  parameterMap_[name].allowedValues = allowedValues;

  // set to default
  this->setValue(name, defaultValue);
}


inline ComponentParameters& ComponentParameters::assignParameters(const ComponentParameters& other)
{
  for (std::map<std::string, Parameter>::iterator i = parameterMap_.begin();
     i != parameterMap_.end();
     ++i)
  {
    setValue(i->first, other.getValue(i->first));
  }
  return *this;
}

} // iris namespace

#endif // IRISAPI_COMPONENTPARAMETERS_H_
