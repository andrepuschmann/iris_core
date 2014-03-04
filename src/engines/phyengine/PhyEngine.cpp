/**
 * \file PhyEngine.cpp
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
 * Implementation of PhyEngine class - the Iris process network engine.
 */

#include <boost/graph/topological_sort.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/progress.hpp>
#include <boost/bind.hpp>

#include "iris/PhyEngine.h"

#include "irisapi/PhyComponent.h"
#include "PhyComponentManager.h"
#include "PhyDataBuffer.h"

using namespace std;
namespace b = boost;

namespace iris
{

    PhyEngine::PhyEngine(std::string name, std::string repository)
        :engineName_(name)
        ,engineManager_(NULL)
    {
        compManager_.reset(new PhyComponentManager());
        compManager_->addRepository(repository);
    }

    PhyEngine::~PhyEngine()
    {
    }

    string PhyEngine::getName() const
    {
        return engineName_;
    }

    void PhyEngine::setEngineManager(EngineCallbackInterface *e)
    {
        engineManager_ = e;
    }

    std::vector< boost::shared_ptr< DataBufferBase > >
    PhyEngine::loadEngine(EngineDescription eng,
                          std::vector< boost::shared_ptr< DataBufferBase > > inputLinks)
    {
        //Set the external input buffer
        engInputBuffers_ = inputLinks;

        //Set the engineGraph
        engineGraph_ = eng.engineGraph;

        //Check the graph
        checkGraph(engineGraph_);

        //Build the engine
        buildEngineGraph(engineGraph_);

        return engOutputBuffers_;
    }

    void PhyEngine::unloadEngine()
    {
        //Destroy all components and clear the vector
        components_.clear();   //Components are deleted here using a custom deallocator due to use of boost::shared_ptr

        //Destroy all internal buffers and clear the vector
        internalBuffers_.clear();
    }

    void PhyEngine::startEngine()
    {
        //Start any components that require it
        for( vector< b::shared_ptr<PhyComponent> >::iterator i = components_.begin(); i != components_.end(); ++i)
        {
            (*i)->start();
        }
        //Start the main engine thread
        thread_.reset( new b::thread( b::bind( &PhyEngine::threadLoop, this ) ) );
    }

    void PhyEngine::stopEngine()
    {
        //Stop any components that require it
        for( vector< b::shared_ptr<PhyComponent> >::iterator i = components_.begin(); i != components_.end(); ++i)
        {
            (*i)->stop();
        }
        thread_->interrupt();
        thread_->join();
    }

    void PhyEngine::addReconfiguration(ReconfigSet reconfigs)
    {
        reconfigQueue_.push(reconfigs);
    }

    void PhyEngine::threadLoop()
    {
        //Do a topological sort of the graph (sorts in reverse topological order)
        vector<unsigned> revTopoOrder;
        topological_sort(engineGraph_, back_inserter(revTopoOrder), b::vertex_index_map(b::identity_property_map()));

        //The main loop of this engine thread
        try{
            while(true)
            {
                b::this_thread::interruption_point();

                //Check message queue for ReconfigSets
                ReconfigSet currentReconfigSet;
                while(reconfigQueue_.tryPop(currentReconfigSet))
                {
                    vector< ParametricReconfig >::iterator paramIt;
                    for(paramIt = currentReconfigSet.paramReconfigs.begin();
                        paramIt != currentReconfigSet.paramReconfigs.end();
                        ++paramIt)
                    {
                        reconfigureParameter(*paramIt);
                    }
                }

                //Go through components in topological order
                for(vector<unsigned>::reverse_iterator i = revTopoOrder.rbegin(); i != revTopoOrder.rend(); ++i)
                {
                    if(i == revTopoOrder.rbegin()) //First component in the graph
                    {
                        components_[*i]->doProcess();
                    }
                    else
                    {
                        InEdgeIterator edgeIt, edgeItEnd;
                        for(b::tie(edgeIt, edgeItEnd) = in_edges(*i, engineGraph_); edgeIt != edgeItEnd; ++edgeIt)
                        {
                            //If there's data available in an input buffer, process it
                            while( engineGraph_[*edgeIt].theBuffer->hasData() )
                            {
                                components_[*i]->doProcess();
                            }
                        }
                    }
                }
            }
        }
        catch(IrisException& ex)
        {
            LOG(LFATAL) << "Error in engine " << engineName_ << ": " << ex.what() << " - Engine thread exiting.";
        }
        catch(b::thread_interrupted&)
        {
            LOG(LINFO) << "Thread in Engine " << engineName_ << " interrupted";
        }
    }

