/**
 * \file ReconfigurationManager_test.cpp
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
 * Main test file for ReconfigurationManager class.
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
