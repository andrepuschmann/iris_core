/**
 * \file StackComponentManager.h
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
 * The Stack Component Manager - loads/unloads all stack components for the
 * StackEngine.
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
*  The StackEngine uses the StackComponentManager to manage the lifecycle of its components
*/
class StackComponentManager
{
private:
  //! Repositories of components which can be loaded by this manager
  std::vector< Repository > repositories_;

  //! Loaded component libraries
  std::vector< ComponentLibrary > loadedLibraries_;

public:
  //! ctor
  StackComponentManager();

  /** Add a repository 
  *   Multiple repositories can be specified by separating paths with ";"
  *   \param repoPath   Path to the component repository
  */
  void addRepository(std::string repoPath);
  
  /** Load a StackComponent
  *   \param desc   Description of StackComponent to be loaded
  *   \returns A pointer to the loaded StackComponent
  */
  boost::shared_ptr<StackComponent> loadComponent(ComponentDescription desc);

  /** Check whether a component exists
  *   \param  name  The name of the component to look for
  */
  bool componentExists(std::string name);

  //! Get all the repositories which are available
  std::vector<boost::filesystem::path> getRepositories();

  //! Get the name of this element
  std::string getName(){return "StackComponentManager";};
};

} // namespace iris
#endif // STACKCOMPONENTMANAGER_H_
