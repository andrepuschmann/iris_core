/**
 * \file StackEngine.cpp
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
 * Implementation of StackEngine class - network stack engine for Iris.
 */

#include "iris/StackEngine.h"

#include "irisapi/StackComponent.h"
#include "StackInTranslator.h"
#include "StackOutTranslator.h"
#include "StackComponentManager.h"

using namespace std;
using namespace boost;

namespace iris
{

    StackEngine::StackEngine(std::string name, std::string repository) throw (IrisException)
        :d_engineName(name)
        ,d_engineManager(NULL)
    {
        d_compManager.reset(new StackComponentManager());
        d_compManager->addRepository(repository);
    }

    StackEngine::~StackEngine()
    {
    }

    string StackEngine::getName() const
    {
        return d_engineName;
    }

    void StackEngine::setEngineManager(EngineCallbackInterface *e)
    {
        d_engineManager = e;
    }

    std::vector< boost::shared_ptr< DataBufferBase > > StackEngine::loadEngine(EngineDescription eng, std::vector< boost::shared_ptr< DataBufferBase > > inputLinks)
        throw (IrisException)
    {
        //Set the external input buffer
        d_engInputBuffers = inputLinks;

        //Set the engineGraph
        d_engineGraph = eng.engineGraph;

        //Check the graph
        checkGraph(d_engineGraph);

        //Build the engine
        buildEngineGraph(eng);

        return d_engOutputBuffers;
    }

    void StackEngine::unloadEngine()
    {
        //Destroy all InTranslators
        d_inTranslators.clear();   
        //Destory all OutTranslators
        d_outTranslators.clear();
        //Destroy all components and clear the vector
        d_components.clear();   //Components are deleted here using a custom deallocator due to use of boost::shared_ptr
    }

    void StackEngine::startEngine()
    {
        //Start all the InTranslators
        for( vector< shared_ptr<StackInTranslator> >::iterator i = d_inTranslators.begin(); i != d_inTranslators.end(); ++i)
        {
            (*i)->startTranslator();    //Start the translator thread
        }

        //Start all the OutTranslators
        for( vector< shared_ptr<StackOutTranslator> >::iterator i = d_outTranslators.begin(); i != d_outTranslators.end(); ++i)
        {
            (*i)->startTranslator();    //Start the translator thread
        }

        //Start all the StackComponents
        for( vector< shared_ptr<StackComponent> >::iterator i = d_components.begin(); i != d_components.end(); ++i)
        {
            (*i)->initialize(); //Initialize the component
            (*i)->start();    //Call start() on the component implementation
            (*i)->startComponent();    //Start the component thread
        }
    }

    void StackEngine::stopEngine()
    {
        //Stop all the InTranslators
        for( vector< shared_ptr<StackInTranslator> >::iterator i = d_inTranslators.begin(); i != d_inTranslators.end(); ++i)
        {
            (*i)->stopTranslator();    //Stop the translator thread
        }

        //Stop all the OutTranslators
        for( vector< shared_ptr<StackOutTranslator> >::iterator i = d_outTranslators.begin(); i != d_outTranslators.end(); ++i)
        {
            (*i)->stopTranslator();    //Start the translator thread
        }

        //Stop all the StackComponents
        for( vector< shared_ptr<StackComponent> >::iterator i = d_components.begin(); i != d_components.end(); ++i)
        {
            (*i)->stop();    //Call stop() on the component implementation
            (*i)->stopComponent();    //Stop the component thread
        }
    }

    void StackEngine::addReconfiguration(ReconfigSet reconfigs)
    {
        //Go through parametric reconfigurations and give them to the relevent components
        vector< ParametricReconfig >::iterator paramIt;
        for(paramIt = reconfigs.d_paramReconfigs.begin();
            paramIt != reconfigs.d_paramReconfigs.end();
            ++paramIt)
        {
            reconfigureParameter(*paramIt);
        }
    }

    void StackEngine::postCommand(Command command)
    {
        bool bFound = false;

        //Find component and post command
        vector< shared_ptr<StackComponent> >::iterator compIt;
        for(compIt = d_components.begin(); compIt != d_components.end(); ++compIt)
        {
            if((*compIt)->getName() == command.componentName)
            {
                (*compIt)->postCommand(command);
                bFound = true;
            }
        }

        if(!bFound)
        {
            LOG(LERROR) << "Parametric reconfiguration failed: could not find component: " << command.componentName;
        }
    }

    void StackEngine::checkGraph(RadioGraph& graph)
    {
        //Check graph obeys StackEngine rules:
    }

