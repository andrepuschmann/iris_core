/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file ControllerManager.h
 *
 *  Created on: 19-May-2009
 *  Created by: sutttonp
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
 *
 */

#ifndef CONTROLLERMANAGER_H_
#define CONTROLLERMANAGER_H_

#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>
#include "boost/filesystem.hpp"

#include "iris/SharedLibrary.h"
#include "iris/ControllerManagerCallbackInterface.h"
#include "irisapi/Event.h"
#include "irisapi/Controller.h"



namespace iris
{

//! Function pointer for "GetApiVersion" in controller library
typedef const char* (*GETAPIVERSIONFUNCTION)();
//! Function pointer for "CreateController" in controller library
typedef Controller* (*CREATECONTROLLERFUNCTION)();
//! Function pointer for "ReleaseController" in controller library
typedef void (*DESTROYCONTROLLERFUNCTION)(Controller*);

//! A loaded controller object
struct LoadedController
{
	std::string name;
	boost::shared_ptr< Controller > contPtr;

	LoadedController(std::string name, boost::shared_ptr< Controller > contPtr)
        :name(name), contPtr(contPtr){}
};

//! A controller library - contains the library (used to create a controller object)
struct ControllerLibrary
{
    boost::filesystem::path path;
    std::string name;
    boost::shared_ptr< SharedLibrary > libPtr;

    //! Constructor initializes our variables
    ControllerLibrary()
        :name(""){}
};

//! A repository containing a number of controller libraries
struct ControllerRepository
{
    boost::filesystem::path path;
    std::vector< ControllerLibrary > controllerLibs;
};

/*!
 *  \brief The ControllerManager manages all controllers running within the IRIS architecture.
 */
class ControllerManager: public ControllerCallbackInterface
{
private:
	std::vector< LoadedController > d_loadedControllers;
	std::vector<ControllerRepository> d_repositories;
	ControllerManagerCallbackInterface* d_engineManager;

	/** Map of events to subscribed controllers
	*
	*	The keys for the map are formed by concatonating the component name with the event name.
	*	Each key is associated with a vector of Controller* - these are the controllers that have
	*	subscribed to that event.
	*/
	std::map< std::string, std::vector<Controller*> > d_eventMap;

public:

    ControllerManager();

	//! Set a pointer to the EngineManager which owns this ControllerManager
	void setCallbackInterface(ControllerManagerCallbackInterface* e);

	/** Add a repository
    *   Multiple repositories can be specified by separating paths with ";"
    *   \param repoPath     Path to the controller repository
    */
    void addRepository(std::string repoPath) throw (ResourceNotFoundException);

    /** Load a Controller
    *   \param	name     Name of Controller to be loaded
    */
	void loadController(std::string name) throw (IrisException);

	//! Start all Controllers
	void startControllers() throw (IrisException);

	//! Stop all Controllers
	void stopControllers() throw (IrisException);

	//! Unload all Controllers
	void unloadControllers() throw (IrisException);

    /** Check whether a controller exists
    *   \param  name    The name of the component to look for
    */
    bool controllerExists(std::string name);

    //! Get all the repositories which are available
    std::vector<boost::filesystem::path> getRepositories();

	//! Activate an event
	void activateEvent(Event &e);

	//! Reconfigure the radio
	virtual void reconfigureRadio(ReconfigSet reconfigs);

	//! Post a command to a component
	virtual void postCommand(Command command);

	//! Get the value of a parameter
	std::string getParameterValue(std::string paramName, std::string componentName);

	//! Subscribe to an event (Called by controllers)
	virtual void subscribeToEvent(std::string eventName, std::string componentName, Controller* cont);

	//! Utility function used for logging
	std::string getName(){return "ControllerManager";}

};

} /* namespace iris */

#endif /* CONTROLLERMANAGER_H_ */
