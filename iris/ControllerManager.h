/**
 * \file ControllerManager.h
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
 * The ControllerManager manages all controllers running within the Iris architecture.
 */

#ifndef IRIS_CONTROLLERMANAGER_H_
#define IRIS_CONTROLLERMANAGER_H_

#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>
#include "boost/filesystem.hpp"

#include "iris/SharedLibrary.h"
#include "iris/ControllerManagerCallbackInterface.h"
#include "iris/RadioRepresentation.h"
#include "irisapi/Event.h"
#include "irisapi/Controller.h"



namespace iris
{

/// Function pointer for "GetApiVersion" in controller library
typedef const char* (*GETAPIVERSIONFUNCTION)();
/// Function pointer for "CreateController" in controller library
typedef Controller* (*CREATECONTROLLERFUNCTION)();
/// Function pointer for "ReleaseController" in controller library
typedef void (*DESTROYCONTROLLERFUNCTION)(Controller*);

/// A controller library - contains the library (used to create a controller object)
struct ControllerLibrary
{
  boost::filesystem::path path;
  std::string name;
  boost::shared_ptr< SharedLibrary > libPtr;

  /// Constructor initializes our variables
  ControllerLibrary()
    :name(""){}
};

/// A loaded controller object
struct LoadedController
{
  std::string name;
  boost::shared_ptr< Controller > contPtr;

  LoadedController(std::string name, boost::shared_ptr< Controller > contPtr)
    :name(name), contPtr(contPtr){}
};

/// A repository containing a number of controller libraries
struct ControllerRepository
{
  boost::filesystem::path path;
  std::vector< ControllerLibrary > controllerLibs;
};


/// The ControllerManager manages all controllers running within the IRIS architecture.
class ControllerManager
  : public ControllerCallbackInterface
{
public:

  ControllerManager();

  /** Set a pointer to the EngineManager which owns this ControllerManager.
   *
   * @param e   Pointer to the callback interface on the EngineManager.
   */
  void setCallbackInterface(ControllerManagerCallbackInterface* e);

  /** Add a repository
   *
   *   Multiple repositories can be specified by separating paths with ";"
   *   \param repoPath   Path to the controller repository
   */
  void addRepository(std::string repoPath);

  /** Load a Controller
   *
   *   \param  name   Name of Controller to be loaded
   */
  void loadController(ControllerDescription desc);

  /// Start all Controllers
  void startControllers();

  /// Stop all Controllers
  void stopControllers();

  /// Unload all Controllers
  void unloadControllers();

  /** Check whether a controller exists
   *
  *   \param  name  The name of the controller to look for.
  */
  bool controllerExists(std::string name);

  /** Get all the repositories which are available
   *
   * @return Vector of paths to the repositories
   */
  std::vector<boost::filesystem::path> getRepositories();

  /** Activate an event
   *
   * @param e   The event to activate.
   */
  void activateEvent(Event &e);

  /** Reconfigure the radio
   *
   * @param reconfigs   A set of reconfigurations.
   */
  virtual void reconfigureRadio(ReconfigSet reconfigs);

  /** Post a command to a component
   *
   * @param command   The command to post.
   */
  virtual void postCommand(Command command);

  /** Get the value of a parameter
   *
   * @param paramName       The name of the parameter.
   * @param componentName   The name of the component containing the parameter.
   * @return
   */
  std::string getParameterValue(std::string paramName, std::string componentName);

  /** Subscribe to an event (Called by controllers)
   *
   * @param eventName       The event name.
   * @param componentName   The name of the component triggering the event.
   * @param cont            A pointer to the subscribing controller.
   */
  virtual void subscribeToEvent(std::string eventName, std::string componentName, Controller* cont);

  /// Utility function used for logging
  std::string getName(){return "ControllerManager";}

private:
  std::vector< ControllerLibrary > loadedLibraries_;
  std::vector< LoadedController > loadedControllers_; ///< Our loaded controllers
  std::vector<ControllerRepository> repositories_;    ///< Our repositories
  ControllerManagerCallbackInterface* engineManager_; ///< The EngineManager which owns this

  /** Map of events to subscribed controllers
  *
  *  The keys for the map are formed by concatenating the component name with the event name.
  *  Each key is associated with a vector of Controller* - these are the controllers that have
  *  subscribed to that event.
  */
  std::map< std::string, std::vector<Controller*> >eventMap_;

};

} // namespace iris

#endif // IRIS_CONTROLLERMANAGER_H_
