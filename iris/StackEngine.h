/**
 * \file StackEngine.h
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
 * The StackEngine class implements a network stack engine for the Iris framework.
 */

#ifndef IRIS_STACKENGINE_H_
#define IRIS_STACKENGINE_H_

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
class StackComponent;
class StackInTranslator;
class StackOutTranslator;
class StackComponentManager;

/** The StackEngine can be used to build a network stack in Iris.
 *
 * Components in a StackEngine run their own thread and pass messages
 * up and down to other components above and below them.
 */
class StackEngine
  : public EngineInterface, public ComponentCallbackInterface
{
public:
  StackEngine(std::string name, std::string repository) throw (IrisException);
  ~StackEngine();

  void setEngineManager(EngineCallbackInterface *e);

  /** Load the engine
  *   \param eng      Description of the engine
  *   \param inputLinks   The input DataBuffers for this engine
  *   \returns The output DataBuffers for this engine
  */
  std::vector< boost::shared_ptr< DataBufferBase > > loadEngine(EngineDescription eng, std::vector< boost::shared_ptr< DataBufferBase > > inputLinks)
    throw (IrisException);
  void unloadEngine();
  void startEngine();
  void stopEngine();
  std::string getName() const;
  void addReconfiguration(ReconfigSet reconfigs);
  void postCommand(Command command);
  void activateEvent(Event &e);

private:
  /// The graph representing the components within the engine and the links between them
  RadioGraph engineGraph_;

  /// The StackComponents running within this engine
  std::vector< boost::shared_ptr<StackComponent> > components_;

  /// The StackInTranslators running within this engine
  std::vector< boost::shared_ptr<StackInTranslator> > inTranslators_;

  /// The StackOutTranslators running within this engine
  std::vector< boost::shared_ptr<StackOutTranslator> > outTranslators_;

  /// The DataBuffers for the external links into and out of this engine
  std::vector< boost::shared_ptr< DataBufferBase > > engInputBuffers_;
  std::vector< boost::shared_ptr< DataBufferBase > > engOutputBuffers_;

  /// Name of this engine
  std::string engineName_;

  /// The component manager for this engine
  boost::scoped_ptr< StackComponentManager > compManager_;

  /// The interface to the owner of this engine
  EngineCallbackInterface *engineManager_;

  // Helper functions
  void createExternalLink(LinkDescription& l);
  bool sameLink(LinkDescription first, LinkDescription second) const;
  boost::shared_ptr< StackComponent > findComponent(std::string name);

  /// Check that a given graph complies with the policies of this engine
  void checkGraph(RadioGraph& graph);
  /// Build a given graph
  void buildEngineGraph(EngineDescription& eng)
    throw (IrisException);

  /// Reconfigure a parameter within a component running in the engine
  void reconfigureParameter(ParametricReconfig reconfig);
  /// Reconfigure the structure of this engine
  void reconfigureStructure();

};

} // namespace iris

#endif // IRIS_STACKENGINE_H_