    void PhyEngine::checkGraph(RadioGraph& graph)
    {
        //Check graph obeys PhyEngine rules:
        //  * Only one source component - can have multiple inputs
        //  * All other components have only one input
    }

    void PhyEngine::buildEngineGraph(RadioGraph& graph)
    {
        //Create the components
        VertexIterator i, iend;
        for(b::tie(i,iend) = vertices(graph); i != iend; ++i)
        {
            //Load the component and add to vector
            ComponentDescription current = graph[*i];
            b::shared_ptr<PhyComponent> comp = compManager_->loadComponent(current);
            comp->setEngine(this);    //Provide an interface to the component
            components_.push_back(comp);
        }

        //Do a topological sort of the graph
        deque<unsigned> topoOrder;
        topological_sort(graph, front_inserter(topoOrder), b::vertex_index_map(b::identity_property_map()));

        //Set up some containers
        vector< ReadBufferBase* > currentInBufs;
        vector< WriteBufferBase* > currentOutBufs;
        map<string, int> inputTypes, outputTypes;

        //The external input buffers feed the source component of the graph
        for( vector< b::shared_ptr< DataBufferBase > >::iterator i = engInputBuffers_.begin(); i != engInputBuffers_.end(); ++i)
        {
            DataBufferBase* buf = (*i).get();
            LinkDescription desc = buf->getLinkDescription();
            inputTypes[desc.sinkPort] = buf->getTypeIdentifier();
            currentInBufs.push_back(dynamic_cast<ReadBufferBase*>(buf));
        }

        //Set buffers on the components
        for(deque<unsigned>::iterator i = topoOrder.begin(); i != topoOrder.end(); ++i)
        {
            //Get the internal input buffer details
            InEdgeIterator edgeIt, edgeItEnd;
            for(b::tie(edgeIt, edgeItEnd) = in_edges(*i, graph); edgeIt != edgeItEnd; ++edgeIt)
            {
                inputTypes[graph[*edgeIt].sinkPort] =  graph[*edgeIt].theBuffer->getTypeIdentifier();
                currentInBufs.push_back(dynamic_cast<ReadBufferBase*>(graph[*edgeIt].theBuffer.get()));
            }

            //TODO: Check that the input port names from the xml match existing input ports
            //TODO: Check that inputs exist for each of the registered input port names

            //Get output buffer types from component
            components_[*i]->calculateOutputTypes(inputTypes, outputTypes);

            // temporary shell for testing template components
            std::vector<int> inTypes, outTypes;
            for (std::map<std::string, int>::iterator j = inputTypes.begin(); j != inputTypes.end(); ++j)
            {
                inTypes.push_back(j->second);
            }
            for (std::map<std::string, int>::iterator j = outputTypes.begin(); j != outputTypes.end(); ++j)
            {
                outTypes.push_back(j->second);
            }

            PhyComponent* x = components_[*i]->setupIO(inTypes, outTypes);
            if (x != components_[*i].get())
            {
                components_[*i].reset(x);
            }

            //Create internal output buffers and add to graph edges
            OutEdgeIterator outEdgeIt, outEdgeItEnd;
            for(b::tie(outEdgeIt, outEdgeItEnd) = out_edges(*i, graph); outEdgeIt != outEdgeItEnd; ++outEdgeIt)
            {
                //Check that the port name exists
                string srcPort = graph[*outEdgeIt].sourcePort;
                if(outputTypes.find(srcPort) == outputTypes.end())
                {
                    throw ResourceNotFoundException("Output port " + srcPort + \
                        " could not be found on PhyComponent " + components_[*i]->getName());
                }

                //Create a PhyDataBuffer of the correct type
                int currentType = outputTypes[srcPort];
                b::shared_ptr< DataBufferBase > buf = createPhyDataBuffer(currentType);
                graph[*outEdgeIt].theBuffer = buf;
                graph[*outEdgeIt].theBuffer->setLinkDescription(graph[*outEdgeIt]);
                internalBuffers_.push_back( buf );

                currentOutBufs.push_back( dynamic_cast<WriteBufferBase*>( buf.get() ) );

                //Remove from the outputTypes map
                outputTypes.erase( outputTypes.find( srcPort ) );
            }

            //Anything left in the outputTypes map must be an external output buffer
            for( map<string, int>::iterator j = outputTypes.begin(); j != outputTypes.end(); ++j)
            {
                //Create a DataBuffer and add to exOutputBuffers
                b::shared_ptr< DataBufferBase > buf = createDataBuffer( j->second );
                LinkDescription l;
                l.sourceEngine = engineName_;
                l.sourceComponent = components_[*i]->getName();
                l.sourcePort = j->first;
                buf->setLinkDescription(l);
                engOutputBuffers_.push_back(buf);
                currentOutBufs.push_back( dynamic_cast<WriteBufferBase*>( buf.get() ) );
            }

            //Set the buffers in the component
            components_[*i]->setBuffers(currentInBufs, currentOutBufs);

            //Initialize the component
            components_[*i]->initialize();

            currentInBufs.clear();
            currentOutBufs.clear();
        }

    }

