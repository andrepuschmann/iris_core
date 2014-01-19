/**
 * \file EngineManager.cpp
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
 * Implementation of EngineManager class - manages all engines running
 * in Iris.
 */

#include "iris/EngineManager.h"
#include "iris/PhyEngine.h"
#include "iris/StackEngine.h"

using namespace std;
namespace b = boost;

namespace iris
{

    EngineManager::EngineManager()
    {
        controllerManager_.setCallbackInterface(this);
    }

    void EngineManager::loadRadio(RadioRepresentation rad)
    {
        //Set the current radio representation
        radioRep_ = rad;

        //Set the controller repositories in the ControllerManager
        controllerManager_.addRepository(reps_.contRepository);

        //Load the controllers
        vector<ControllerDescription> conts = rad.getControllers();
        vector<ControllerDescription>::iterator contIt;
        for(contIt=conts.begin(); contIt!=conts.end(); ++contIt)
        {
            controllerManager_.loadController(*contIt);
        }

        engineGraph_ = rad.getEngineGraph();

        //Create the engines
        EngVertexIterator i, iend;
        for(b::tie(i,iend) = vertices(engineGraph_); i != iend; ++i)
        {
            //Get the engine description
            EngineDescription current = engineGraph_[*i];

            //Add the engine to our vector
            engines_.push_back(createEngine(current));
        }

        //Do a topological sort of the graph
        deque<unsigned> topoOrder;
        topological_sort(engineGraph_, front_inserter(topoOrder), b::vertex_index_map(b::identity_property_map()));

        //Go through graph in topological order and set buffers
        for(deque<unsigned>::iterator i = topoOrder.begin(); i != topoOrder.end(); ++i)
        {
            //Get input buffers
            vector< b::shared_ptr< DataBufferBase > > inputBuffers, outputBuffers;
            EngInEdgeIterator edgeIt, edgeItEnd;
            for(b::tie(edgeIt, edgeItEnd) = in_edges(*i, engineGraph_); edgeIt != edgeItEnd; ++edgeIt)
            {
                inputBuffers.push_back(engineGraph_[*edgeIt].theBuffer);
            }

            //Load the engine, passing in the input buffers
            outputBuffers = engines_[*i].loadEngine(engineGraph_[*i], inputBuffers);

            //Set the ouput buffers in the graph edges
            EngOutEdgeIterator outEdgeIt, outEdgeItEnd;
            for(b::tie(outEdgeIt, outEdgeItEnd) = out_edges(*i, engineGraph_); outEdgeIt != outEdgeItEnd; ++outEdgeIt)
            {
                for(vector< b::shared_ptr< DataBufferBase > >::iterator it = outputBuffers.begin(); it != outputBuffers.end(); ++it)
                {
                    LinkDescription first = (*it)->getLinkDescription();
                    LinkDescription second = engineGraph_[*outEdgeIt];
                    if(sameLink(first, second))
                    {
                        engineGraph_[*outEdgeIt].theBuffer = *it;
                        engineGraph_[*outEdgeIt].theBuffer->setLinkDescription( second );
                    }
                }
            }
        }
    }

    void EngineManager::startRadio()
    {
        //Start all the controllers
        controllerManager_.startControllers();

        //Start all the engines, one by one
        for( b::ptr_vector<EngineInterface>::iterator i = engines_.begin(); i != engines_.end(); ++i)
        {
            i->startEngine();
        }
    }

    void EngineManager::stopRadio()
    {
        //Stop all the controllers
        controllerManager_.stopControllers();

        //Stop all the engines, one by one
        for( b::ptr_vector<EngineInterface>::iterator i = engines_.begin(); i != engines_.end(); ++i)
        {
            i->stopEngine();
        }
    }

    void EngineManager::unloadRadio()
    {
        //Unload all controllers
        controllerManager_.unloadControllers();

        //Unload all the engines and remove them from the vector
        for( b::ptr_vector<EngineInterface>::iterator i = engines_.begin(); i != engines_.end(); ++i)
        {
            i->unloadEngine();
        }
        engines_.clear();  //Automatic deletion of pointers due to boost::ptr_vector
    }

    RadioRepresentation& EngineManager::getCurrentRadio()
    {
        return radioRep_;
    }

    void EngineManager::reconfigureRadio(ReconfigSet reconfigs)
    {
        //Separate out reconfigurations for each engine
        b::ptr_vector<EngineInterface>::iterator engIt;
        for(engIt = engines_.begin(); engIt != engines_.end(); ++engIt)
        {
            string current = engIt->getName();
            ReconfigSet currentReconfigs;

            //Find parametric reconfigs for this engine
            vector<ParametricReconfig>::iterator parmIt;
            for(parmIt = reconfigs.paramReconfigs.begin(); parmIt != reconfigs.paramReconfigs.end(); ++parmIt)
            {
                if(parmIt->engineName == current)
                    currentReconfigs.paramReconfigs.push_back(*parmIt);
            }
            //Find structural reconfigs for this engine
            vector<StructuralReconfig>::iterator structIt;
            for(structIt = reconfigs.structReconfigs.begin(); structIt != reconfigs.structReconfigs.end(); ++structIt)
            {
                if(structIt->engineName == current)
                    currentReconfigs.structReconfigs.push_back(*structIt);
            }

            //Apply reconfigurations to the relevent engine if non-empty
            if(!(currentReconfigs.paramReconfigs.empty() && currentReconfigs.structReconfigs.empty()))
                engIt->addReconfiguration(currentReconfigs);

            //Apply reconfigurations to the radio representation
            radioRep_.reconfigureRepresentation(currentReconfigs);
        }
    }

    void EngineManager::postCommand(Command command)
    {
        //Post the command to the relevant engine
        b::ptr_vector<EngineInterface>::iterator engIt;
        for(engIt = engines_.begin(); engIt != engines_.end(); ++engIt)
        {
            if(command.engineName == engIt->getName())
            {
                engIt->postCommand(command);
            }
        }
    }

    std::string EngineManager::getParameterValue(std::string paramName, std::string componentName)
    {
        return radioRep_.getParameterValue(paramName, componentName);
    }

    std::string EngineManager::getParameterName(std::string componentName, int paramIndex, std::string &paramValue)
    {
        return radioRep_.getParameterName(componentName, paramIndex, paramValue);
    }

    std::string EngineManager::getEngineName(std::string componentName, int *engineIndex, int *compIndex)
    {
        return radioRep_.getEngineName(componentName, engineIndex, compIndex);
    }

    int EngineManager::getNrEngines()
    {
        return radioRep_.getNrEngines();
    }

    int EngineManager::getNrComponents()
    {
        return radioRep_.getNrComponents();
    }

    std::string EngineManager::getEngineNameFromIndex(int index)
    {
        return radioRep_.getEngineNameFromIndex(index);
    }

    std::string EngineManager::getComponentName(int index)
    {
        return radioRep_.getComponentName(index);
    }

    int  EngineManager::getNrParameters(std::string componentName)
    {
        return radioRep_.getNrParameters(componentName);
    }

    void EngineManager::activateEvent(Event &e)
    {
        //Pass the event to the controller manager
        controllerManager_.activateEvent(e);
    }

    EngineInterface* EngineManager::createEngine(EngineDescription& d)
    {
        EngineInterface* current = NULL;

        if(d.type == "phyengine")
        {
            current = new PhyEngine(d.name, reps_.phyRepository);
        }
        else if(d.type == "stackengine")
        {
            current = new StackEngine(d.name, reps_.stackRepository);
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
