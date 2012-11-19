/**
 * \file PNEngine.cpp
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
 * Implementation of PNEngine class - the Iris process network engine.
 */

#include <boost/graph/topological_sort.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/progress.hpp>
#include <boost/bind.hpp>

#include "iris/PNEngine.h"

#include "irisapi/PNComponent.h"
#include "PNComponentManager.h"
#include "PNDataBuffer.h"

using namespace std;
using namespace boost;

namespace iris
{

    PNEngine::PNEngine(std::string name, std::string repository) throw (IrisException)
        :d_engineName(name)
        ,d_engineManager(NULL)
    {
        d_compManager.reset(new PNComponentManager());
        d_compManager->addRepository(repository);
    }

    PNEngine::~PNEngine()
    {
    }

    string PNEngine::getName() const
    {
        return d_engineName;
    }

    void PNEngine::setEngineManager(EngineCallbackInterface *e)
    {
        d_engineManager = e;
    }

    std::vector< boost::shared_ptr< DataBufferBase > > PNEngine::loadEngine(EngineDescription eng, std::vector< boost::shared_ptr< DataBufferBase > > inputLinks)
        throw (IrisException)
    {
        //Set the external input buffer
        d_engInputBuffers = inputLinks;

        //Set the engineGraph
        d_engineGraph = eng.engineGraph;

        //Check the graph
        checkGraph(d_engineGraph);

        //Build the engine
        buildEngineGraph(d_engineGraph);

        return d_engOutputBuffers;
    }

    void PNEngine::unloadEngine()
    {
        //Destroy all components and clear the vector
        d_components.clear();   //Components are deleted here using a custom deallocator due to use of boost::shared_ptr

        //Destroy all internal buffers and clear the vector
        d_internalBuffers.clear();
    }

    void PNEngine::startEngine()
    {
        //Start any components that require it
        for( vector< shared_ptr<PNComponent> >::iterator i = d_components.begin(); i != d_components.end(); ++i)
        {
            (*i)->start();
        }
        //Start the main engine thread
        d_thread.reset( new thread( boost::bind( &PNEngine::threadLoop, this ) ) );
    }

    void PNEngine::stopEngine()
    {
        //Stop any components that require it
        for( vector< shared_ptr<PNComponent> >::iterator i = d_components.begin(); i != d_components.end(); ++i)
        {
            (*i)->stop();
        }
        d_thread->interrupt();
        d_thread->join();
    }

    void PNEngine::addReconfiguration(ReconfigSet reconfigs)
    {
        d_reconfigQueue.push(reconfigs);
    }

    void PNEngine::threadLoop()
    {
        //Do a topological sort of the graph (sorts in reverse topological order)
        vector<unsigned> revTopoOrder;
        topological_sort(d_engineGraph, back_inserter(revTopoOrder), vertex_index_map(identity_property_map()));

        //The main loop of this engine thread
        try{
            while(true)
            {
                this_thread::interruption_point();

                //Check message queue for ReconfigSets
                ReconfigSet currentReconfigSet;
                while(d_reconfigQueue.tryPop(currentReconfigSet))
                {
                    vector< ParametricReconfig >::iterator paramIt;
                    for(paramIt = currentReconfigSet.d_paramReconfigs.begin();
                        paramIt != currentReconfigSet.d_paramReconfigs.end();
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
                        d_components[*i]->doProcess();
                    }
                    else
                    {
                        InEdgeIterator edgeIt, edgeItEnd;
                        for(tie(edgeIt, edgeItEnd) = in_edges(*i, d_engineGraph); edgeIt != edgeItEnd; ++edgeIt)
                        {
                            //If there's data available in an input buffer, process it
                            while( d_engineGraph[*edgeIt].theBuffer->hasData() )
                            {
                                d_components[*i]->doProcess();
                            }
                        }
                    }
                }
            }
        }
        catch(IrisException& ex)
        {
            LOG(LFATAL) << "Error in engine " << d_engineName << ": " << ex.what() << " - Engine thread exiting.";
        }
        catch(thread_interrupted&)
        {
            LOG(LINFO) << "Thread in Engine " << d_engineName << " interrupted";
        }
    }

