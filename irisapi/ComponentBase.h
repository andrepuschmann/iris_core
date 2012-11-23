/**
 * \file ComponentBase.h
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
 * The base class for all Iris Components.
 */

#ifndef IRISAPI_COMPONENTBASE_H_
#define IRISAPI_COMPONENTBASE_H_

#include <irisapi/ComponentParameters.h>
#include <irisapi/ComponentEvents.h>
#include <irisapi/ComponentInformation.h>
#include <irisapi/ComponentPorts.h>
#include <irisapi/Logging.h>
#include <irisapi/LibraryDefs.h>
#include <irisapi/Version.h>

#include <boost/algorithm/string.hpp>

/** Macro for IRIS boilerplate code in each component.
 *  \param ComponentType Type of the component (PNComponent, SdfComponent, ...)
 *  \param ComponentClass Class of the component class to be exported by the library
 *
 *  This macro defines the GetApiVersion(), CreateComponent(), and ReleaseComponent()
 *  functions with the correct C-style library export specifiers. It must be called
 *  in each component's source file.
 *  Example, for a pn component called ExampleComponent:
 *  \code
 *  IRIS_COMPONENT_EXPORTS(PNComponent, ExampleComponent)
 *
 *  ExampleComponent::~ExampleComponent(string name)
 *   ...
 *  \endcode
 */
#define IRIS_COMPONENT_EXPORTS(ComponentType, ComponentClass) \
  extern "C" EXPORT_DECLSPEC const char* GetApiVersion() \
  { \
    return Version::getApiVersion(); \
  } \
  extern "C" EXPORT_DECLSPEC ComponentType* CreateComponent(string name) \
  { \
    return new ComponentClass(name); \
  } \
  extern "C" EXPORT_DECLSPEC void ReleaseComponent(ComponentType* comp) \
  { \
    delete comp; \
  }

namespace iris
{

/** The base class for all components
 *
 * The ComponentBase class provides functionality for component parameters, events,
 * information and ports via its parent classes.
 */
class ComponentBase
  : public ComponentParameters,
    public ComponentEvents,
    public ComponentInformation,
    public ComponentPorts
{
public:

  virtual ~ComponentBase() {};

  /** Construct this component
   *
   * \param name      component name
   * \param type      component type
   * \param description   brief description of this component
   * \param author    component author
   * \param version     component version
   */
  ComponentBase(std::string name, std::string type, std::string description, std::string author, std::string version ):
    ComponentInformation(name, type, description, author, version)
  {};

  /** Set the logging policy for this component
  *
  *   \param policy   The logging policy to set
  */
  virtual void setLoggingPolicy(LoggingPolicy* policy) const
  {
    Logger::getPolicy() = policy;
  };

  /** Assign all parameters and events from another class
   *  to this one
   * \param other The instance to copy the data from
   * \return A reference to *this.
   */
  ComponentBase& assign(const ComponentBase& other)
    throw (ParameterNotFoundException, InvalidDataTypeException)
  {
    assignParameters(other);
    assignEvents(other);

    return *this;
  }

  /** Activate an event with a single data element.
   *
   * @param name  The name of the event to activate.
   * @param data  The data being passed with the event.
   */
  template<typename T>
  inline void activateEvent(std::string name, T &data) 
    throw (EventNotFoundException, InvalidDataTypeException);

  /** Activate an event with multiple data elements.
   *
   * @param name  The name of the event to activate.
   * @param data  The vector of data being passed with the event.
   */
  template<typename T>
  inline void activateEvent(std::string name, std::vector<T> &data) 
    throw (EventNotFoundException, InvalidDataTypeException);

};

// Get the name of this component and pass everything on to ComponentEvents
template<typename T>
inline void ComponentBase::activateEvent(std::string name, T &data) 
  throw (EventNotFoundException, InvalidDataTypeException)
{
  boost::to_lower(name);
  activateEventInternal(getName(), name, data);
}

// Get the name of this component and pass everything on to ComponentEvents
template<typename T>
inline void ComponentBase::activateEvent(std::string name, std::vector<T> &data)
  throw (EventNotFoundException, InvalidDataTypeException)
{
  boost::to_lower(name);
  activateEventInternal(getName(), name, data);
}

} // namespace iris


#endif // IRISAPI_COMPONENTBASE_H_
