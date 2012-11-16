/**
 * @file ComponentEvents.h
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
 * Contains classes for the event interface of an Iris Component.
 */

#ifndef COMPONENTEVENTS_H_
#define COMPONENTEVENTS_H_

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
    //! description of the event
    std::string description;

    //! name of the event
    std::string name;

    //! type of data to be passed with event
    int typeId;

    //! default constructor
    EventDescription(std::string n="", std::string d="", int t=0) :
        description(d), name(n), typeId(t)
    {
    }
};

/*!
 * \brief An interface to the events of a component
 *
 * The ComponentEvents class permits components to register events of different types. 
 * These events can then be triggered by the component together with relevent data.
 *
 */
class ComponentEvents : boost::noncopyable
{
private:
    std::map<std::string, EventDescription> d_events;
    ComponentCallbackInterface* d_engine;

protected:
    ComponentEvents& assignEvents(const ComponentEvents& other)
    {
        d_events = other.d_events;
        d_engine = other.d_engine;

        return *this;
    }

    void registerEvent(std::string name, std::string description, int typeId) throw (InvalidDataTypeException)
    {
        if(typeId < 0)
            throw InvalidDataTypeException("Invalid data type specified when registering event " + name);

        boost::to_lower(name);
        EventDescription e(name, description, typeId);
        d_events[name] = e;
    }

    template<typename T>
    inline void activateEventInternal(std::string compName, std::string name, T &data) 
        throw (EventNotFoundException, InvalidDataTypeException);

    template<typename T>
    inline void activateEventInternal(std::string compName, std::string name, std::vector<T> &data) 
        throw (EventNotFoundException, InvalidDataTypeException);

public:
    ComponentEvents():d_engine(NULL){}
    void setEngine(ComponentCallbackInterface* e){d_engine = e;}
    size_t getNumEvents(){return d_events.size();}
    std::map<std::string, EventDescription> getEvents(){return d_events;}
};

template<typename T>
inline void ComponentEvents::activateEventInternal(std::string compName, std::string name, T &data)
    throw (EventNotFoundException, InvalidDataTypeException)
{
    //Check that we have an interface to the engine
    if(d_engine == NULL)
    {
        return;
    }

    //Check that the event exists and that the datatypes match
    std::map<std::string, EventDescription>::const_iterator it = d_events.find(name);
    if (it == d_events.end())
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
        d_engine->activateEvent(e);
        return;
    }else{
        throw InvalidDataTypeException("Event data type did not match registered type for event " + name); 
    }
}

template<typename T>
inline void ComponentEvents::activateEventInternal(std::string compName, std::string name, std::vector<T> &data) 
    throw (EventNotFoundException, InvalidDataTypeException)
{
    //Check that we have an interface to the engine
    if(d_engine == NULL)
    {
        return;
    }

    //Check that the event exists and that the datatypes match
    std::map<std::string, EventDescription>::const_iterator it = d_events.find(name);
    if (it == d_events.end())
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
        d_engine->activateEvent(e);
        return;
    }else{
        throw InvalidDataTypeException("Event data type did not match registered type for event " + name); 
    }
}

} /* namespace iris */

#endif /* COMPONENTEVENTS_H_ */
