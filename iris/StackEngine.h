/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file StackEngine.h
 * The Stack engine
 *
 *  Created on: 1-Mar-2010
 *  Created by: suttonp
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
 *
 */

#ifndef STACKENGINE_H_
#define STACKENGINE_H_

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

/** The StackEngine class implements a stack engine for the IRIS framework.
*
*/
class StackEngine:public EngineInterface, public ComponentCallbackInterface
{
private:
    //! The graph representing the components within the engine and the links between them
    RadioGraph d_engineGraph;

    //! The StackComponents running within this engine
    std::vector< boost::shared_ptr<StackComponent> > d_components;

	//! The StackInTranslators running within this engine
	std::vector< boost::shared_ptr<StackInTranslator> > d_inTranslators;

	//! The StackOutTranslators running within this engine
	std::vector< boost::shared_ptr<StackOutTranslator> > d_outTranslators;

    //! The DataBuffers for the external links into and out of this engine
    std::vector< boost::shared_ptr< DataBufferBase > > d_engInputBuffers;
    std::vector< boost::shared_ptr< DataBufferBase > > d_engOutputBuffers;

    //! Name of this engine
    std::string d_engineName;

    //! The component manager for this engine
	boost::scoped_ptr< StackComponentManager > d_compManager;

	//! The interface to the owner of this engine
	EngineCallbackInterface *d_engineManager;

    // Helper functions
	void createExternalLink(LinkDescription& l);
    bool sameLink(LinkDescription first, LinkDescription second) const;
	boost::shared_ptr< StackComponent > findComponent(std::string name);

    //! Check that a given graph complies with the policies of this engine
    void checkGraph(RadioGraph& graph);
    //! Build a given graph
    void buildEngineGraph(EngineDescription& eng)
        throw (IrisException);

    //! Reconfigure a parameter within a component running in the engine
    void reconfigureParameter(ParametricReconfig reconfig);
    //! Reconfigure the structure of this engine
    void reconfigureStructure();
public:
    //! ctor
    StackEngine(std::string name, std::string repository)
        throw (IrisException);
    //! dtor
    ~StackEngine();

	void setEngineManager(EngineCallbackInterface *e);

    /** Load the engine
    *   \param eng          Description of the engine
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

};


} /* namespace iris */

#endif /* STACKENGINE_H_ */
