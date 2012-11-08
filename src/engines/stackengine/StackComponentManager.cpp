/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file StackComponentManager.cpp
 * Implementation of StackComponentManager class.
 *
 *  Created on: 2-Mar-2010
 *  Created by: suttonp
 *  $Revision: 1381 $
 *  $LastChangedDate: 2011-11-23 13:37:07 +0000 (Wed, 23 Nov 2011) $
 *  $LastChangedBy: suttonp $
 */

#include <sstream>
#include <boost/algorithm/string.hpp>

#include "irisapi/Version.h"
#include "StackComponentManager.h"

using namespace std;
namespace b=boost;
namespace bfs=boost::filesystem;

namespace iris
{
    StackComponentManager::StackComponentManager()
    {
    }

	void StackComponentManager::addRepository(std::string repoPath) throw (ResourceNotFoundException)
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
				LOG(LFATAL) << "Could not add repository " << str << " path does not exist or is not a directory.";
                throw ResourceNotFoundException("Could not add repository " + str + " path does not exist or is not a directory.");
            }

            //Create a repository object
            Repository tmp;
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
                        ComponentLibrary current;
                        current.path = dir_iter->path();
                        current.name = stem;
                        boost::to_lower(current.name);
                        tmp.componentLibs.push_back(current);
                    }
                }
            }

            //Add to vector of repository paths
            d_repositories.push_back(tmp);
        }
    }

    b::shared_ptr<StackComponent> StackComponentManager::loadComponent(ComponentDescription desc)
        throw (IrisException)
    {
        ComponentLibrary temp;

        //Check if the library has already been loaded
        vector< ComponentLibrary >::iterator libIt;
        for(libIt=d_loadedLibraries.begin();libIt!=d_loadedLibraries.end();++libIt)
        {
            if(libIt->name == desc.type)
                temp = *libIt;
        }

        //If the library hasn't already been loaded, look for it in our repositories
        if(temp.name == "")
        {
            vector< Repository >::iterator repIt;
            for(repIt=d_repositories.begin();repIt!=d_repositories.end();++repIt)
            {
                vector< ComponentLibrary >::iterator compIt;
                for(compIt=repIt->componentLibs.begin();compIt!=repIt->componentLibs.end();++compIt)
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

        //Check that we found the library
        if(temp.name == "")
		{
			LOG(LFATAL) << "Could not find component " << desc.type << " in repositories.";
            throw ResourceNotFoundException("Could not find component " + desc.type + " in repositories.");
		}

        //Load it if necessary
        if(temp.libPtr == NULL)
        {
            //Load the component library and set the pointer, add to vector of loaded libraries
            temp.libPtr.reset(new SharedLibrary(temp.path));
            d_loadedLibraries.push_back(temp);
        }

        //Pull a PNComponent class out of the library
        CREATEFUNCTION createFunction = (CREATEFUNCTION)temp.libPtr->getSymbol("CreateComponent");
        DESTROYFUNCTION destroyFunction = (DESTROYFUNCTION)temp.libPtr->getSymbol("ReleaseComponent");
		GETAPIVERSIONFUNCTION getApiFunction = (GETAPIVERSIONFUNCTION)temp.libPtr->getSymbol("GetApiVersion"); 

		//Check API version numbers match
		string coreVer, moduleVer;
		coreVer = Version::getApiVersion();
		moduleVer = getApiFunction();
		if(coreVer != moduleVer)
		{	
			stringstream message;
			message << "API version mismatch between core and component " << desc.name << \
				". Core API version = " << coreVer << ". Module API version = " << moduleVer << ".";
			LOG(LFATAL) << message.str();
			throw ApiVersionException(message.str());
		}

        //Create a shared_ptr and use a custom deallocator so the component is destroyed from the library
        boost::shared_ptr<StackComponent> comp(createFunction(desc.name), destroyFunction);

        //Set the LoggingPolicy
        comp->setLoggingPolicy(Logger::getPolicy());

        //Set the parameter values here
        for(vector<ParameterDescription>::iterator i = desc.parameters.begin();
            i != desc.parameters.end(); ++i)
        {
            comp->setValue(i->name, i->value);
        }

        //Prompt the component to register its ports
        comp->registerPorts();

        return comp;
    }

    bool StackComponentManager::componentExists(std::string name)
    {
        //Look for the component in our repositories
        vector< Repository >::iterator repIt;
        for(repIt=d_repositories.begin();repIt!=d_repositories.end();++repIt)
        {
            vector< ComponentLibrary >::iterator compIt;
            for(compIt=repIt->componentLibs.begin();compIt!=repIt->componentLibs.end();++compIt)
            {
                if(compIt->name == name)
                {
                    return true;
                }
            }
        }
        return false;
    }

    vector<bfs::path> StackComponentManager::getRepositories()
    {
        vector<bfs::path> paths;
        vector<Repository>::iterator it;
        for(it=d_repositories.begin();it!=d_repositories.end();++it)
        {
            paths.push_back(it->path);
        }
        return paths;
    }

} /* namespace iris */