    void StackEngine::buildEngineGraph(EngineDescription& eng)
        throw (IrisException)
    {
        //Create the components
        for(vector<ComponentDescription>::iterator i = eng.components.begin(); 
            i != eng.components.end(); i++)
        {
            shared_ptr<StackComponent> comp = d_compManager->loadComponent(*i);
            comp->setEngine(this);    //Provide an interface to the component
            d_components.push_back(comp);
        }

         //Create the links
        for(vector<LinkDescription>::iterator i = eng.links.begin(); 
            i != eng.links.end(); i++)
        {
            if(i->sinkEngine != i->sourceEngine)
            {
                //External link
                createExternalLink(*i);
            }
            else
            {
                //Internal link
                shared_ptr<StackComponent> above = findComponent(i->sourceComponent);
                shared_ptr<StackComponent> below = findComponent(i->sinkComponent);
                if(above == NULL)
                {
                    throw ResourceNotFoundException("Could not find StackComponent " + i->sourceComponent +
                        " specified in link."); 
                }
                if(below == NULL)
                {
                    throw ResourceNotFoundException("Could not find StackComponent " + i->sinkComponent +
                        " specified in link."); 
                }

                above->addBufferBelow(i->sourcePort, below->getBuffer());
                below->addBufferAbove(i->sinkPort, above->getBuffer());
            }
        }
    }

    void StackEngine::reconfigureParameter(ParametricReconfig reconfig)
    {
        bool bFound = false;

        //Find component
        vector< shared_ptr<StackComponent> >::iterator compIt;
        for(compIt = d_components.begin(); compIt != d_components.end(); ++compIt)
        {
            if((*compIt)->getName() == reconfig.componentName)
            {
                (*compIt)->addReconfiguration(reconfig);
                bFound = true;
            }
        }

        if(!bFound)
        {
            LOG(LERROR) << "Parametric reconfiguration failed: could not find component: " << reconfig.componentName;
        }
    }

    void StackEngine::activateEvent(Event &e)
    {
        if(d_engineManager == NULL)
        {
            LOG(LERROR) << "Failed to activate event: Engine " << d_engineName << " could not access EngineManager";
            return;
        }

        //Simply pass the event on to the EngineManager
        d_engineManager->activateEvent(e);
    }

    void StackEngine::createExternalLink(LinkDescription& l)
    {
        if(l.sinkEngine == getName())
        {
            //Incoming link

            //Find the StackComponent that receives the link
            shared_ptr<StackComponent> comp = findComponent(l.sinkComponent);
            if(comp == NULL)
            {
                throw ResourceNotFoundException("Could not find StackComponent " + l.sinkComponent +
                        " specified in link."); 
            }
            //Find the DataBuffer for the link
            shared_ptr< DataBufferBase > buf;
            for( vector< shared_ptr< DataBufferBase > >::iterator i = d_engInputBuffers.begin(); i != d_engInputBuffers.end(); ++i)
            {
                if(sameLink(l, (*i)->getLinkDescription()))
                {
                    buf = *i;
                }
            }

            //Create a translator and link up
            shared_ptr< StackInTranslator > t(new StackInTranslator());
            t->setInputBuffer(dynamic_cast<ReadBufferBase*>(buf.get()));
            t->setBufferAbove(comp->getBuffer());
            d_inTranslators.push_back(t);
        }
        else
        {
            //Outgoing link

            //Find the StackComponent that provides the link
            shared_ptr<StackComponent> comp = findComponent(l.sourceComponent);
            if(comp == NULL)
            {
                throw ResourceNotFoundException("Could not find StackComponent " + l.sourceComponent +
                        " specified in link."); 
            }

            //Create a DataBuffer for the link
            shared_ptr< DataBufferBase > buf(new DataBuffer<uint8_t>);
            buf->setLinkDescription(l);
            d_engOutputBuffers.push_back(buf);

            //Create a translator and link up
            shared_ptr< StackOutTranslator > t(new StackOutTranslator());
            t->setOutputBuffer(dynamic_cast<WriteBufferBase*>(buf.get()));
            comp->addBufferBelow(l.sourcePort,t->getBuffer());
            d_outTranslators.push_back(t);
        }
    }

    bool StackEngine::sameLink(LinkDescription first, LinkDescription second) const
    {
        return (first.sourceComponent == second.sourceComponent &&
            first.sinkComponent == second.sinkComponent &&
            first.sourcePort == second.sourcePort &&
            first.sinkPort == second.sinkPort);
    }

    boost::shared_ptr< StackComponent > StackEngine::findComponent(std::string name)
    {
        boost::shared_ptr<StackComponent> comp;
        for(vector< shared_ptr<StackComponent> >::iterator i = d_components.begin();
            i != d_components.end(); i++)
        {
            if((*i)->getName() == name)
            {
                return *i;
            }
        }
        return comp;
    }

} /* namespace iris */
