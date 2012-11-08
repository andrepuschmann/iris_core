/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file ComponentBase.h
 * Contains base class for all components.
 *
 *  Created on: 20-Nov-2008
 *  Created by: jlotze
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
 */

#ifndef COMPONENTBASE_H_
#define COMPONENTBASE_H_

#include <irisapi/ComponentParameters.h>
#include <irisapi/ComponentEvents.h>
#include <irisapi/ComponentInformation.h>
#include <irisapi/ComponentPorts.h>
#include <irisapi/Logging.h>
#include <irisapi/LibraryDefs.h>
#include <irisapi/Version.h>

#include <boost/algorithm/string.hpp>

/** Macro for IRIS boilerplate code in each component.
 *  @param ComponentType Type of the component (PNComponent, SdfComponent, ...)
 *  @param ComponentClass Class of the component class to be exported by the library
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

/*!
 * \brief The base class for all components
 *
 * The ComponentBase class provides functionality for component parameters, information and ports via
 * its parent classes.
 */

class ComponentBase : public ComponentParameters, public ComponentEvents, 
	public ComponentInformation, public ComponentPorts
{
private:

protected:

public:

    virtual ~ComponentBase() {};

    /*!
     * \brief Construct this component
     *
     * \param name          component name
     * \param type          component type
     * \param description   brief description of this component
     * \param author        component author
     * \param version       component version
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
     * @param other The instance to copy the data from
     * @return A reference to *this.
     */
    ComponentBase& assign(const ComponentBase& other)
        throw (ParameterNotFoundException, InvalidDataTypeException)
    {
        assignParameters(other);
        assignEvents(other);

        return *this;
    }

	//! Activate an event
	template<typename T>
    inline void activateEvent(std::string name, T &data) 
		throw (EventNotFoundException, InvalidDataTypeException);

	//! Activate an event
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

} /* namespace iris */


#endif /* COMPONENTBASE_H_ */
