/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file StackComponentManager.h
 * The Stack Component Manager
 *
 *  Created on: 2-Mar-2010
 *  Created by: suttonp
 *  $Revision: 851 $
 *  $LastChangedDate: 2010-03-05 12:37:47 +0000 (Fri, 05 Mar 2010) $
 *  $LastChangedBy: suttonp $
 *
 */

#ifndef STACKCOMPONENTMANAGER_H_
#define STACKCOMPONENTMANAGER_H_

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include "boost/filesystem.hpp"

#include "iris/SharedLibrary.h"
#include "iris/RadioRepresentation.h"
#include "irisapi/Logging.h"
#include "irisapi/StackComponent.h"
#include "irisapi/Exceptions.h"

namespace iris
{

//! Function pointer for "GetApiVersion" in component library
typedef const char* (*GETAPIVERSIONFUNCTION)();
//! Function pointer for "CreateComponent" in component library
typedef StackComponent* (*CREATEFUNCTION)(std::string);
//! Function pointer for "ReleaseComponent" in component library
typedef void (*DESTROYFUNCTION)(StackComponent*);

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

/** The StackComponentManager class implements a component manager for the StackEngine.
*
*	The StackEngine uses the StackComponentManager to manage the lifecycle of its components
*/
class StackComponentManager
{
private:
    //! Repositories of components which can be loaded by this manager
    std::vector< Repository > d_repositories;

    //! Loaded component libraries
    std::vector< ComponentLibrary > d_loadedLibraries;

public:
    //! ctor
    StackComponentManager();

    /** Add a repository 
    *   Multiple repositories can be specified by separating paths with ";"
    *   \param repoPath     Path to the component repository
    */
    void addRepository(std::string repoPath) throw (ResourceNotFoundException);
    
    /** Load a StackComponent
    *   \param desc     Description of StackComponent to be loaded
    *   \returns A pointer to the loaded StackComponent
    */
    boost::shared_ptr<StackComponent> loadComponent(ComponentDescription desc) throw (IrisException);

    /** Check whether a component exists
    *   \param  name    The name of the component to look for
    */
    bool componentExists(std::string name);

    //! Get all the repositories which are available
    std::vector<boost::filesystem::path> getRepositories();

	//! Get the name of this element
	std::string getName(){return "PNComponentManager";};
};

} /* namespace iris */
#endif /* STACKCOMPONENTMANAGER_H_ */
