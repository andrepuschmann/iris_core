/**
 * \file PhyEngine.h
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
 * The PhyEngine implements a process network engine for the Iris framework.
 */

#ifndef IRIS_PHYENGINE_H_
#define IRIS_PHYENGINE_H_

#include <deque>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include "boost/filesystem.hpp"

#include "iris/EngineInterface.h"
#include "iris/DataBuffer.h"
#include "irisapi/Exceptions.h"
#include "irisapi/ComponentCallbackInterface.h"
#include "irisapi/ReconfigurationDescriptions.h"
#include "irisapi/MessageQueue.h"
#include "irisapi/Logging.h"

namespace iris
{
//Forward declarations to avoid inter-element dependencies
class PhyComponent;
class PhyComponentManager;

/** The PhyEngine class implements a process network engine for the IRIS framework.
*
*  Each PhyEngine runs its own thread of execution and serves one or more PhyComponents.
*/
class PhyEngine:public EngineInterface, public ComponentCallbackInterface
{
public:
  PhyEngine(std::string name, std::string repository);
  ~PhyEngine();

  void setEngineManager(EngineCallbackInterface *e);

  /** Load the engine
   *
  *   \param eng          Description of the engine
  *   \param inputLinks   The input DataBuffers for this engine
  *   \return             The output DataBuffers for this engine
  */
  std::vector< boost::shared_ptr< DataBufferBase > >
  loadEngine(EngineDescription eng,
             std::vector< boost::shared_ptr< DataBufferBase > > inputLinks);

  void unloadEngine();
  void startEngine();
  void stopEngine();
  std::string getName() const;
  void addReconfiguration(ReconfigSet reconfigs);
  void postCommand(Command command);
  void activateEvent(Event &e);

private:
  /// The component manager for this engine
  boost::scoped_ptr< PhyComponentManager > compManager_;

  /// Handle for this engine's thread of execution
  boost::scoped_ptr< boost::thread > thread_;

  /// The graph representing the components within the engine and the links between them
  RadioGraph engineGraph_;

  /// The PhyComponents running within this engine
  std::vector< boost::shared_ptr<PhyComponent> > components_;

  /// The DataBuffers for the internal links between components of this engine
  std::vector< boost::shared_ptr< DataBufferBase > > internalBuffers_;

  /// The DataBuffers for the external links into and out of this engine
  std::vector< boost::shared_ptr< DataBufferBase > > engInputBuffers_;
  std::vector< boost::shared_ptr< DataBufferBase > > engOutputBuffers_;

  /// Name of this engine
  std::string engineName_;

  /// The reconfiguration message queue for this engine
  MessageQueue< ReconfigSet > reconfigQueue_;

  /// The interface to the owner of this engine
  EngineCallbackInterface *engineManager_;


  /// Helper functions
   boost::shared_ptr< DataBufferBase >  createDataBuffer(int type) const;
   boost::shared_ptr< DataBufferBase >  createPhyDataBuffer(int type) const;
  bool sameLink(LinkDescription first, LinkDescription second) const;

  /// The internal loop which this engine's thread executes
  void threadLoop();
  /// Check that a given graph complies with the policies of this engine
  void checkGraph(RadioGraph& graph);
  /// Build a given graph
  void buildEngineGraph(RadioGraph& graph);

  /// Reconfigure a parameter within a component running in the engine
  void reconfigureParameter(ParametricReconfig reconfig);
  /// Reconfigure the structure of this engine
  void reconfigureStructure();

};


} // namespace iris

#endif // IRIS_PHYENGINE_H_
