/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file ControllerManager.cpp
 * Implementation of ControllerManager class.
 *
 *  Created on: 19-May-2009
 *  Created by: suttonp
 *  $Revision: 1381 $
 *  $LastChangedDate: 2011-11-23 13:37:07 +0000 (Wed, 23 Nov 2011) $
 *  $LastChangedBy: suttonp $
 */

#include "iris/ControllerManager.h"
#include <boost/algorithm/string.hpp>

#include "irisapi/Version.h"

using namespace std;
namespace b=boost;
namespace bfs=boost::filesystem;

namespace iris
{

    ControllerManager::ControllerManager()
    {
    }

	void ControllerManager::setCallbackInterface(ControllerManagerCallbackInterface* e)
	{
		d_engineManager = e;
	}

	void ControllerManager::addRepository(std::string repoPath) throw (ResourceNotFoundException)
    {
        while(!repoPath.empty())
        {
            //Extract paths from the repository string
            size_t pos = repoPath.find_first_of(';');
            string str(repoPath, 0, pos);
            repoPath.erase(0, pos);
            // break out of the while if str is empty (e.g. with a repos="/path/to/1;")
            if (str.empty())
                break;

            bfs::path currentPath(str);

            //Check that the path exists and is a directory
            if(!bfs::exists(currentPath) || !bfs::is_directory(currentPath))
            {
                throw ResourceNotFoundException("Could not add controller repository - path does not exist or is not a directory.");
            }

            //Create a repository object
            ControllerRepository tmp;
            tmp.path = currentPath;

            //Go through files in the repository and add to the repository vector of component libraries
            bfs::directory_iterator dir_iter(currentPath), dir_end;
            for(;dir_iter != dir_end; ++dir_iter)
            {
                //Check that the file path contains the system library extension
                string filename = dir_iter->path().leaf();
                size_t pos = filename.find_last_of('.');
                if(pos != string::npos)
                {
                    string extension = filename.substr(pos);
                    if(extension == SharedLibrary::getSystemExtension())
                    {
                    	size_t pre = SharedLibrary::getSystemPrefix().length();
                    	string stem = filename.substr(pre,pos-pre); //Remove library prefix and postfix
                        ControllerLibrary current;
                        current.path = dir_iter->path();
                        current.name = stem;
                        boost::to_lower(current.name);
                        tmp.controllerLibs.push_back(current);
                    }
                }
            }

            //Add to vector of repository paths
            d_repositories.push_back(tmp);
        }
    }

	void ControllerManager::loadController(std::string name)
        throw (IrisException)
    {
        //Check if the library has already been loaded
        vector< LoadedController >::iterator libIt;
        for(libIt=d_loadedControllers.begin();libIt!=d_loadedControllers.end();++libIt)
        {
            if(libIt->name == name)
			{
				LOG(LERROR) << "Controller " + name + " has already been loaded.";
				return;
			}
        }

        //Look for the controller in our repositories
        vector< ControllerRepository >::iterator repIt;
        for(repIt=d_repositories.begin();repIt!=d_repositories.end();++repIt)
        {
            vector< ControllerLibrary >::iterator compIt;
            for(compIt=repIt->controllerLibs.begin();compIt!=repIt->controllerLibs.end();++compIt)
            {
                if(compIt->name == name)
                {
					//Load the controller library and add to vector of loaded libraries
					compIt->libPtr.reset(new SharedLibrary(compIt->path));

					//Pull a Controller class out of the library
					CREATECONTROLLERFUNCTION createFunction = (CREATECONTROLLERFUNCTION)compIt->libPtr->getSymbol("CreateController");
					DESTROYCONTROLLERFUNCTION destroyFunction = (DESTROYCONTROLLERFUNCTION)compIt->libPtr->getSymbol("ReleaseController");
					GETAPIVERSIONFUNCTION getApiFunction = (GETAPIVERSIONFUNCTION)compIt->libPtr->getSymbol("GetApiVersion"); 

					//Check API version numbers match
					string coreVer, moduleVer;
					coreVer = Version::getApiVersion();
					moduleVer = getApiFunction();
					if(coreVer != moduleVer)
					{	
						stringstream message;
						message << "API version mismatch between core and controller " << name << \
							". Core API version = " << coreVer << ". Module API version = " << moduleVer << ".";
						throw ApiVersionException(message.str());
					}

					//Create a shared_ptr and use a custom deallocator so the component is destroyed from the library
					boost::shared_ptr<Controller> cont(createFunction(), destroyFunction);

					//Set the LoggingPolicy and EngineManagerControllerInterface
					cont->setLoggingPolicy(Logger::getPolicy());
					cont->setCallbackInterface(this);

					//Call load on the controller
					cont->load();

					//Add to d_loadedControllers
					LoadedController l(compIt->name, cont);
					d_loadedControllers.push_back(l);	

					return;
                }
            }
        }
        
        //Only get here if we didn't find the library
		throw ResourceNotFoundException("Could not find controller " + name + " in repositories.");
      
    }

