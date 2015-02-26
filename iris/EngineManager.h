/**
 * \file EngineManager.h
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
 * The EngineManager controls all engines running within the Iris architecture.
 */

#ifndef IRIS_ENGINEMANAGER_H_
#define IRIS_ENGINEMANAGER_H_

#include <boost/graph/topological_sort.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>   //Autodeleting, Exception-safe container of pointers
#include "boost/filesystem.hpp"

#include "iris/RadioRepresentation.h"
#include "iris/PhyEngine.h"
#include "iris/ControllerManager.h"
#include "irisapi/Exceptions.h"
#include "irisapi/Logging.h"

namespace iris
{

/// A container for component repository strings
struct Repositories
{
  std::string stackRepository;
  std::string phyRepository;
  std::string sdfRepository;
  std::string contRepository;
};

/// The EngineManager controls all engines running within the Iris architecture.
class EngineManager:
  public EngineCallbackInterface,
  public ControllerManagerCallbackInterface
{
private:
  ControllerManager controllerManager_;   ///< The single ControllerManager for this radio.
  boost::ptr_vector<EngineInterface> engines_;  ///< Engines of the current radio.
  Repositories reps_;         ///< The paths to the radio component repositories
  EngineGraph engineGraph_;   ///< Graph of all radio engines and the links between them.

  /// The current radio representation - gets updated with any reconfigurations which occur.
  RadioRepresentation radioRep_;

  /** Create an engine
   *
   *   \param  d    Description of the engine to be created.
   *   \returns     Pointer to the created engine.
   */
  EngineInterface* createEngine(EngineDescription& d);

  /** Check whether two links are equivalent
  *
  *   \param  first     The first link to be compared.
  *   \param  second    The second link to be compared.
  *   \returns Whether the links are equivalent
  */
  bool sameLink(LinkDescription first, LinkDescription second);

public:
  EngineManager();
  void setRepositories(Repositories reps){reps_ = reps;}
  Repositories getRepositories() const {return reps_;}
  void loadRadio(RadioRepresentation rad);
  void startRadio();
  void stopRadio();
  void unloadRadio();
  RadioRepresentation& getCurrentRadio();
  void reconfigureRadio(ReconfigSet reconfigs);
  void postCommand(Command command);
  std::string getParameterValue(std::string paramName, std::string componentName);
  std::string getParameterName(std::string componentName, int paramIndex, std::string &paramValue);
  std::string getEngineName(std::string componentName, int *engineIndex, int *compIndex);
  int getNrEngines();
  int getNrComponents();
  std::string getEngineNameFromIndex(int index);
  std::string getComponentName(int index);
  int getNrParameters(std::string componentName);
  void activateEvent(Event &e);
};

} // namespace iris

#endif // IRIS_ENGINEMANAGER_H_