    void PNEngine::checkGraph(RadioGraph& graph)
    {
        //Check graph obeys PNEngine rules:
        //  * Only one source component - can have multiple inputs
        //  * All other components have only one input
    }

    void PNEngine::buildEngineGraph(RadioGraph& graph)
        throw (IrisException)
    {
        //Create the components
        VertexIterator i, iend;
        for(tie(i,iend) = vertices(graph); i != iend; ++i)
        {
            //Load the component and add to vector
            ComponentDescription current = graph[*i];
            shared_ptr<PNComponent> comp = d_compManager->loadComponent(current);
            comp->setEngine(this);    //Provide an interface to the component
            d_components.push_back(comp);
        }

        //Do a topological sort of the graph
        deque<unsigned> topoOrder;
        topological_sort(graph, front_inserter(topoOrder), vertex_index_map(identity_property_map()));

        //Set up some containers
        vector< ReadBufferBase* > currentInBufs;
        vector< WriteBufferBase* > currentOutBufs;
        map<string, int> inputTypes, outputTypes;

        //The external input buffers feed the source component of the graph
        for( vector< boost::shared_ptr< DataBufferBase > >::iterator i = d_engInputBuffers.begin(); i != d_engInputBuffers.end(); ++i)
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
            for(tie(edgeIt, edgeItEnd) = in_edges(*i, graph); edgeIt != edgeItEnd; ++edgeIt)
            {
                inputTypes[graph[*edgeIt].sinkPort] =  graph[*edgeIt].theBuffer->getTypeIdentifier();
                currentInBufs.push_back(dynamic_cast<ReadBufferBase*>(graph[*edgeIt].theBuffer.get()));
            }

            //TODO: Check that the input port names from the xml match existing input ports
            //TODO: Check that inputs exist for each of the registered input port names

            //Get output buffer types from component
            outputTypes = d_components[*i]->calculateOutputTypes(inputTypes);

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

            PNComponent* x = d_components[*i]->setupIO(inTypes, outTypes);
            if (x != d_components[*i].get())
            {
                d_components[*i].reset(x);
            }

            //Create internal output buffers and add to graph edges
            OutEdgeIterator outEdgeIt, outEdgeItEnd;
            for(tie(outEdgeIt, outEdgeItEnd) = out_edges(*i, graph); outEdgeIt != outEdgeItEnd; ++outEdgeIt)
            {
                //Check that the port name exists
                string srcPort = graph[*outEdgeIt].sourcePort;
                if(outputTypes.find(srcPort) == outputTypes.end())
                {
                    throw ResourceNotFoundException("Output port " + srcPort + \
                        " could not be found on PNComponent " + d_components[*i]->getName()); 
                }

                //Create a PNDataBuffer of the correct type
                int currentType = outputTypes[srcPort];
                shared_ptr< DataBufferBase > buf = createPNDataBuffer(currentType);
                graph[*outEdgeIt].theBuffer = buf;
                graph[*outEdgeIt].theBuffer->setLinkDescription(graph[*outEdgeIt]);
                d_internalBuffers.push_back( buf );

                currentOutBufs.push_back( dynamic_cast<WriteBufferBase*>( buf.get() ) );

                //Remove from the outputTypes map
                outputTypes.erase( outputTypes.find( srcPort ) );
            }

            //Anything left in the outputTypes map must be an external output buffer
            for( map<string, int>::iterator j = outputTypes.begin(); j != outputTypes.end(); ++j)
            {
                //Create a DataBuffer and add to exOutputBuffers
                shared_ptr< DataBufferBase > buf = createDataBuffer( j->second );
                LinkDescription l;
                l.sourceEngine = d_engineName;
                l.sourceComponent = d_components[*i]->getName();
                l.sourcePort = j->first;
                buf->setLinkDescription(l);
                d_engOutputBuffers.push_back(buf);
                currentOutBufs.push_back( dynamic_cast<WriteBufferBase*>( buf.get() ) );
            }

            //Set the buffers in the component
            d_components[*i]->setBuffers(currentInBufs, currentOutBufs);

            //Initialize the component
            d_components[*i]->initialize();

            currentInBufs.clear();
            currentOutBufs.clear();
        }

    }

    void PNEngine::reconfigureParameter(ParametricReconfig reconfig)
    {
        bool bFound = false;

        //Find component
        vector< shared_ptr<PNComponent> >::iterator compIt;
        for(compIt = d_components.begin(); compIt != d_components.end(); ++compIt)
        {
            if((*compIt)->getName() == reconfig.componentName)
            {
                (*compIt)->setValue(reconfig.parameterName, reconfig.parameterValue);
                (*compIt)->parameterHasChanged(reconfig.parameterName);
                LOG(LINFO) << "Reconfigured parameter " << reconfig.parameterName << " : " << reconfig.parameterValue;
                bFound = true;
            }
        }

        if(!bFound)
        {
            LOG(LERROR) << "Parametric reconfiguration failed: could not find component: " << reconfig.componentName;
        }
    }

    void PNEngine::postCommand(Command command)
    {
        LOG(LERROR) << "PNComponents do not support commands - failed to post command " <<
                command.commandName << " to " << command.componentName;
    }

    void PNEngine::activateEvent(Event &e)
    {
        if(d_engineManager == NULL)
        {
            LOG(LERROR) << "Failed to activate event: Engine " << d_engineName << " could not access EngineManager";
            return;
        }

        //Simply pass the event on to the EngineManager
        d_engineManager->activateEvent(e);
    }

    bool PNEngine::sameLink(LinkDescription first, LinkDescription second) const
    {
        return (first.sourceComponent == second.sourceComponent &&
            first.sinkComponent == second.sinkComponent &&
            first.sourcePort == second.sourcePort &&
            first.sinkPort == second.sinkPort);
    }

    // Internal namespace for metaprogramming loop 
    namespace internal{
    //! Struct for metaprogramming loop to go through all iris types and create a DataBuffer of the correct type
    template <int N = boost::mpl::size<IrisDataTypes>::value>
    struct getBufferOfType
    {
        static bool EXEC(int type, shared_ptr<DataBufferBase> &ptr)
        {
            typedef typename boost::mpl::at_c<IrisDataTypes,N-1>::type T;

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
        static bool EXEC(int type, shared_ptr<DataBufferBase> &ptr)
        {
            return false;
        }
    };

    //! Struct for metaprogramming loop to go through all iris types and create a PNDataBuffer of the correct type
    template <int N = boost::mpl::size<IrisDataTypes>::value>
    struct getPNBufferOfType
    {
        static bool EXEC(int type, shared_ptr<DataBufferBase> &ptr)
        {
            typedef typename boost::mpl::at_c<IrisDataTypes,N-1>::type T;

            if(N-1 == type)
            {
                ptr.reset(new PNDataBuffer<T>);
                return true;
            }
            else
            {
                return getPNBufferOfType<N-1>::EXEC(type, ptr);
            }
        }
    };
    //! Template specialization to end the recursion
    template <>
    struct getPNBufferOfType<0>
    {
        static bool EXEC(int type, shared_ptr<DataBufferBase> &ptr)
        {
            return false;
        }
    };
    } /* namespace internal */

    //! Create a DataBuffer of a particular data type
    shared_ptr< DataBufferBase > PNEngine::createDataBuffer(int type) const
    {
        shared_ptr< DataBufferBase> ret;
        if(!internal::getBufferOfType<>::EXEC(type, ret))
        {
            throw InvalidDataTypeException("Attempted to create DataBuffer with invalid data type value: " + type);
        }
        return ret;
    }

    //! Create a PNDataBuffer of a particular data type
    shared_ptr< DataBufferBase > PNEngine::createPNDataBuffer(int type) const
    {
        shared_ptr< DataBufferBase> ret;
        if(!internal::getPNBufferOfType<>::EXEC(type, ret))
        {
            throw InvalidDataTypeException("Attempted to create DataBuffer with invalid data type value: " + type);
        }
        return ret;
    }

} /* namespace iris */