    bool ControllerManager::controllerExists(std::string name)
    {
        //Look for the controller in our repositories
        vector< ControllerRepository >::iterator repIt;
        for(repIt=d_repositories.begin();repIt!=d_repositories.end();++repIt)
        {
            vector< ControllerLibrary >::iterator compIt;
            for(compIt=repIt->controllerLibs.begin();compIt!=repIt->controllerLibs.end();++compIt)
            {
                if(compIt->name == name)
                {
                    return true;
                }
            }
        }
        return false;
    }

	void ControllerManager::startControllers()
		throw (IrisException)
	{
		//Go through the loaded Controllers and start each one
		vector< LoadedController>::iterator it;
		for(it = d_loadedControllers.begin(); it != d_loadedControllers.end(); ++it)
		{
			it->contPtr->start();
			LOG(LINFO) << "Controller " + it->name + " started.";
		}
	}

	void ControllerManager::stopControllers()
		throw (IrisException)
	{
		//Go through the loaded Controllers and stop each one
		vector< LoadedController>::iterator it;
		for(it = d_loadedControllers.begin(); it != d_loadedControllers.end(); ++it)
		{
			it->contPtr->stop();
			LOG(LINFO) << "Controller " + it->name + " stopped.";
		}
	}

	void ControllerManager::unloadControllers()
        throw (IrisException)
	{
		//Clear the event-controller map
		d_eventMap.clear();

		//Go through the loaded Controllers and unload each one
		vector< LoadedController>::iterator it;
		for(it = d_loadedControllers.begin(); it != d_loadedControllers.end(); ++it)
		{
			it->contPtr->unload();
			LOG(LINFO) << "Controller " + it->name + " unloaded.";
		}

		//Just clear the vector, boost::shared_ptr will automatically call the custom deallocator
		d_loadedControllers.clear();
	}

    vector<bfs::path> ControllerManager::getRepositories()
    {
        vector<bfs::path> paths;
        vector<ControllerRepository>::iterator it;
        for(it=d_repositories.begin();it!=d_repositories.end();++it)
        {
            paths.push_back(it->path);
        }
        return paths;
    }

	//! Inform controllers that an event has been activated
	void ControllerManager::activateEvent(Event &e)
	{
		vector<Controller*> controllers = d_eventMap[e.eventName + e.componentName];
		vector<Controller*>::iterator contIt;
		for(contIt=controllers.begin();contIt!=controllers.end();++contIt)
		{
			(*contIt)->postEvent(e);
		}
	}

	//! Reconfigure the radio
	void ControllerManager::reconfigureRadio(ReconfigSet reconfigs)
	{
		if(d_engineManager == NULL)
			return;

		d_engineManager->reconfigureRadio(reconfigs);
	}

	//! Post a command to a component
	void ControllerManager::postCommand(Command command)
	{
		if(d_engineManager == NULL)
			return;

		d_engineManager->postCommand(command);
	}

	//! Get the value of a parameter
	std::string ControllerManager::getParameterValue(std::string paramName, std::string componentName)
	{
		return d_engineManager->getParameterValue(paramName, componentName);
	}

	//! Subscribe to an event
	void ControllerManager::subscribeToEvent(std::string eventName, std::string componentName, Controller* cont)
	{
		d_eventMap[eventName + componentName].push_back(cont); 
	}


} /* namespace iris */
