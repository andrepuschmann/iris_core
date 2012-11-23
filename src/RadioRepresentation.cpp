/**
 * \file RadioRepresentation.cpp
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
 * Implementation of RadioRepresentation class - used to hold all
 * information needed to run a radio.
 */

#include "iris/RadioRepresentation.h"
#include "irisapi/ReconfigurationDescriptions.h"

#include <boost/graph/graph_utility.hpp>

using std::vector;
using std::string;
using std::cout;
using std::endl;

namespace iris
{

    RadioRepresentation::RadioRepresentation()
        :isBuilt_(false)
    {}

    RadioRepresentation::RadioRepresentation(const RadioRepresentation& r)
    {
        copy(r);
    }

    RadioRepresentation& RadioRepresentation::operator= (const RadioRepresentation& r)
    {
        copy(r);
        return *this;
    }

    void RadioRepresentation::copy(const RadioRepresentation& r)
    {
        isBuilt_ = r.isBuilt_;
        controllers_ = r.controllers_;
        links_ = r.links_;
        engines_ = r.engines_;
        externalLinks_ = r.externalLinks_;
        radioGraph_ = r.radioGraph_;
        engineGraph_ = r.engineGraph_;
    }

    void RadioRepresentation::addControllerDescription(ControllerDescription con)
    {
        controllers_.push_back(con);
    }

    void RadioRepresentation::addEngineDescription(EngineDescription eng)
    {
        engines_.push_back(eng);
    }


    void RadioRepresentation::addLinkDescription(LinkDescription link)
    {
        links_.push_back(link);
    }

    //Use the link descriptions and engine descriptions to build the radio graph, the engine graph and the
    //internal graphs for each engine.
    void RadioRepresentation::buildGraphs() throw (GraphStructureErrorException)
    {
        boost::mutex::scoped_lock lock(mutex_);

        //Add component descriptions as vertices to radio graph
        vector<EngineDescription>::iterator engIt;
        for(engIt = engines_.begin(); engIt != engines_.end(); engIt++)
        {
            vector<ComponentDescription>::iterator compIt;
            for(compIt = engIt->components.begin(); compIt != engIt->components.end(); compIt++)
            {
                //Add the component description to the component graph
                Vertex v = add_vertex(radioGraph_);
                radioGraph_[v] = *compIt;
            }
        }

        //Add all the links as edges of the radio graph
        vector<LinkDescription>::iterator linkIt;
        for(linkIt = links_.begin(); linkIt != links_.end(); linkIt++)
        {
            Vertex src, snk;
            if(!findComponent(linkIt->sourceComponent, radioGraph_, src))
                throw GraphStructureErrorException("Could not find component " + linkIt->sourceComponent + " referenced by link");
            if(!findComponent(linkIt->sinkComponent, radioGraph_, snk))
                throw GraphStructureErrorException("Could not find component " + linkIt->sinkComponent + " referenced by link");
            bool inserted;
            Edge e;
            tie(e, inserted) = add_edge(src, snk, radioGraph_);
            radioGraph_[e] = *linkIt;
        }

        //Find all the internal and external (ones that cross engine boundaries) edges
        EdgeIterator ei, eiend;
        for(tie(ei,eiend) = edges(radioGraph_); ei != eiend; ++ei)
        {
            string srcEng, snkEng;
            srcEng = radioGraph_[source(*ei, radioGraph_)].engineName;
            snkEng = radioGraph_[target(*ei, radioGraph_)].engineName;

            //Set the source and sink engines
            radioGraph_[*ei].sourceEngine = srcEng;
            radioGraph_[*ei].sinkEngine = snkEng;

            if(srcEng != snkEng) //External link
            {
                //Add to the vector of external links
                externalLinks_.push_back(radioGraph_[*ei]);
            }
            else //Internal link
            {
                //Find the engine which the link belongs to and add it
                for(engIt = engines_.begin(); engIt != engines_.end(); engIt++)
                {
                    if(engIt->name == srcEng)
                        engIt->links.push_back(radioGraph_[*ei]);
                }
            }
        }

        //Build all the individual engine graphs
        for(engIt = engines_.begin(); engIt != engines_.end(); engIt++)
        {
            buildEngineDescriptionGraph(*engIt);
        }

        //Add external links to the engine descriptions
        for(linkIt = externalLinks_.begin(); linkIt != externalLinks_.end(); linkIt++)
        {
            for(engIt = engines_.begin(); engIt != engines_.end(); engIt++)
            {
                if(engIt->name == linkIt->sourceEngine || engIt->name == linkIt->sinkEngine)
                {
                    engIt->links.push_back(*linkIt);
                }
            }
        }

        //Build the overall engine graph
        for(engIt = engines_.begin(); engIt != engines_.end(); engIt++)
        {
            //Add the engine description to the engine graph
            EngVertex engV = add_vertex(engineGraph_);
            engineGraph_[engV] = *engIt;
        }
        vector<LinkDescription>::iterator exLinkIt;
        for(exLinkIt = externalLinks_.begin(); exLinkIt != externalLinks_.end(); ++exLinkIt)
        {
            LinkDescription el = *exLinkIt;
            //Add link to the engine graph
            EngVertex engSrc, engSnk;
            if(!findEngine(el.sourceEngine, engineGraph_, engSrc))
                throw GraphStructureErrorException("Could not find engine " + el.sourceEngine);
            if(!findEngine(el.sinkEngine, engineGraph_, engSnk))
                throw GraphStructureErrorException("Could not find engine " + el.sinkEngine);

            bool inserted;
            EngEdge engE;
            tie(engE, inserted) = add_edge(engSrc, engSnk, engineGraph_);
            engineGraph_[engE] = el;
        }

        isBuilt_ = true;
    }

