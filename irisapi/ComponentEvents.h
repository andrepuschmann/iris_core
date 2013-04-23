/**
 * \file ComponentEvents.h
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
 * Contains classes for the event interface of an Iris Component.
 */

#ifndef IRISAPI_COMPONENTEVENTS_H_
#define IRISAPI_COMPONENTEVENTS_H_

#include "irisapi/Event.h"
#include "irisapi/Exceptions.h"
#include "irisapi/ComponentCallbackInterface.h"

#include <boost/algorithm/string.hpp>

namespace iris
{

/** Represents an event of a component child class. Will be created by
 *  the registerEvent function in the ComponentEvents class.
 */
struct EventDescription
{
  std::string description;
  std::string name;
  int typeId;       ///< Type of data passed with event.

  EventDescription(std::string n="", std::string d="", int t=0) :
    description(d), name(n), typeId(t)
  {}
};

/** An interface to the events of a component
 *
 * The ComponentEvents class permits components to register events of different types. 
 * These events can then be triggered by the component together with relevent data.
 */
class ComponentEvents : boost::noncopyable
{
public:
  ComponentEvents():engine_(NULL){}
  void setEngine(ComponentCallbackInterface* e){engine_ = e;}
  size_t getNumEvents(){return events_.size();}
  std::map<std::string, EventDescription> getEvents(){return events_;}

protected:
  ComponentEvents& assignEvents(const ComponentEvents& other)
  {
    events_ = other.events_;
    engine_ = other.engine_;

    return *this;
  }

  void registerEvent(std::string name, std::string description, int typeId)
  {
    if(typeId < 0)
      throw InvalidDataTypeException("Invalid data type specified when registering event " + name);

    boost::to_lower(name);
    EventDescription e(name, description, typeId);
    events_[name] = e;
  }

  template<typename T>
  inline void activateEventInternal(std::string compName, std::string name, T &data);

  template<typename T>
  inline void activateEventInternal(std::string compName, std::string name, std::vector<T> &data);

private:
  std::map<std::string, EventDescription> events_;
  ComponentCallbackInterface* engine_;
};

template<typename T>
inline void ComponentEvents::activateEventInternal(std::string compName, std::string name, T &data)
{
  //Check that we have an interface to the engine
  if(engine_ == NULL)
  {
    return;
  }

  //Check that the event exists and that the datatypes match
  std::map<std::string, EventDescription>::const_iterator it = events_.find(name);
  if (it == events_.end())
    throw EventNotFoundException("Event " + name + " not found");
  
  if(it->second.typeId == TypeInfo<T>::identifier)
  {
    //std::vector<boost::any> d;
    //d.push_back(boost::any(data));
    //d_engine->activateEvent(name, d);
    Event e;
    e.data.push_back(boost::any(data));
    e.eventName = name;
    e.componentName = compName;
    e.typeId = TypeInfo<T>::identifier;
    engine_->activateEvent(e);
    return;
  }else{
    throw InvalidDataTypeException("Event data type did not match registered type for event " + name); 
  }
}

template<typename T>
inline void ComponentEvents::activateEventInternal(std::string compName, std::string name, std::vector<T> &data)
{
  //Check that we have an interface to the engine
  if(engine_ == NULL)
  {
    return;
  }

  //Check that the event exists and that the datatypes match
  std::map<std::string, EventDescription>::const_iterator it = events_.find(name);
  if (it == events_.end())
    throw EventNotFoundException("Event " + name + " not found");

  if(it->second.typeId == TypeInfo<T>::identifier)
  {
    //std::vector<boost::any> d;
    //d.resize(data.size());
    //std::copy(data.begin(), data.end(), d.begin());
    //d_engine->activateEvent(name, d);
    Event e;
    e.data.resize(data.size());
    std::copy(data.begin(), data.end(), e.data.begin());
    e.eventName = name;
    e.componentName = compName;
    e.typeId = TypeInfo<T>::identifier;
    engine_->activateEvent(e);
    return;
  }else{
    throw InvalidDataTypeException("Event data type did not match registered type for event " + name); 
  }
}

} // namespace iris

#endif // IRISAPI_COMPONENTEVENTS_H_
