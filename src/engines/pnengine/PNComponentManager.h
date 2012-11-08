/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file PNComponentManager.h
 * The Process Network Component Manager
 *
 *  Created on: 4-Jan-2009
 *  Created by: suttonp
 *  $Revision: 1317 $
 *  $LastChangedDate: 2011-09-13 13:01:51 +0100 (Tue, 13 Sep 2011) $
 *  $LastChangedBy: suttonp $
 *
 */

#ifndef PNCOMPONENTMANAGER_H_
#define PNCOMPONENTMANAGER_H_

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include "boost/filesystem.hpp"

#include "iris/SharedLibrary.h"
#include "iris/RadioRepresentation.h"
#include "irisapi/Logging.h"
#include "irisapi/PNComponent.h"
#include "irisapi/Exceptions.h"

namespace iris
{

//! Function pointer for "GetApiVersion" in component library
typedef const char* (*GETAPIVERSIONFUNCTION)();
//! Function pointer for "CreateComponent" in component library
typedef PNComponent* (*CREATEFUNCTION)(std::string);
//! Function pointer for "ReleaseComponent" in component library
typedef void (*DESTROYFUNCTION)(PNComponent*);

//! A component library
struct ComponentLibrary
{
    boost::filesystem::path path;
    std::string name;
    boost::shared_ptr< SharedLibrary > libPtr;

    //! Constructor initializes our variables
    ComponentLibrary()
        :name(""){}
};

//! A repository containing a number of component libraries
struct Repository
{
    boost::filesystem::path path;
    std::vector< ComponentLibrary > componentLibs;
};

/** The PNComponentManager class implements a component manager for the PNEngine.
*
*	The PNEngine uses the PNComponentManager to manage the lifecycle of its components
*/
class PNComponentManager
{
private:
    //! Repositories of components which can be loaded by this manager
    std::vector< Repository > d_repositories;

    //! Loaded component libraries
    std::vector< ComponentLibrary > d_loadedLibraries;

public:
    //! ctor
    PNComponentManager();

    /** Add a repository 
    *   Multiple repositories can be specified by separating paths with ";"
    *   \param repoPath     Path to the component repository
    */
    void addRepository(std::string repoPath) throw (ResourceNotFoundException);
    
    /** Load a PNComponent
    *   \param desc     Description of PNComponent to be loaded
    *   \returns A pointer to the loaded PNComponent
    */
    boost::shared_ptr<PNComponent> loadComponent(ComponentDescription desc) throw (IrisException);

    /** Check whether a component exists
    *   \param  name    The name of the component to look for
    */
    bool componentExists(std::string name);

    //! Get all the repositories which are available
    std::vector<boost::filesystem::path> getRepositories();

	//! Get the name of this element
	std::string getName(){return "PNComponentManager";};
};

}
#endif