    void RadioRepresentation::reconfigureRepresentation(ReconfigSet reconfigs)
    {
        //Find all the parametric reconfigurations and apply them
        vector< ParametricReconfig >::iterator paramIt;
        for(paramIt = reconfigs.paramReconfigs.begin();
            paramIt != reconfigs.paramReconfigs.end();
            ++paramIt)
        {
            reconfigureParameter(*paramIt);
        }
    }

    void RadioRepresentation::reconfigureParameter(ParametricReconfig reconfig)
    {
        boost::mutex::scoped_lock lock(mutex_);

        //Apply change to the RadioGraph
        Vertex v;
        ComponentDescription newComp;
        if(!findComponent(reconfig.componentName, radioGraph_, v))
        {
            throw ResourceNotFoundException("Could not find component " + reconfig.componentName + " when reconfiguring RadioRepresentation");
            return;
        }
        vector<ParameterDescription>::iterator paramIt;
        for(paramIt = radioGraph_[v].parameters.begin(); paramIt != radioGraph_[v].parameters.end(); ++paramIt)
        {
            if(paramIt->name == reconfig.parameterName)
                paramIt->value = reconfig.parameterValue;
        }
        newComp = radioGraph_[v];

        //Apply change to the EngineDescription
        EngineDescription engD;
        vector<EngineDescription>::iterator engIt;
        for(engIt = engines_.begin(); engIt != engines_.end(); ++engIt)
        {
            if(engIt->name == reconfig.engineName)
            {
                vector<ComponentDescription>::iterator compIt;
                for(compIt = engIt->components.begin(); compIt != engIt->components.end(); ++compIt)
                {
                    if(compIt->name == reconfig.componentName)
                    {
                        *compIt = newComp;
                        engD = *engIt;
                        break;
                    }
                }
            }
        }

        //Apply change to the EngineGraph
        EngVertex ver;
        if(!findEngine(reconfig.engineName, engineGraph_, ver))
        {
            throw ResourceNotFoundException("Could not find engine " + reconfig.engineName + " when reconfiguring RadioRepresentation");
            return;
        }
        engineGraph_[ver] = engD;
    }

    std::string RadioRepresentation::getParameterValue(std::string paramName, std::string componentName)
    {
        boost::mutex::scoped_lock lock(mutex_);

        Vertex v;
        if(!findComponent(componentName, radioGraph_, v))
        {
            return "";
        }
        vector<ParameterDescription>::iterator paramIt;
        for(paramIt = radioGraph_[v].parameters.begin(); paramIt != radioGraph_[v].parameters.end(); ++paramIt)
        {
            if(paramIt->name == paramName)
                return paramIt->value;
        }
        return "";
    }

