/**
 * @file RadioRepresentation.cpp
 * @version 1.0
 *
 * @section COPYRIGHT
 *
 * Copyright 2012 The Iris Project Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution
 * and at http://www.softwareradiosystems.com/iris/copyright.html.
 *
 * @section LICENSE
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
 * @section DESCRIPTION
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
        :isBuilt(false)
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
        isBuilt = r.isBuilt;
        controllers = r.controllers;
        links = r.links;
        engines = r.engines;
        externalLinks = r.externalLinks;
        theRadioGraph = r.theRadioGraph;
        theEngineGraph = r.theEngineGraph;
    }

    void RadioRepresentation::addControllerDescription(ControllerDescription con)
    {
        controllers.push_back(con);
    }

    void RadioRepresentation::addEngineDescription(EngineDescription eng)
    {
        engines.push_back(eng);
    }


    void RadioRepresentation::addLinkDescription(LinkDescription link)
    {
        links.push_back(link);
    }

    //Use the link descriptions and engine descriptions to build the radio graph, the engine graph and the
    //internal graphs for each engine.
    void RadioRepresentation::buildGraphs() throw (GraphStructureErrorException)
    {
        boost::mutex::scoped_lock lock(d_mutex);

        //Add component descriptions as vertices to radio graph
        vector<EngineDescription>::iterator engIt;
        for(engIt = engines.begin(); engIt != engines.end(); engIt++)
        {
            vector<ComponentDescription>::iterator compIt;
            for(compIt = engIt->components.begin(); compIt != engIt->components.end(); compIt++)
            {
                //Add the component description to the component graph
                Vertex v = add_vertex(theRadioGraph);
                theRadioGraph[v] = *compIt;
            }
        }

        //Add all the links as edges of the radio graph
        vector<LinkDescription>::iterator linkIt;
        for(linkIt = links.begin(); linkIt != links.end(); linkIt++)
        {
            Vertex src, snk;
            if(!findComponent(linkIt->sourceComponent, theRadioGraph, src))
                throw GraphStructureErrorException("Could not find component " + linkIt->sourceComponent + " referenced by link");
            if(!findComponent(linkIt->sinkComponent, theRadioGraph, snk))
                throw GraphStructureErrorException("Could not find component " + linkIt->sinkComponent + " referenced by link");
            bool inserted;
            Edge e;
            tie(e, inserted) = add_edge(src, snk, theRadioGraph);
            theRadioGraph[e] = *linkIt;
        }

        //Find all the internal and external (ones that cross engine boundaries) edges
        EdgeIterator ei, eiend;
        for(tie(ei,eiend) = edges(theRadioGraph); ei != eiend; ++ei)
        {
            string srcEng, snkEng;
            srcEng = theRadioGraph[source(*ei, theRadioGraph)].engineName;
            snkEng = theRadioGraph[target(*ei, theRadioGraph)].engineName;

            //Set the source and sink engines
            theRadioGraph[*ei].sourceEngine = srcEng;
            theRadioGraph[*ei].sinkEngine = snkEng;

            if(srcEng != snkEng) //External link
            {
                //Add to the vector of external links
                externalLinks.push_back(theRadioGraph[*ei]);
            }
            else //Internal link
            {
                //Find the engine which the link belongs to and add it
                for(engIt = engines.begin(); engIt != engines.end(); engIt++)
                {
                    if(engIt->name == srcEng)
                        engIt->links.push_back(theRadioGraph[*ei]);
                }
            }
        }

        //Build all the individual engine graphs
        for(engIt = engines.begin(); engIt != engines.end(); engIt++)
        {
            buildEngineDescriptionGraph(*engIt);
        }

        //Add external links to the engine descriptions
        for(linkIt = externalLinks.begin(); linkIt != externalLinks.end(); linkIt++)
        {
            for(engIt = engines.begin(); engIt != engines.end(); engIt++)
            {
                if(engIt->name == linkIt->sourceEngine || engIt->name == linkIt->sinkEngine)
                {
                    engIt->links.push_back(*linkIt);
                }
            }
        }

        //Build the overall engine graph
        for(engIt = engines.begin(); engIt != engines.end(); engIt++)
        {
            //Add the engine description to the engine graph
            EngVertex engV = add_vertex(theEngineGraph);
            theEngineGraph[engV] = *engIt;
        }
        vector<LinkDescription>::iterator exLinkIt;
        for(exLinkIt = externalLinks.begin(); exLinkIt != externalLinks.end(); ++exLinkIt)
        {
            LinkDescription el = *exLinkIt;
            //Add link to the engine graph
            EngVertex engSrc, engSnk;
            if(!findEngine(el.sourceEngine, theEngineGraph, engSrc))
                throw GraphStructureErrorException("Could not find engine " + el.sourceEngine);
            if(!findEngine(el.sinkEngine, theEngineGraph, engSnk))
                throw GraphStructureErrorException("Could not find engine " + el.sinkEngine);

            bool inserted;
            EngEdge engE;
            tie(engE, inserted) = add_edge(engSrc, engSnk, theEngineGraph);
            theEngineGraph[engE] = el;
        }

        isBuilt = true;
    }

    void RadioRepresentation::reconfigureRepresentation(ReconfigSet reconfigs)
    {
        //Find all the parametric reconfigurations and apply them
        vector< ParametricReconfig >::iterator paramIt;
        for(paramIt = reconfigs.d_paramReconfigs.begin();
            paramIt != reconfigs.d_paramReconfigs.end();
            ++paramIt)
        {
            reconfigureParameter(*paramIt);
        }
    }

    void RadioRepresentation::reconfigureParameter(ParametricReconfig reconfig)
    {
        boost::mutex::scoped_lock lock(d_mutex);

        //Apply change to the RadioGraph
        Vertex v;
        ComponentDescription newComp;
        if(!findComponent(reconfig.componentName, theRadioGraph, v))
        {
            throw ResourceNotFoundException("Could not find component " + reconfig.componentName + " when reconfiguring RadioRepresentation");
            return;
        }
        vector<ParameterDescription>::iterator paramIt;
        for(paramIt = theRadioGraph[v].parameters.begin(); paramIt != theRadioGraph[v].parameters.end(); ++paramIt)
        {
            if(paramIt->name == reconfig.parameterName)
                paramIt->value = reconfig.parameterValue;
        }
        newComp = theRadioGraph[v];

        //Apply change to the EngineDescription
        EngineDescription engD;
        vector<EngineDescription>::iterator engIt;
        for(engIt = engines.begin(); engIt != engines.end(); ++engIt)
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
        if(!findEngine(reconfig.engineName, theEngineGraph, ver))
        {
            throw ResourceNotFoundException("Could not find engine " + reconfig.engineName + " when reconfiguring RadioRepresentation");
            return;
        }
        theEngineGraph[ver] = engD;
    }

    std::string RadioRepresentation::getParameterValue(std::string paramName, std::string componentName)
    {
        boost::mutex::scoped_lock lock(d_mutex);

        Vertex v;
        if(!findComponent(componentName, theRadioGraph, v))
        {
            return "";
        }
        vector<ParameterDescription>::iterator paramIt;
        for(paramIt = theRadioGraph[v].parameters.begin(); paramIt != theRadioGraph[v].parameters.end(); ++paramIt)
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
        for(tie(i, iend) = vertices(theRadioGraph); i != iend; ++i)
        {
            result += theRadioGraph[*i].name + "\n";
            for(tie(ei,eiend) = out_edges(*i,theRadioGraph); ei != eiend; ++ei)
            {
                result += theRadioGraph[*i].name + "." + theRadioGraph[*ei].sourcePort + " --> " \
                    + theRadioGraph[target(*ei, theRadioGraph)].name + "." + theRadioGraph[*ei].sinkPort + "\n";
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
        for(tie(i, iend) = vertices(theEngineGraph); i != iend; ++i)
        {
            result += theEngineGraph[*i].name + "\n";
            for(tie(ei,eiend) = out_edges(*i,theEngineGraph); ei != eiend; ++ei)
            {
                result += theEngineGraph[*i].name + "." + theEngineGraph[*ei].sourcePort + " --> " \
                    + theEngineGraph[target(*ei, theEngineGraph)].name + "." += theEngineGraph[*ei].sinkPort + "\n";
            }
        }
        return result;
    }

    bool RadioRepresentation::isGraphBuilt() const
    {
        return isBuilt;
    }

    vector<ControllerDescription> RadioRepresentation::getControllers() const
    {
        return controllers;
    }

    vector<EngineDescription> RadioRepresentation::getEngines() const
    {
        return engines;
    }

    vector<LinkDescription> RadioRepresentation::getLinks() const
    {
        return links;
    }

    vector<LinkDescription> RadioRepresentation::getExternalLinks() const
    {
        return externalLinks;
    }

    RadioGraph RadioRepresentation::getRadioGraph() const
    {
        return theRadioGraph;
    }

    EngineGraph RadioRepresentation::getEngineGraph() const
    {
        return theEngineGraph;
    }

}
