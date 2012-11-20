/**
 * \file RadioRepresentation.h
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
 * The RadioRepresentation contains all the information needed by the
 * Iris system to build a radio.
 */

#ifndef IRIS_RADIOREPRESENTATION_H_
#define IRIS_RADIOREPRESENTATION_H_

#include <cstdlib>
#include <string>

#include <boost/thread/mutex.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "irisapi/Exceptions.h"
#include "irisapi/LinkDescription.h"

namespace iris
{
    //Forward declarations
    class DataBufferBase; 
    struct ReconfigSet;
    struct ParametricReconfig;

    //! A controller
    struct ControllerDescription
    {
        std::string type;
    };

    //! A component input or output port
    struct PortDescription
    {
        std::string name;
        std::string type;

        bool operator==(const PortDescription& port) const
        {
            return (name == port.name && type == port.type);   
        }
    };

    //! A component parameter
    struct ParameterDescription
    {
        std::string name;
        std::string value;

        bool operator==(const ParameterDescription& param) const
        {
            return (name == param.name && value == param.value);   
        }
    };

    //! A component
    struct ComponentDescription
    {
        std::string name;
        std::string type;
        std::string engineName;
        std::vector<ParameterDescription> parameters;
        std::vector<PortDescription> ports;

        //! == operator only looks at name, type and engineName
        bool operator==(const ComponentDescription& comp) const
        {
            return (name == comp.name && type == comp.type && engineName == comp.engineName);
        }
    };

    /** The graph used to represent the radio structure of components and links
     *
     *  We use a directed graph which allows us to access both the
     *  in-edges and out-edges of each vertex (boost::bidirectionalS).
     *  Vectors are used for internal storage and the ComponentDescription
     *  and LinkDescription structs are added as bundled properties of
     *  vertices and edges respectively.
     */
    typedef boost::adjacency_list< \
        boost::vecS, boost::vecS, boost::bidirectionalS, \
        ComponentDescription, \
        LinkDescription \
    > RadioGraph;
    typedef boost::graph_traits < RadioGraph >::vertex_descriptor    Vertex;
    typedef boost::graph_traits < RadioGraph >::edge_descriptor      Edge;
    typedef boost::graph_traits < RadioGraph >::vertex_iterator      VertexIterator;
    typedef boost::graph_traits < RadioGraph >::edge_iterator        EdgeIterator;
    typedef boost::graph_traits < RadioGraph >::out_edge_iterator    OutEdgeIterator;
    typedef boost::graph_traits < RadioGraph >::in_edge_iterator     InEdgeIterator;

     //! An Iris v2.0 engine
    struct EngineDescription
    {
        std::string name;
        std::string type;
        RadioGraph engineGraph;
        std::vector<ComponentDescription> components;
        std::vector<LinkDescription> links;

        //! == operator only looks at name and type
        bool operator==(const EngineDescription& eng) const
        {
            return (name == eng.name && type == eng.type);
        }
    };

    /** The graph used to represent the radio structure of engines and links
     *
     *  We use a directed graph which allows us to access both the
     *  in-edges and out-edges of each vertex (boost::bidirectionalS).
     *  Vectors are used for internal storage and the EngineDescription
     *  and LinkDescription structs are added as bundled properties of
     *  vertices and edges respectively.
     */
    typedef boost::adjacency_list< \
        boost::vecS, boost::vecS, boost::bidirectionalS, \
        EngineDescription, \
        LinkDescription \
    > EngineGraph;
    typedef boost::graph_traits < EngineGraph >::vertex_descriptor    EngVertex;
    typedef boost::graph_traits < EngineGraph >::edge_descriptor      EngEdge;
    typedef boost::graph_traits < EngineGraph >::vertex_iterator      EngVertexIterator;
    typedef boost::graph_traits < EngineGraph >::edge_iterator        EngEdgeIterator;
    typedef boost::graph_traits < EngineGraph >::out_edge_iterator    EngOutEdgeIterator;
    typedef boost::graph_traits < EngineGraph >::in_edge_iterator     EngInEdgeIterator;




/*! \class RadioRepresentation
 *  \brief The RadioRepresentation contains all the information needed by the
 *  Iris v2.0 system to build a radio.
 */
class RadioRepresentation
{
    private:
        //! A graph of all components and their links
        RadioGraph theRadioGraph;
        //! A graph of all engines and their links
        EngineGraph theEngineGraph;

        //! All the controllers in this RadioRepresentation
        std::vector<ControllerDescription> controllers;
        //! All the links in this RadioRepresentation
        std::vector<LinkDescription> links;
        //! All the engines in this RadioRepresentation
        std::vector<EngineDescription> engines;
        //! All the external links (from one engine to another) in this RadioRepresentation
        std::vector<LinkDescription> externalLinks;

        //! Have the graphs been built?
        bool isBuilt;

        //! A mutex to ensure thread-safe access
        mutable boost::mutex mutex_;

        //! Reconfigure a parameter within the representation
        void reconfigureParameter(ParametricReconfig reconfig);

        //! Build an engine graph
        void buildEngineDescriptionGraph(EngineDescription& eng) const throw (GraphStructureErrorException);

    public:
        //! Constructor
        RadioRepresentation();

        //! Copy constructor
        RadioRepresentation(const RadioRepresentation& r);

        //! Assignment operator
        RadioRepresentation& operator= (const RadioRepresentation& r);

        //! Copy a RadioRepresentation
        void copy(const RadioRepresentation& r);

        //! Add a ControllerDescription to this RadioRepresentation
        void addControllerDescription(ControllerDescription con);
        //! Add an EngineDescription to this RadioRepresentation
        void addEngineDescription(EngineDescription eng);
        //! Add a LinkDescription to this RadioRepresentation
        void addLinkDescription(LinkDescription link);

        /** Build the graphs of this RadioRepresentation
        *
        *   Every RadioRepresentation holds two graphs - theRadioGraph and theEngineGraph.
        *   theRadioGraph contains all components and the links between them.
        *   theEngineGraph contains all engines and the links between them.
        *   This function uses the EngineDescriptions and LinkDescriptions to build these graphs
        */
        void buildGraphs() throw (GraphStructureErrorException);
        //! Have the graphs for this RadioRepresentation been built?
        bool isGraphBuilt() const;

        //! Reconfigure the representation
        void reconfigureRepresentation(ReconfigSet reconfigs);

        //! Get the current value of a parameter
        std::string getParameterValue(std::string paramName, std::string componentName);

        //! Print the structure of theRadioGraph to a string
        std::string printRadioGraph() const;
        //! Print the structure of theEngineGraph to a string
        std::string printEngineGraph() const;

        std::vector<ControllerDescription> getControllers() const;
        std::vector<EngineDescription> getEngines() const;
        std::vector<LinkDescription> getLinks() const;
        std::vector<LinkDescription> getExternalLinks() const;

        RadioGraph getRadioGraph() const;
        EngineGraph getEngineGraph() const;

        //! Find a component in a RadioGraph
        static bool findComponent(std::string name, const RadioGraph& graph, Vertex& ver);
        //! Find an engine in an EngineGraph
        static bool findEngine(std::string name, const EngineGraph& graph, EngVertex& ver);
};

//! output to an ostream
inline std::ostream& operator <<(std::ostream& os, const RadioRepresentation& rg)
{
    return os << "Radio Graph: " << std::endl << rg.printRadioGraph() << std::endl
              << "Engine Graph: " << std::endl << rg.printEngineGraph();
}

} // namespace iris

#endif // IRIS_RADIOREPRESENTATION_H_
