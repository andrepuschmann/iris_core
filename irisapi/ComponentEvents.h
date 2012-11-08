/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file ComponentEvents.h
 * Contains classes for the event interface of a component
 *
 *  Created on: 19-May-2009
 *  Created by: suttonp
 *  $Revision: 1195 $
 *  $LastChangedDate: 2010-12-21 13:56:46 +0000 (Tue, 21 Dec 2010) $
 *  $LastChangedBy: suttonp $
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
