/**
 * \file PhyComponentManager.cpp
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
 * Implementation of PhyComponentManager class - loads/unloads PhyComponents
 * for the PhyEngine.
 */

#include <sstream>
#include <boost/algorithm/string.hpp>

#include "irisapi/Version.h"
#include "PhyComponentManager.h"

using namespace std;
namespace b=boost;
namespace bfs=boost::filesystem;

namespace iris
{

PhyComponentManager::PhyComponentManager()
{
}

void PhyComponentManager::addRepository(std::string repoPath)
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
                    ComponentLibrary current;
                    current.path = dir_iter->path();
                    current.name = stem;
                    boost::to_lower(current.name);
                    tmp.componentLibs.push_back(current);
                }
            }
        }

        //Add to vector of repository paths
        repositories_.push_back(tmp);
    }
}

b::shared_ptr<PhyComponent> PhyComponentManager::loadComponent(ComponentDescription desc)
{
    ComponentLibrary temp;

    //Check if the library has already been loaded
    vector< ComponentLibrary >::iterator libIt;
    for(libIt=loadedLibraries_.begin();libIt!=loadedLibraries_.end();++libIt)
    {
        if(libIt->name == desc.type)
            temp = *libIt;
    }

    //If the library hasn't already been loaded, look for it in our repositories
    if(temp.name == "")
    {
        vector< Repository >::iterator repIt;
        for(repIt=repositories_.begin();repIt!=repositories_.end();++repIt)
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
        loadedLibraries_.push_back(temp);
    }

    //Pull a PhyComponent class out of the library
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
    boost::shared_ptr<PhyComponent> comp(createFunction(desc.name), destroyFunction);

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

bool PhyComponentManager::componentExists(std::string name)
{
    //Look for the component in our repositories
    vector< Repository >::iterator repIt;
    for(repIt=repositories_.begin();repIt!=repositories_.end();++repIt)
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

vector<bfs::path> PhyComponentManager::getRepositories()
{
    vector<bfs::path> paths;
    vector<Repository>::iterator it;
    for(it=repositories_.begin();it!=repositories_.end();++it)
    {
        paths.push_back(it->path);
    }
    return paths;
}

} /* namespace iris */
