/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file EngineManager.cpp
 * Implementation of EngineManager class.
 *
 *  Created on: 12-jan-2008
 *  Created by: suttonp
 *  $Revision: 1316 $
 *  $LastChangedDate: 2011-09-13 12:41:16 +0100 (Tue, 13 Sep 2011) $
 *  $LastChangedBy: suttonp $
 */

#include "iris/EngineManager.h"
#include "iris/PNEngine.h"
#include "iris/StackEngine.h"

using namespace std;
using namespace boost;

namespace iris
{

    EngineManager::EngineManager()
    {
		d_controllerManager.setCallbackInterface(this);
    }

    void EngineManager::loadRadio(RadioRepresentation rad)
    {
    	//Set the current radio representation
    	d_radioRep = rad;

		//Set the controller repositories in the ControllerManager
		d_controllerManager.addRepository(d_reps.contRepository);

		//Load the controllers
		vector<ControllerDescription> conts = rad.getControllers();
		vector<ControllerDescription>::iterator contIt;
		for(contIt=conts.begin(); contIt!=conts.end(); ++contIt)
		{
			d_controllerManager.loadController(contIt->type);
		}

        d_engineGraph = rad.getEngineGraph();

        //Create the engines
        EngVertexIterator i, iend;
        for(tie(i,iend) = vertices(d_engineGraph); i != iend; ++i)
        {
            //Get the engine description
            EngineDescription current = d_engineGraph[*i];

            //Add the engine to our vector
            d_engines.push_back(createEngine(current));
        }

        //Do a topological sort of the graph
        deque<unsigned> topoOrder;
        topological_sort(d_engineGraph, front_inserter(topoOrder), vertex_index_map(identity_property_map()));

        //Go through graph in topological order and set buffers
        for(deque<unsigned>::iterator i = topoOrder.begin(); i != topoOrder.end(); ++i)
	    {
            //Get input buffers
            vector< shared_ptr< DataBufferBase > > inputBuffers, outputBuffers;
            EngInEdgeIterator edgeIt, edgeItEnd;
            for(tie(edgeIt, edgeItEnd) = in_edges(*i, d_engineGraph); edgeIt != edgeItEnd; ++edgeIt)
            {
                inputBuffers.push_back(d_engineGraph[*edgeIt].theBuffer);
            }

            //Load the engine, passing in the input buffers
            outputBuffers = d_engines[*i].loadEngine(d_engineGraph[*i], inputBuffers);

            //Set the ouput buffers in the graph edges
            EngOutEdgeIterator outEdgeIt, outEdgeItEnd;
            for(tie(outEdgeIt, outEdgeItEnd) = out_edges(*i, d_engineGraph); outEdgeIt != outEdgeItEnd; ++outEdgeIt)
            {
                for(vector< shared_ptr< DataBufferBase > >::iterator it = outputBuffers.begin(); it != outputBuffers.end(); ++it)
                {
                    LinkDescription first = (*it)->getLinkDescription();
                    LinkDescription second = d_engineGraph[*outEdgeIt];
                    if(sameLink(first, second))
                    {
                        d_engineGraph[*outEdgeIt].theBuffer = *it;
                        d_engineGraph[*outEdgeIt].theBuffer->setLinkDescription( second );
                    }
                }
            }
        }
    }

    void EngineManager::startRadio()
    {
		//Start all the controllers
		d_controllerManager.startControllers();

        //Start all the engines, one by one
        for( ptr_vector<EngineInterface>::iterator i = d_engines.begin(); i != d_engines.end(); ++i)
        {
            i->startEngine();
        }
    }

    void EngineManager::stopRadio()
    {
		//Stop all the controllers
		d_controllerManager.stopControllers();

        //Stop all the engines, one by one
        for( ptr_vector<EngineInterface>::iterator i = d_engines.begin(); i != d_engines.end(); ++i)
        {
            i->stopEngine();
        }
    }

    void EngineManager::unloadRadio()
    {
		//Unload all controllers
		d_controllerManager.unloadControllers();

        //Unload all the engines and remove them from the vector
        for( ptr_vector<EngineInterface>::iterator i = d_engines.begin(); i != d_engines.end(); ++i)
        {
            i->unloadEngine();
        }
        d_engines.clear();  //Automatic deletion of pointers due to boost::ptr_vector
    }

    RadioRepresentation& EngineManager::getCurrentRadio()
    {
        return d_radioRep;
    }

    void EngineManager::reconfigureRadio(ReconfigSet reconfigs)
    {
        //Separate out reconfigurations for each engine
        boost::ptr_vector<EngineInterface>::iterator engIt;
        for(engIt = d_engines.begin(); engIt != d_engines.end(); ++engIt)
        {
            string current = engIt->getName();
            ReconfigSet currentReconfigs;

            //Find parametric reconfigs for this engine
            vector<ParametricReconfig>::iterator parmIt;
            for(parmIt = reconfigs.d_paramReconfigs.begin(); parmIt != reconfigs.d_paramReconfigs.end(); ++parmIt)
            {
                if(parmIt->engineName == current)
                    currentReconfigs.d_paramReconfigs.push_back(*parmIt);
            }
            //Find structural reconfigs for this engine
            vector<StructuralReconfig>::iterator structIt;
            for(structIt = reconfigs.d_structReconfigs.begin(); structIt != reconfigs.d_structReconfigs.end(); ++structIt)
            {
                if(structIt->engineName == current)
                    currentReconfigs.d_structReconfigs.push_back(*structIt);
            }

			//Apply reconfigurations to the relevent engine if non-empty
			if(!(currentReconfigs.d_paramReconfigs.empty() && currentReconfigs.d_structReconfigs.empty()))
				engIt->addReconfiguration(currentReconfigs);

			//Apply reconfigurations to the radio representation
			d_radioRep.reconfigureRepresentation(currentReconfigs);
        }
    }

    void EngineManager::postCommand(Command command)
	{
		//Post the command to the relevant engine
		boost::ptr_vector<EngineInterface>::iterator engIt;
		for(engIt = d_engines.begin(); engIt != d_engines.end(); ++engIt)
		{
			if(command.engineName == engIt->getName())
			{
				engIt->postCommand(command);
			}
		}
	}

    std::string EngineManager::getParameterValue(std::string paramName, std::string componentName)
    {
    	return d_radioRep.getParameterValue(paramName, componentName);
    }

	void EngineManager::activateEvent(Event &e)
	{
		//Pass the event to the controller manager
		d_controllerManager.activateEvent(e);
	}

    EngineInterface* EngineManager::createEngine(EngineDescription& d) throw (ResourceNotFoundException)
    {
        EngineInterface* current = NULL;

        if(d.type == "pnengine")
        {
            current = new PNEngine(d.name, d_reps.pnRepository); 
        }
		else if(d.type == "stackengine")
        {
            current = new StackEngine(d.name, d_reps.stackRepository); 
        }
        else
        {
            throw ResourceNotFoundException("Engine type \"" + d.type + "\" does not exist.");
        }

		// Give the engine an interface to this EngineManager
		current->setEngineManager(this);
        return current;
    }

    //Check to see if the source component and port match
    bool EngineManager::sameLink(LinkDescription first, LinkDescription second)
    {
        return (first.sourceComponent == second.sourceComponent &&
            first.sourcePort == second.sourcePort);
    }

} /* namespace iris */
