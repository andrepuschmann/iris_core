/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/*
 * file ReconfigurationManagerTest.cpp
 * Main test file for ReconfigurationManager element
 *
 *  Created on: 18-Dec-2008
 *  Created by: suttonp
 *  $Revision: 1346 $
 *  $LastChangedDate: 2011-09-19 18:24:26 +0100 (Mon, 19 Sep 2011) $
 *  $LastChangedBy: suttonp $
 */


#define BOOST_TEST_MODULE ReconfigurationManagerTest

#include <boost/test/unit_test.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#include <algorithm>
#include <string>

#include "iris/ReconfigurationManager.h"
#include "iris/XmlParser.h"

using namespace std;
using namespace iris;

BOOST_AUTO_TEST_SUITE (ReconfigurationManagerTest)


BOOST_AUTO_TEST_CASE(ReconfigurationManagerParametersTest)
{
    string xmlConfig1("\
<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\" ?>\
<softwareradio>\
<engine name=\"pnengine1\" class=\"pnengine\">\
<component name=\"src1\" class=\"sourcepncomponent\">\
<parameter name=\"param1\" value=\"1\" />\
<parameter name=\"param2\" value=\"1\" />\
<parameter name=\"param3\" value=\"1\" />\
<port name=\"output1\" class=\"output\" />\
</component>\
<component name=\"snk1\" class=\"sinkpncomponent\">\
<port name=\"input1\" class=\"input\" />\
</component>\
</engine>\
<link source=\"src1.output1\" sink=\"snk1.input1\" />\
</softwareradio>\
");

    string xmlConfig2("\
<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\" ?>\
<softwareradio>\
<engine name=\"pnengine1\" class=\"pnengine\">\
<component name=\"snk1\" class=\"sinkpncomponent\">\
<port name=\"input1\" class=\"input\" />\
</component>\
<component name=\"src1\" class=\"sourcepncomponent\">\
<parameter name=\"param3\" value=\"2\" />\
<parameter name=\"param1\" value=\"2\" />\
<parameter name=\"param2\" value=\"2\" />\
<port name=\"output1\" class=\"output\" />\
</component>\
</engine>\
<link source=\"src1.output1\" sink=\"snk1.input1\" />\
</softwareradio>\
");

    RadioRepresentation first;
    RadioRepresentation second;
    XmlParser::parseXmlString(xmlConfig1, first);
    XmlParser::parseXmlString(xmlConfig2, second);

    ReconfigSet reconfigs = ReconfigurationManager::compareRadios(first, second);
    BOOST_REQUIRE(reconfigs.d_paramReconfigs.size() == 3);
    vector<ParametricReconfig>::iterator it;
    for(it = reconfigs.d_paramReconfigs.begin(); it != reconfigs.d_paramReconfigs.end(); ++it)
    {
        BOOST_REQUIRE(it->engineName == "pnengine1");
        BOOST_REQUIRE(it->componentName == "src1");
        BOOST_REQUIRE(it->parameterName == "param1" || it->parameterName == "param2" || it->parameterName == "param3");
        BOOST_REQUIRE(it->parameterValue == "2");
    }


}

BOOST_AUTO_TEST_SUITE_END()
