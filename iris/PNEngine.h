/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file PNEngine.h
 * The Process Network engine
 *
 *  Created on: 2-Jan-2009
 *  Created by: suttonp
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
 *
 */

#ifndef PNENGINE_H_
#define PNENGINE_H_

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
class PNComponent;
class PNComponentManager;

/** The PNEngine class implements a process network engine for the IRIS framework.
*
*	Each PNEngine runs its own thread of execution and serves one or more PNComponents.
*/
class PNEngine:public EngineInterface, public ComponentCallbackInterface
{
private:
	//! The component manager for this engine
	boost::scoped_ptr< PNComponentManager > d_compManager;

    //! Handle for this engine's thread of execution
    boost::scoped_ptr< boost::thread > d_thread;

    //! The graph representing the components within the engine and the links between them
    RadioGraph d_engineGraph;

    //! The PNComponents running within this engine
    std::vector< boost::shared_ptr<PNComponent> > d_components;

    //! The DataBuffers for the internal links between components of this engine
	std::vector< boost::shared_ptr< DataBufferBase > > d_internalBuffers;

    //! The DataBuffers for the external links into and out of this engine
    std::vector< boost::shared_ptr< DataBufferBase > > d_engInputBuffers;
    std::vector< boost::shared_ptr< DataBufferBase > > d_engOutputBuffers;

    //! Name of this engine
    std::string d_engineName;

    //! The reconfiguration message queue for this engine
    MessageQueue< ReconfigSet > d_reconfigQueue;

	//! The interface to the owner of this engine
	EngineCallbackInterface *d_engineManager;


    //! Helper functions
     boost::shared_ptr< DataBufferBase >  createDataBuffer(int type) const;
	 boost::shared_ptr< DataBufferBase >  createPNDataBuffer(int type) const;
    bool sameLink(LinkDescription first, LinkDescription second) const;

    //! The internal loop which this engine's thread executes
    void threadLoop();
    //! Check that a given graph complies with the policies of this engine
    void checkGraph(RadioGraph& graph);
    //! Build a given graph
    void buildEngineGraph(RadioGraph& graph)
        throw (IrisException);

    //! Reconfigure a parameter within a component running in the engine
    void reconfigureParameter(ParametricReconfig reconfig);
    //! Reconfigure the structure of this engine
    void reconfigureStructure();
public:
    //! ctor
    PNEngine(std::string name, std::string repository)
        throw (IrisException);
    //! dtor
    ~PNEngine();

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


}
#endif
