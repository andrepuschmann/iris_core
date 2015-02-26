/**
 * \file ControllerManager.cpp
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
 * Implementation of ControllerManager class - manages all controllers
 * running in Iris.
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
        engineManager_ = e;
    }

    void ControllerManager::addRepository(std::string repoPath)
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
#if BOOST_FILESYSTEM_VERSION == 2
              string filename = dir_iter->path().leaf();
#else
              string filename = dir_iter->path().filename().string();
#endif
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
            repositories_.push_back(tmp);
        }
    }

    void ControllerManager::loadController(ControllerDescription desc)
    {
        ControllerLibrary temp;

        //Check if the library has already been loaded
        vector< ControllerLibrary >::iterator libIt;
        for(libIt=loadedLibraries_.begin();libIt!=loadedLibraries_.end();++libIt)
        {
            if(libIt->name == desc.type)
                temp = *(libIt);
        }

        //If the library hasn't already been loaded, look for it in our repositories
        if(temp.name == "")
        {
          vector< ControllerRepository >::iterator repIt;
          for(repIt=repositories_.begin();repIt!=repositories_.end();++repIt)
          {
              vector< ControllerLibrary >::iterator compIt;
              for(compIt=repIt->controllerLibs.begin();compIt!=repIt->controllerLibs.end();++compIt)
              {
                  if(compIt->name == desc.type)
                  {
                    if(temp.name == "")
                    {   //First component found which matches
                        temp = *compIt;
                    }
                    else if(bfs::last_write_time(compIt->path) > bfs::last_write_time(temp.path))
                    {   //Found more than one component which matches - choose the more recent one
                        temp = *compIt;
                    }
                  }
              }
          }
        }

        //Check that we found it
        if(temp.name == "")
        {
          LOG(LFATAL) << "Could not find controller " << desc.name << " in repositories.";
          throw ResourceNotFoundException("Could not find controller " + desc.type + " in repositories.");
        }

        //Load if necessary
        if(temp.libPtr == NULL)
        {
          temp.libPtr.reset(new SharedLibrary(temp.path));
          loadedLibraries_.push_back(temp);
        }

        //Pull a Controller class out of the library
        CREATECONTROLLERFUNCTION createFunction = (CREATECONTROLLERFUNCTION)temp.libPtr->getSymbol("CreateController");
        DESTROYCONTROLLERFUNCTION destroyFunction = (DESTROYCONTROLLERFUNCTION)temp.libPtr->getSymbol("ReleaseController");
        GETAPIVERSIONFUNCTION getApiFunction = (GETAPIVERSIONFUNCTION)temp.libPtr->getSymbol("GetApiVersion");

        //Check API version numbers match
        string coreVer, moduleVer;
        coreVer = Version::getApiVersion();
        moduleVer = getApiFunction();
        if(coreVer != moduleVer)
        {
            stringstream message;
            message << "API version mismatch between core and controller " << desc.type << \
                ". Core API version = " << coreVer << ". Module API version = " << moduleVer << ".";
            throw ApiVersionException(message.str());
        }

        //Create a shared_ptr and use a custom deallocator so the component is destroyed from the library
        boost::shared_ptr<Controller> cont(createFunction(), destroyFunction);

        //Set the LoggingPolicy and EngineManagerControllerInterface
        cont->setLoggingPolicy(Logger::getPolicy());
        cont->setCallbackInterface(this);

        //Set the parameter values here
        vector<ParameterDescription>::iterator i = desc.parameters.begin();
        for(;i != desc.parameters.end(); ++i)
        {
          cont->setValue(i->name, i->value);
        }

        //Call load on the controller
        cont->load();

        //Add to loadedControllers_
        LoadedController l(temp.name, cont);
        loadedControllers_.push_back(l);

        return;
    }

    bool ControllerManager::controllerExists(std::string name)
    {
        //Look for the controller in our repositories
        vector< ControllerRepository >::iterator repIt;
        for(repIt=repositories_.begin();repIt!=repositories_.end();++repIt)
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
    {
        //Go through the loaded Controllers and start each one
        vector< LoadedController>::iterator it;
        for(it = loadedControllers_.begin(); it != loadedControllers_.end(); ++it)
        {
            it->contPtr->start();
            LOG(LINFO) << "Controller " + it->name + " started.";
        }
    }

    void ControllerManager::stopControllers()
    {
        //Go through the loaded Controllers and stop each one
        vector< LoadedController>::iterator it;
        for(it = loadedControllers_.begin(); it != loadedControllers_.end(); ++it)
        {
            it->contPtr->stop();
            LOG(LINFO) << "Controller " + it->name + " stopped.";
        }
    }

    void ControllerManager::unloadControllers()
    {
        //Clear the event-controller map
        eventMap_.clear();

        //Go through the loaded Controllers and unload each one
        vector< LoadedController>::iterator it;
        for(it = loadedControllers_.begin(); it != loadedControllers_.end(); ++it)
        {
            it->contPtr->unload();
            LOG(LINFO) << "Controller " + it->name + " unloaded.";
        }

        //Just clear the vector, boost::shared_ptr will automatically call the custom deallocator
        loadedControllers_.clear();
    }

    vector<bfs::path> ControllerManager::getRepositories()
    {
        vector<bfs::path> paths;
        vector<ControllerRepository>::iterator it;
        for(it=repositories_.begin();it!=repositories_.end();++it)
        {
            paths.push_back(it->path);
        }
        return paths;
    }

    //! Inform controllers that an event has been activated
    void ControllerManager::activateEvent(Event &e)
    {
        vector<Controller*> controllers = eventMap_[e.eventName + e.componentName];
        vector<Controller*>::iterator contIt;
        for(contIt=controllers.begin();contIt!=controllers.end();++contIt)
        {
            (*contIt)->postEvent(e);
        }
    }

    //! Reconfigure the radio
    void ControllerManager::reconfigureRadio(ReconfigSet reconfigs)
    {
        if(engineManager_ == NULL)
            return;

        engineManager_->reconfigureRadio(reconfigs);
    }

    //! Post a command to a component
    void ControllerManager::postCommand(Command command)
    {
        if(engineManager_ == NULL)
            return;

        engineManager_->postCommand(command);
    }

    //! Get the value of a parameter
    std::string ControllerManager::getParameterValue(std::string paramName, std::string componentName)
    {
        return engineManager_->getParameterValue(paramName, componentName);
    }

    //! Subscribe to an event
    void ControllerManager::subscribeToEvent(std::string eventName, std::string componentName, Controller* cont)
    {
        eventMap_[eventName + componentName].push_back(cont); 
    }


    //! Get the parameter name and value (returned by reference)
    std::string ControllerManager::getParameterName(std::string componentName, int paramIndex, std::string &paramValue)
    {
        return engineManager_->getParameterName(componentName, paramIndex, paramValue);
    }

    //! Get the engine name
    std::string ControllerManager::getEngineName(std::string componentName, int *engineIndex, int *compIndex)
    {
        return engineManager_->getEngineName(componentName, engineIndex, compIndex);
    }

    //! Get the number of engines in the current radio
    int ControllerManager::getNrEngines()
    {
        return engineManager_->getNrEngines();
    }

    //! Get the number of components in the current radio
    int ControllerManager::getNrComponents()
    {
        return engineManager_->getNrComponents();
    }

    //! Get the engine name for a given index
    std::string ControllerManager::getEngineNameFromIndex(int index)
    {
        return engineManager_->getEngineNameFromIndex(index);
    }

    //! Get the component name for a given component index
    std::string ControllerManager::getComponentName(int index)
    {
        return engineManager_->getComponentName(index);
    }

    //! Get the number of parameters of a given component
    int  ControllerManager::getNrParameters(std::string componentName)
    {
        return engineManager_->getNrParameters(componentName);
    }

} /* namespace iris */
