/**
 * \file StackEngine.cpp
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
 * Implementation of StackEngine class - network stack engine for Iris.
 */

#include "iris/StackEngine.h"

#include "irisapi/StackComponent.h"
#include "StackInTranslator.h"
#include "StackOutTranslator.h"
#include "StackComponentManager.h"

using namespace std;
namespace b = boost;

namespace iris
{

    StackEngine::StackEngine(std::string name, std::string repository)
        :engineName_(name)
        ,engineManager_(NULL)
    {
        compManager_.reset(new StackComponentManager());
        compManager_->addRepository(repository);
    }

    StackEngine::~StackEngine()
    {
    }

    string StackEngine::getName() const
    {
        return engineName_;
    }

    void StackEngine::setEngineManager(EngineCallbackInterface *e)
    {
        engineManager_ = e;
    }

    std::vector< boost::shared_ptr< DataBufferBase > >
    StackEngine::loadEngine(EngineDescription eng,
                            std::vector< boost::shared_ptr< DataBufferBase > > inputLinks)
    {
        //Set the external input buffer
        engInputBuffers_ = inputLinks;

        //Set the engineGraph
        engineGraph_ = eng.engineGraph;

        //Check the graph
        checkGraph(engineGraph_);

        //Build the engine
        buildEngineGraph(eng);

        return engOutputBuffers_;
    }

    void StackEngine::unloadEngine()
    {
        //Destroy all InTranslators
        inTranslators_.clear();   
        //Destory all OutTranslators
        outTranslators_.clear();
        //Destroy all components and clear the vector
        components_.clear();   //Components are deleted here using a custom deallocator due to use of boost::shared_ptr
    }

    void StackEngine::startEngine()
    {
        //Start all the InTranslators
        for( vector< b::shared_ptr<StackInTranslator> >::iterator i = inTranslators_.begin(); i != inTranslators_.end(); ++i)
        {
            (*i)->startTranslator();    //Start the translator thread
        }

        //Start all the OutTranslators
        for( vector< b::shared_ptr<StackOutTranslator> >::iterator i = outTranslators_.begin(); i != outTranslators_.end(); ++i)
        {
            (*i)->startTranslator();    //Start the translator thread
        }

        //Start all the StackComponents
        for( vector< b::shared_ptr<StackComponent> >::iterator i = components_.begin(); i != components_.end(); ++i)
        {
            (*i)->initialize(); //Initialize the component
            (*i)->start();    //Call start() on the component implementation
            (*i)->startComponent();    //Start the component thread
        }
    }

    void StackEngine::stopEngine()
    {
        //Stop all the InTranslators
        for( vector< b::shared_ptr<StackInTranslator> >::iterator i = inTranslators_.begin(); i != inTranslators_.end(); ++i)
        {
            (*i)->stopTranslator();    //Stop the translator thread
        }

        //Stop all the OutTranslators
        for( vector< b::shared_ptr<StackOutTranslator> >::iterator i = outTranslators_.begin(); i != outTranslators_.end(); ++i)
        {
            (*i)->stopTranslator();    //Start the translator thread
        }

        //Stop all the StackComponents
        for( vector< b::shared_ptr<StackComponent> >::iterator i = components_.begin(); i != components_.end(); ++i)
        {
            (*i)->stop();    //Call stop() on the component implementation
            (*i)->stopComponent();    //Stop the component thread
        }
    }

    void StackEngine::addReconfiguration(ReconfigSet reconfigs)
    {
        //Go through parametric reconfigurations and give them to the relevent components
        vector< ParametricReconfig >::iterator paramIt;
        for(paramIt = reconfigs.paramReconfigs.begin();
            paramIt != reconfigs.paramReconfigs.end();
            ++paramIt)
        {
            reconfigureParameter(*paramIt);
        }
    }