    void PhyEngine::reconfigureParameter(ParametricReconfig reconfig)
    {
        bool bFound = false;

        //Find component
        vector< b::shared_ptr<PhyComponent> >::iterator compIt;
        for(compIt = components_.begin(); compIt != components_.end(); ++compIt)
        {
            if((*compIt)->getName() == reconfig.componentName)
            {
                (*compIt)->setValue(reconfig.parameterName, reconfig.parameterValue);
                (*compIt)->parameterHasChanged(reconfig.parameterName);
                LOG(LDEBUG) << "Reconfigured parameter " << reconfig.parameterName << " : " << reconfig.parameterValue;
                bFound = true;
            }
        }

        if(!bFound)
        {
            LOG(LERROR) << "Parametric reconfiguration failed: could not find component: " << reconfig.componentName;
        }
    }

    void PhyEngine::postCommand(Command command)
    {
        bool bFound = false;

        //Find component and post command
        vector< b::shared_ptr<PhyComponent> >::iterator compIt;
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
            LOG(LERROR) << "Posting command failed: could not find component: " << command.componentName;
        }

    }

    void PhyEngine::activateEvent(Event &e)
    {
        if(engineManager_ == NULL)
        {
            LOG(LERROR) << "Failed to activate event: Engine " << engineName_ << " could not access EngineManager";
            return;
        }

        //Simply pass the event on to the EngineManager
        engineManager_->activateEvent(e);
    }

    bool PhyEngine::sameLink(LinkDescription first, LinkDescription second) const
    {
        return (first.sourceComponent == second.sourceComponent &&
            first.sinkComponent == second.sinkComponent &&
            first.sourcePort == second.sourcePort &&
            first.sinkPort == second.sinkPort);
    }

    // Internal namespace for metaprogramming loop 
    namespace internal{
    //! Struct for metaprogramming loop to go through all iris types and create a DataBuffer of the correct type
    template <int N = b::mpl::size<IrisDataTypes>::value>
    struct getBufferOfType
    {
        static bool EXEC(int type, b::shared_ptr<DataBufferBase> &ptr)
        {
            typedef typename b::mpl::at_c<IrisDataTypes,N-1>::type T;

            if(N-1 == type)
            {
                ptr.reset(new DataBuffer<T>);
                return true;
            }
            else
            {
                return getBufferOfType<N-1>::EXEC(type, ptr);
            }
        }
    };
    //! Template specialization to end the recursion
    template <>
    struct getBufferOfType<0>
    {
        static bool EXEC(int type, b::shared_ptr<DataBufferBase> &ptr)
        {
            return false;
        }
    };

    //! Struct for metaprogramming loop to go through all iris types and create a PhyDataBuffer of the correct type
    template <int N = b::mpl::size<IrisDataTypes>::value>
    struct getPhyBufferOfType
    {
        static bool EXEC(int type, b::shared_ptr<DataBufferBase> &ptr)
        {
            typedef typename b::mpl::at_c<IrisDataTypes,N-1>::type T;

            if(N-1 == type)
            {
                ptr.reset(new PhyDataBuffer<T>);
                return true;
            }
            else
            {
                return getPhyBufferOfType<N-1>::EXEC(type, ptr);
            }
        }
    };
    //! Template specialization to end the recursion
    template <>
    struct getPhyBufferOfType<0>
    {
        static bool EXEC(int type, b::shared_ptr<DataBufferBase> &ptr)
        {
            return false;
        }
    };
    } /* namespace internal */

    //! Create a DataBuffer of a particular data type
    b::shared_ptr< DataBufferBase > PhyEngine::createDataBuffer(int type) const
    {
        b::shared_ptr< DataBufferBase> ret;
        if(!internal::getBufferOfType<>::EXEC(type, ret))
        {
            throw InvalidDataTypeException("Attempted to create DataBuffer with invalid data type value: " + type);
        }
        return ret;
    }

    //! Create a PhyDataBuffer of a particular data type
    b::shared_ptr< DataBufferBase > PhyEngine::createPhyDataBuffer(int type) const
    {
        b::shared_ptr< DataBufferBase> ret;
        if(!internal::getPhyBufferOfType<>::EXEC(type, ret))
        {
            throw InvalidDataTypeException("Attempted to create DataBuffer with invalid data type value: " + type);
        }
        return ret;
    }

} /* namespace iris */
