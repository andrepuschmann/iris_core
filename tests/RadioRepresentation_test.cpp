/**
 * \file RadioRepresentation_test.cpp
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
 * Main test file for RadioRepresentation class.
 */

#define BOOST_TEST_MODULE RadioRepresentationTest

#include <boost/test/unit_test.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#include <algorithm>
#include <string>
#include <iostream>

#include "iris/RadioRepresentation.h"

using namespace std;
using namespace iris;

BOOST_AUTO_TEST_SUITE (RadioRepresentationTest)


BOOST_AUTO_TEST_CASE(RadioRepresentationBasic)
{
    // Check construction of a RadioRepresentation object
    RadioRepresentation* theRadio = NULL;
    BOOST_CHECK_NO_THROW(theRadio = new RadioRepresentation());
    BOOST_REQUIRE(theRadio != NULL);
    delete theRadio;
}

BOOST_AUTO_TEST_CASE(RadioRepresentationCleanBuild)
{
    //Try building a simple radio representation with one engine and two components
    ParameterDescription par1;
    par1.name = "parameter1";
    par1.value = "1";

    PortDescription por1, por2;
    por1.name = "output1";
    por1.type = "output";
    por1.name = "input1";
    por1.type = "input";

    ComponentDescription comp1, comp2;
    comp1.name = "comp1";
    comp1.type = "testcomp";
    comp1.parameters.push_back(par1);
    comp1.ports.push_back(por1);
    comp1.engineName = "testengine";
    comp2.name = "comp2";
    comp2.type = "testcomp";
    comp2.ports.push_back(por2);
    comp2.engineName = "testengine";

    LinkDescription l1;
    l1.sourceComponent = "comp1";
    l1.sinkComponent = "comp2";
    l1.sourcePort = "output1";
    l1.sinkPort = "input1";

    EngineDescription eng1;
    eng1.name = "eng1";
    eng1.type = "testengine";
    eng1.components.push_back(comp1);
    eng1.components.push_back(comp2);

    RadioRepresentation r;
    r.addEngineDescription(eng1);
    r.addLinkDescription(l1);

    BOOST_CHECK_EQUAL(r.isGraphBuilt(), false);
    BOOST_CHECK_NO_THROW(r.buildGraphs());
    BOOST_CHECK_EQUAL(r.isGraphBuilt(), true);

    //Check the structure of the RadioGraph
    RadioGraph theRGraph = r.getRadioGraph();
    BOOST_CHECK_EQUAL(num_vertices(theRGraph), 2);
    BOOST_CHECK_EQUAL(num_edges(theRGraph), 1);

    VertexIterator i, iend;
    OutEdgeIterator ei, eiend;
    for(tie(i, iend) = vertices(theRGraph); i != iend; ++i)
    {
        for(tie(ei,eiend) = out_edges(*i,theRGraph); ei != eiend; ++ei)
        {
            BOOST_CHECK_EQUAL(theRGraph[*ei].sourcePort, "output1");
            BOOST_CHECK_EQUAL(theRGraph[*ei].sinkPort, "input1");
            BOOST_CHECK_EQUAL(theRGraph[*ei].sourceComponent, "comp1");
            BOOST_CHECK_EQUAL(theRGraph[*ei].sinkComponent, "comp2");

            Vertex v;
            v = source(*ei, theRGraph);
            BOOST_CHECK_EQUAL(theRGraph[v].name, "comp1");
            v = target(*ei, theRGraph);
            BOOST_CHECK_EQUAL(theRGraph[v].name, "comp2");
        }
    }

    //Check the engine graph structure
    EngineGraph theEGraph = r.getEngineGraph();
    BOOST_CHECK_EQUAL(num_vertices(theEGraph), 1);
    BOOST_CHECK_EQUAL(num_edges(theEGraph), 0);

    //Check the printing functions
    cout << "Printing radio graph" << endl;
    string result = r.printRadioGraph();
    cout << result;
    cout << "Printing engine graph" << endl;
    result = r.printEngineGraph();
    cout << result;

}

BOOST_AUTO_TEST_CASE(RadioRepresentationDirtyBuild)
{
    //Try building a simple radio representation with one engine and two components
    ParameterDescription par1;
    par1.name = "parameter1";
    par1.value = "1";

    PortDescription por1, por2;
    por1.name = "output1";
    por1.type = "output";
    por1.name = "input1";
    por1.type = "input";

    ComponentDescription comp1, comp2;
    comp1.name = "comp1";
    comp1.type = "testcomp";
    comp1.parameters.push_back(par1);
    comp1.ports.push_back(por1);
    comp1.engineName = "testengine";
    comp2.name = "comp2";
    comp2.type = "testcomp";
    comp2.ports.push_back(por2);
    comp2.engineName = "testengine";

    LinkDescription l1;
    l1.sourceComponent = "comp0";   //Error here - this comp name doesn't exist
    l1.sinkComponent = "comp2";
    l1.sourcePort = "output1";
    l1.sinkPort = "input1";

    EngineDescription eng1;
    eng1.name = "eng1";
    eng1.type = "testengine";
    eng1.components.push_back(comp1);
    eng1.components.push_back(comp2);

    RadioRepresentation r;
    r.addEngineDescription(eng1);
    r.addLinkDescription(l1);

    BOOST_CHECK_EQUAL(r.isGraphBuilt(), false);
    BOOST_CHECK_THROW(r.buildGraphs(), GraphStructureErrorException);
    BOOST_CHECK_EQUAL(r.isGraphBuilt(), false);
}

BOOST_AUTO_TEST_SUITE_END()