    void StackEngine::postCommand(Command command)
    {
        bool bFound = false;

        //Find component and post command
        vector< b::shared_ptr<StackComponent> >::iterator compIt;
        for(compIt = components_.begin(); compIt != components_.end(); ++compIt)
        {
            if((*compIt)->getName() == command.componentName)
            {
                (*compIt)->postCommand(command);
                bFound = true;
            }
        }

        if(!bFound)
        {
            LOG(LERROR) << "Posting command " << command.commandName << " failed: could not find component: " << command.componentName;
        }
    }

    void StackEngine::checkGraph(RadioGraph& graph)
    {
        //Check graph obeys StackEngine rules:
    }

    void StackEngine::buildEngineGraph(EngineDescription& eng)
    {
        //Create the components
        for(vector<ComponentDescription>::iterator i = eng.components.begin(); 
            i != eng.components.end(); i++)
        {
            b::shared_ptr<StackComponent> comp = compManager_->loadComponent(*i);
            comp->setEngine(this);    //Provide an interface to the component
            components_.push_back(comp);
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
                b::shared_ptr<StackComponent> above = findComponent(i->sourceComponent);
                b::shared_ptr<StackComponent> below = findComponent(i->sinkComponent);
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

                above->addBufferBelow(i->sourcePort, i->sinkPort, below->getBufferForAbove());
                below->addBufferAbove(i->sinkPort, i->sourcePort, above->getBufferForBelow());
            }
        }
    }

    void StackEngine::reconfigureParameter(ParametricReconfig reconfig)
    {
        bool bFound = false;

        //Find component
        vector< b::shared_ptr<StackComponent> >::iterator compIt;
        for(compIt = components_.begin(); compIt != components_.end(); ++compIt)
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
        if(engineManager_ == NULL)
        {
            LOG(LERROR) << "Failed to activate event: Engine " << engineName_ << " could not access EngineManager";
            return;
        }

        //Simply pass the event on to the EngineManager
        engineManager_->activateEvent(e);
    }

    void StackEngine::createExternalLink(LinkDescription& l)
    {
        if(l.sinkEngine == getName())
        {
            //Incoming link

            //Find the StackComponent that receives the link
            b::shared_ptr<StackComponent> comp = findComponent(l.sinkComponent);
            if(comp == NULL)
            {
                throw ResourceNotFoundException("Could not find StackComponent " + l.sinkComponent +
                        " specified in link."); 
            }
            //Find the DataBuffer for the link
            b::shared_ptr< DataBufferBase > buf;
            for( vector< b::shared_ptr< DataBufferBase > >::iterator i = engInputBuffers_.begin(); i != engInputBuffers_.end(); ++i)
            {
                if(sameLink(l, (*i)->getLinkDescription()))
                {
                    buf = *i;
                }
            }

            //Create a translator and link up
            b::shared_ptr< StackInTranslator > t(new StackInTranslator());
            t->setInputBuffer(dynamic_cast<ReadBufferBase*>(buf.get()));
            t->setBufferAbove(comp->getBufferForBelow());
            inTranslators_.push_back(t);
        }
        else
        {
            //Outgoing link

            //Find the StackComponent that provides the link
            b::shared_ptr<StackComponent> comp = findComponent(l.sourceComponent);
            if(comp == NULL)
            {
                throw ResourceNotFoundException("Could not find StackComponent " + l.sourceComponent +
                        " specified in link."); 
            }

            //Create a DataBuffer for the link
            b::shared_ptr< DataBufferBase > buf(new DataBuffer<uint8_t>);
            buf->setLinkDescription(l);
            engOutputBuffers_.push_back(buf);

            //Create a translator and link up
            b::shared_ptr< StackOutTranslator > t(new StackOutTranslator());
            t->setOutputBuffer(dynamic_cast<WriteBufferBase*>(buf.get()));
            comp->addBufferBelow(l.sourcePort, l.sinkPort, t->getBuffer());
            outTranslators_.push_back(t);
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
        b::shared_ptr<StackComponent> comp;
        for(vector< b::shared_ptr<StackComponent> >::iterator i = components_.begin();
            i != components_.end(); i++)
        {
            if((*i)->getName() == name)
            {
                return *i;
            }
        }
        return comp;
    }

} /* namespace iris */
