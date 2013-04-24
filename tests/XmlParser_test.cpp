/**
 * \file XmlParser_test.cpp
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
 * Main test file for XmlParser class.
 */

#define BOOST_TEST_MODULE XmlParserTest

#include <boost/test/unit_test.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

#include <algorithm>
#include <string>

#include "iris/XmlParser.h"

using namespace std;
using namespace iris;

BOOST_AUTO_TEST_SUITE (XmlParserTest)


/* not required anymore - XmlParser only has static functions
BOOST_AUTO_TEST_CASE(XmlParserBasic)
{

    // Check construction of an XMLParser object
    XmlParser* theParser = NULL;
    BOOST_CHECK_NO_THROW(theParser = new XmlParser());
    BOOST_REQUIRE(theParser != NULL);
    delete theParser;

}
*/

BOOST_AUTO_TEST_CASE(XmlParserCleanParse)
{
    string xmlConfig("\
<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\" ?>\
<softwareradio>\
<controller class=\"testcontroller\" />\
<engine name=\"phyengine1\" class=\"phyengine\">\
<component name=\"src1\" class=\"sourcephycomponent\">\
<port name=\"output1\" class=\"output\" />\
</component>\
<component name=\"splitter1\" class=\"splitterphycomponent\">\
<parameter name=\"x_numoutputs\" value=\"2\" />\
<port name=\"input1\" class=\"input\" />\
<port name=\"output1\" class=\"output\" />\
<port name=\"output2\" class=\"output\" />\
</component>\
</engine>\
<engine name=\"phyengine2\" class=\"phyengine\">\
<component name=\"snk1\" class=\"sinkphycomponent\">\
<port name=\"input1\" class=\"input\" />\
</component>\
</engine>\
<engine name=\"phyengine3\" class=\"phyengine\">\
<component name=\"snk2\" class=\"sinkphycomponent\">\
<port name=\"input1\" class=\"input\" />\
</component>\
</engine>\
<link source=\"src1.output1\" sink=\"splitter1.input1\" />\
<link source=\"splitter1.output1\" sink=\"snk1.input1\" />\
<link source=\"splitter1.output2\" sink=\"snk2.input1\" />\
</softwareradio>\
");

    //XmlParser theParser;
    RadioRepresentation theRadio;
    //Parse the xml string
    BOOST_CHECK_NO_THROW(XmlParser::parseXmlString( xmlConfig, theRadio));
    string result;
    //Generate an xml string from the RadioRepresentation
    BOOST_CHECK_NO_THROW(XmlParser::generateXmlString(theRadio, result));
    
    cout << xmlConfig << endl;
    cout << result << endl;

    //Check that the original string matches the generated one
    BOOST_REQUIRE(boost::equals(xmlConfig, result));
}

BOOST_AUTO_TEST_CASE(XmlParserDirtyParse1)
{
    string xmlConfig("\
<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\" ?>\
<softwareradio>\
<engine name=\"phyengine1\" class=\"phyengine\">\
<component name=\"src1\" class=\"sourcephycomponent\">\
<port name=\"output1\" class=\"output\" />\
</component>\
<component name=\"splitter1\" class=\"splitterphycomponent\">\
<parameter name=\"x_numoutputs\" value=\"2\" />\
<port name=\"input1\" class=\"input\" />\
<port name=\"output1\" class=\"output\" />\
<port name=\"output2\" class=\"output\" />\
</component>\
</engine>\
<engine name=\"phyengine2\" class=\"phyengine\">\
<component name=\"snk1\" class=\"sinkphycomponent\">\
<port name=\"input1\" class=\"input\" />\
</component>\
</engine>\
<engine name=\"phyengine3\" class=\"phyengine\">\
<component name=\"snk2\" class=\"sinkphycomponent\">\
<port name=\"input1\" class=\"input\" />\
</component>\
</engine>\
<link source=\"src1.output1\" sink=\"splitter1.input1\" />\
<link source=\"splitter1.output1\" sink=\"snk1.input1\" />\
<link source=\"splitter1.output2\" sink=\"snk2.input1\" />\
</softwareradi>\
"); //Error in closing tag

    //XmlParser theParser;
    RadioRepresentation theRadio;
    //Parse the xml string
    BOOST_CHECK_THROW(XmlParser::parseXmlString( xmlConfig, theRadio), XmlParsingException);
}

BOOST_AUTO_TEST_CASE(XmlParserDirtyParse2)
{
    string xmlConfig("\
<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\" ?>\
<softwareradio>\
<engine name=\"phyengine1\" class=\"phyengine\">\
<component name=\"src1\" class=\"sourcephycomponent\">\
<port name=\"output1\" class=\"output\" />\
</component>\
<component name=\"splitter1\" class=\"splitterphycomponent\">\
<parameter name=\"x_numoutputs\" value=\"2\" />\
<port name=\"input1\" class=\"input\" />\
<port name=\"output1\" class=\"output\" />\
<port name=\"output2\" class=\"output\" />\
</component>\
</engine>\
<engine name=\"phyengine2\" class=\"phyengine\">\
<component name=\"snk1\" class=\"sinkphycomponent\">\
<port name=\"input1\" class=\"input\" />\
</component>\
</engine>\
<engine name=\"phyengine3\" class=\"phyengine\">\
<component name=\"snk2\" class=\"sinkphycomponent\">\
<port name=\"input1\" class=\"input\" />\
</component>\
</engine>\
<link source=\"src1.output1\" sink=\"splitter1.input1\" />\
<link source=\"splitter1.output1\" sink=\"snk1.input1\" />\
<link source=\"splitter1.output2\" sink=\"snk3.input1\" />\
</softwareradio>\
"); //Error last link statement - snk3 doesn't exist

    //XmlParser theParser;
    RadioRepresentation theRadio;
    //Parse the xml string
    BOOST_CHECK_THROW(XmlParser::parseXmlString( xmlConfig, theRadio), GraphStructureErrorException);
}

BOOST_AUTO_TEST_SUITE_END()
