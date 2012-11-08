/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/*
 * file XmlParserTest.cpp
 * Main test file for XmlParser element
 *
 *  Created on: 18-Dec-2008
 *  Created by: suttonp
 *  $Revision: 1346 $
 *  $LastChangedDate: 2011-09-19 18:24:26 +0100 (Mon, 19 Sep 2011) $
 *  $LastChangedBy: suttonp $
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
<engine name=\"pnengine1\" class=\"pnengine\">\
<component name=\"src1\" class=\"sourcepncomponent\">\
<port name=\"output1\" class=\"output\" />\
</component>\
<component name=\"splitter1\" class=\"splitterpncomponent\">\
<parameter name=\"x_numoutputs\" value=\"2\" />\
<port name=\"input1\" class=\"input\" />\
<port name=\"output1\" class=\"output\" />\
<port name=\"output2\" class=\"output\" />\
</component>\
</engine>\
<engine name=\"pnengine2\" class=\"pnengine\">\
<component name=\"snk1\" class=\"sinkpncomponent\">\
<port name=\"input1\" class=\"input\" />\
</component>\
</engine>\
<engine name=\"pnengine3\" class=\"pnengine\">\
<component name=\"snk2\" class=\"sinkpncomponent\">\
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
<engine name=\"pnengine1\" class=\"pnengine\">\
<component name=\"src1\" class=\"sourcepncomponent\">\
<port name=\"output1\" class=\"output\" />\
</component>\
<component name=\"splitter1\" class=\"splitterpncomponent\">\
<parameter name=\"x_numoutputs\" value=\"2\" />\
<port name=\"input1\" class=\"input\" />\
<port name=\"output1\" class=\"output\" />\
<port name=\"output2\" class=\"output\" />\
</component>\
</engine>\
<engine name=\"pnengine2\" class=\"pnengine\">\
<component name=\"snk1\" class=\"sinkpncomponent\">\
<port name=\"input1\" class=\"input\" />\
</component>\
</engine>\
<engine name=\"pnengine3\" class=\"pnengine\">\
<component name=\"snk2\" class=\"sinkpncomponent\">\
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
<engine name=\"pnengine1\" class=\"pnengine\">\
<component name=\"src1\" class=\"sourcepncomponent\">\
<port name=\"output1\" class=\"output\" />\
</component>\
<component name=\"splitter1\" class=\"splitterpncomponent\">\
<parameter name=\"x_numoutputs\" value=\"2\" />\
<port name=\"input1\" class=\"input\" />\
<port name=\"output1\" class=\"output\" />\
<port name=\"output2\" class=\"output\" />\
</component>\
</engine>\
<engine name=\"pnengine2\" class=\"pnengine\">\
<component name=\"snk1\" class=\"sinkpncomponent\">\
<port name=\"input1\" class=\"input\" />\
</component>\
</engine>\
<engine name=\"pnengine3\" class=\"pnengine\">\
<component name=\"snk2\" class=\"sinkpncomponent\">\
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