    void RadioRepresentation::buildEngineDescriptionGraph(EngineDescription& eng) const throw (GraphStructureErrorException)
    {
        //Add all components as vertices of the graph
        vector<ComponentDescription>::iterator compIt;
        for(compIt = eng.components.begin(); compIt != eng.components.end(); compIt++)
        {
            Vertex v = add_vertex(eng.engineGraph);
            eng.engineGraph[v] = *compIt;
        }

        //Add all the links as edges of the graph
        vector<LinkDescription>::iterator linkIt;
        for(linkIt = eng.links.begin(); linkIt != eng.links.end(); linkIt++)
        {
            Vertex src, snk;
            if(!findComponent(linkIt->sourceComponent, eng.engineGraph, src))
                throw GraphStructureErrorException("Could not find component " + linkIt->sourceComponent + " referenced by link");
            if(!findComponent(linkIt->sinkComponent, eng.engineGraph, snk))
                throw GraphStructureErrorException("Could not find component " + linkIt->sourceComponent + " referenced by link");
            
            bool inserted;
            Edge e;
            tie(e, inserted) = add_edge(src, snk, eng.engineGraph);
            eng.engineGraph[e] = *linkIt;
        }
    }

    bool RadioRepresentation::findComponent(std::string name, const RadioGraph& graph, Vertex& ver)
    {
        VertexIterator i, iend;
        for(tie(i, iend) = vertices(graph); i != iend; ++i)
        {
            if(graph[*i].name == name)
            {
                ver = *i;
                return true;
            }
        }
        return false;
    }

    bool RadioRepresentation::findEngine(std::string name, const EngineGraph& graph, EngVertex& ver)  
    {
        EngVertexIterator i, iend;
        for(tie(i, iend) = vertices(graph); i != iend; ++i)
        {
            if(graph[*i].name == name)
            {
                ver = *i;
                return true;
            }
        }
        return false;
    }

    string RadioRepresentation::printRadioGraph() const
    {
        string result;
        if(!isGraphBuilt())
        {
            result += "Graph has not yet been built";
            return result;
        }
        VertexIterator i, iend;
        OutEdgeIterator ei, eiend;
        for(tie(i, iend) = vertices(radioGraph_); i != iend; ++i)
        {
            result += radioGraph_[*i].name + "\n";
            for(tie(ei,eiend) = out_edges(*i,radioGraph_); ei != eiend; ++ei)
            {
                result += radioGraph_[*i].name + "." + radioGraph_[*ei].sourcePort + " --> " \
                    + radioGraph_[target(*ei, radioGraph_)].name + "." + radioGraph_[*ei].sinkPort + "\n";
            }
        }
        return result;
    }

    string RadioRepresentation::printEngineGraph() const
    {
        string result;
        if(!isGraphBuilt())
        {
            result += "Graph has not yet been built";
            return result;
        }
        EngVertexIterator i, iend;
        EngOutEdgeIterator ei, eiend;
        for(tie(i, iend) = vertices(engineGraph_); i != iend; ++i)
        {
            result += engineGraph_[*i].name + "\n";
            for(tie(ei,eiend) = out_edges(*i,engineGraph_); ei != eiend; ++ei)
            {
                result += engineGraph_[*i].name + "." + engineGraph_[*ei].sourcePort + " --> " \
                    + engineGraph_[target(*ei, engineGraph_)].name + "." += engineGraph_[*ei].sinkPort + "\n";
            }
        }
        return result;
    }

    bool RadioRepresentation::isGraphBuilt() const
    {
        return isBuilt_;
    }

    vector<ControllerDescription> RadioRepresentation::getControllers() const
    {
        return controllers_;
    }

    vector<EngineDescription> RadioRepresentation::getEngines() const
    {
        return engines_;
    }

    vector<LinkDescription> RadioRepresentation::getLinks() const
    {
        return links_;
    }

    vector<LinkDescription> RadioRepresentation::getExternalLinks() const
    {
        return externalLinks_;
    }

    RadioGraph RadioRepresentation::getRadioGraph() const
    {
        return radioGraph_;
    }

    EngineGraph RadioRepresentation::getEngineGraph() const
    {
        return engineGraph_;
    }

}
