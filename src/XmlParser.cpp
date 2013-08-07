/**
 * \file XmlParser.cpp
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
 * Implementation of XmlParser class used to parse xml radio configurations.
 */

#define TIXML_USE_TICPP

#include "iris/XmlParser.h"

#include "ticpp.h"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace ticpp;

namespace iris
{

/******************* Helper functions ******************************/
std::string getName()
{
    return "XmlParser";
}

LinkDescription readLink(Element &linkElem)
{
    //Check for illegal nodes here
    Iterator< Node > c;
    for ( c = c.begin(&linkElem); c != c.end(); c++ )
    {
        if(c->Type() == TiXmlNode::ELEMENT)
        {
            string s = c->Value();
            LOG(LFATAL) << "Illegal element in xml file: " << s;
            throw XmlParsingException("Illegal element in xml file: " + s);
        }
    }

    LinkDescription theLink;
    string source, sink;
    if(linkElem.HasAttribute("sink"))
        sink = linkElem.GetAttribute("sink");
    else
        sink = linkElem.GetAttribute("below");
    if(linkElem.HasAttribute("source"))
        source = linkElem.GetAttribute("source");
    else
        source = linkElem.GetAttribute("above");

    //Create tokenizers
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(".");
    tokenizer sourceTokens(source, sep);
    tokenizer sinkTokens(sink, sep);

    //Pull out source component and port
    tokenizer::iterator tok_iter = sourceTokens.begin();
    theLink.sourceComponent = (*tok_iter++);
    theLink.sourcePort = (*tok_iter);

    //Pull out sink component and port
    tok_iter = sinkTokens.begin();
    theLink.sinkComponent = (*tok_iter++);
    theLink.sinkPort = (*tok_iter);

    boost::to_lower(theLink.sourceComponent);
    boost::to_lower(theLink.sourcePort);
    boost::to_lower(theLink.sinkComponent);
    boost::to_lower(theLink.sinkPort);

    LOG(LINFO) << "Parsed link: " << theLink.sourceComponent << " . " << theLink.sourcePort \
        << " -> " << theLink.sinkComponent << " . " << theLink.sinkPort;

    return theLink;
}

ControllerDescription readController(Element &controllerElem)
{
    //Check for illegal nodes here
    Iterator< Node > c;
    for ( c = c.begin(&controllerElem); c != c.end(); c++ )
    {
        if(c->Type() == TiXmlNode::ELEMENT)
        {
            string s = c->Value();
            LOG(LFATAL) << "Illegal element in xml file: " << s;
            throw XmlParsingException("Illegal element in xml file: " + s);
        }
    }

    ControllerDescription theController;

    //Parse the component type
    theController.type = controllerElem.GetAttribute("class");
    boost::to_lower(theController.type);
    LOG(LINFO) << "Parsed controller: " << theController.type;

    return theController;
}


ComponentDescription readComponent(Element &componentElem)
{
    //Check for illegal nodes here
    Iterator< Node > c;
    for ( c = c.begin(&componentElem); c != c.end(); c++ )
    {
        if(c->Type() == TiXmlNode::ELEMENT)
        {
            string s = c->Value();
            if(s != "port" && s != "parameter")
            {
                LOG(LFATAL) << "Illegal element in xml file: " << s;
                throw XmlParsingException("Illegal element in xml file: " + s);
            }
        }
    }

    ComponentDescription theComponent;

    //Parse the component type and name
    theComponent.name = componentElem.GetAttribute("name");
    theComponent.type = componentElem.GetAttribute("class");
    boost::to_lower(theComponent.name);
    boost::to_lower(theComponent.type);
    LOG(LINFO) << "Parsed component: " << theComponent.name;

    //Parse all the parameters and add to the component description
    Iterator< Element > child("parameter");
    for ( child = child.begin(&componentElem); child != child.end(); child++ )
    {
        ParameterDescription param;
        param.name = child->GetAttribute("name");
        param.value = child->GetAttribute("value");
        boost::to_lower(param.name);
        boost::to_lower(param.value);
        theComponent.parameters.push_back(param);
    }

    //Parse all the ports and add to the component description
    Iterator< Element > child2("port");
    for ( child2 = child2.begin(&componentElem); child2 != child2.end(); child2++ )
    {
        PortDescription port;
        port.name = child2->GetAttribute("name");
        port.type = child2->GetAttribute("class");
        boost::to_lower(port.name);
        boost::to_lower(port.type);
        theComponent.ports.push_back(port);
    }

    return theComponent;
}

EngineDescription readEngine(Element &engineElem)
{
    //Check for illegal elements here
    Iterator< Node > c;
    for ( c = c.begin(&engineElem); c != c.end(); c++ )
    {
        if(c->Type() == TiXmlNode::ELEMENT)
        {
            string s = c->Value();
            if(s != "component")
            {
                LOG(LFATAL) << "Illegal element in xml file: " << s;
                throw XmlParsingException("Illegal element in xml file: " + s);
            }
        }
    }

    EngineDescription theEngine;
    theEngine.name = engineElem.GetAttribute("name");
    theEngine.type = engineElem.GetAttribute("class");
    boost::to_lower(theEngine.name);
    boost::to_lower(theEngine.type);
    LOG(LINFO) << "Parsed engine: " << theEngine.name;

    //Parse all the components and add to the engine description
    Iterator< Element > child("component");
    for ( child = child.begin(&engineElem); child != child.end(); child++ )
    {
        ComponentDescription comp = readComponent(*child);
        comp.engineName = theEngine.name;
        theEngine.components.push_back(comp);
    }

    return theEngine;
}

void readSoftwareRadio(Element &headElem, RadioRepresentation& theRadio)
{
    //Check for illegal nodes here
    Iterator< Node > c;
    for ( c = c.begin(&headElem); c != c.end(); c++ )
    {
        if(c->Type() == TiXmlNode::ELEMENT)
        {
            string s = c->Value();
            if(s != "controller" && s != "engine" && s!= "link")
            {
                LOG(LFATAL) << "Illegal element in xml file: " << s;
                throw XmlParsingException("Illegal element in xml file: " + s);
            }
        }
    }

    //Parse all the controllers
    Iterator< Element > child("controller");
    for ( child = child.begin(&headElem); child != child.end(); child++ )
    {
        ControllerDescription con = readController(*child);
        theRadio.addControllerDescription(con);
    }

    //Parse all the engines
    Iterator< Element > child1("engine");
    for ( child1 = child1.begin(&headElem); child1 != child1.end(); child1++ )
    {
        EngineDescription eng = readEngine(*child1);
        theRadio.addEngineDescription(eng);
    }

    //Parse all the links
    Iterator< Element > child2("link");
    for ( child2 = child2.begin(&headElem); child2 != child2.end(); child2++ )
    {
        LinkDescription link = readLink(*child2);
        theRadio.addLinkDescription(link);
    }
}

Element writeComponent( ComponentDescription &compDesc)
{
    Element e("component");
    e.SetAttribute("name", compDesc.name);
    e.SetAttribute("class", compDesc.type);

    //Add all the parameters
    vector<ParameterDescription> params = compDesc.parameters;
    vector<ParameterDescription>::iterator paramIt;
    for(paramIt=params.begin(); paramIt!=params.end(); paramIt++)
    {
        Element currentParam("parameter");
        currentParam.SetAttribute("name", paramIt->name);
        currentParam.SetAttribute("value", paramIt->value);
        e.InsertEndChild(currentParam);
    }

    //Add all the ports
    vector<PortDescription> ports = compDesc.ports;
    vector<PortDescription>::iterator portIt;
    for(portIt=ports.begin(); portIt!=ports.end(); portIt++)
    {
        Element currentPort("port");
        currentPort.SetAttribute("name", portIt->name);
        currentPort.SetAttribute("class", portIt->type);
        e.InsertEndChild(currentPort);
    }

    return e;
}

Element writeLink( LinkDescription &linkDesc)
{
    Element e("link");

    e.SetAttribute("source", linkDesc.sourceComponent + "." + linkDesc.sourcePort);
    e.SetAttribute("sink", linkDesc.sinkComponent + "." + linkDesc.sinkPort);

    return e;
}

Element writeEngine( EngineDescription &engDesc)
{
    Element e("engine");
    e.SetAttribute("name", engDesc.name);
    e.SetAttribute("class", engDesc.type);

    //Write all the components in the engine
    vector<ComponentDescription> components = engDesc.components;
    vector<ComponentDescription>::iterator compIt;
    for(compIt=components.begin();compIt!=components.end();compIt++)
    {
        Element currentComp = writeComponent(*compIt);
        e.InsertEndChild(currentComp);
    }

    return e;
}

Element writeController( ControllerDescription &conDesc)
{
    Element e("controller");
    e.SetAttribute("class", conDesc.type);

    return e;
}

Element writeSoftwareRadio( RadioRepresentation &swrDesc)
{
    Element e("softwareradio");

    //Write all the controllers
    vector<ControllerDescription> controllers = swrDesc.getControllers();
    vector<ControllerDescription>::iterator conIt;
    for(conIt=controllers.begin();conIt!=controllers.end();conIt++)
    {
        Element currentController = writeController(*conIt);
        e.InsertEndChild(currentController);
    }

    //Write all the engines
    vector<EngineDescription> engines = swrDesc.getEngines();
    vector<EngineDescription>::iterator engIt;
    for(engIt=engines.begin();engIt!=engines.end();engIt++)
    {
        Element currentEngine = writeEngine(*engIt);
        e.InsertEndChild(currentEngine);
    }

    //Write all the links
    vector<LinkDescription> links = swrDesc.getLinks();
    vector<LinkDescription>::iterator linkIt;
    for(linkIt=links.begin();linkIt!=links.end();linkIt++)
    {
        Element currentLink = writeLink(*linkIt);
        e.InsertEndChild(currentLink);
    }

    return e;
}

/***********************End of Helper functions******************************/


//Parses the specified Xml file into a radio description
void XmlParser::parseXmlFile(std::string filename, RadioRepresentation &radio)
{
    try{
        Document doc(filename);
        doc.LoadFile();

        //Pull out the root element
        Element* head = doc.FirstChildElement();
        string headValue = head->Value();
        if( headValue == "softwareradio")
        {
            readSoftwareRadio(*head, radio);
        }else{
            throw XmlParsingException("The root element of the xml configuration must be \"softwareradio\".");
        }

        //Instruct the radio description to build a graph of the radio
        radio.buildGraphs();
    }
    catch( Exception& ex )
    {
        throw XmlParsingException(ex.what());
    }
}

void XmlParser::parseXmlString( std::string &xml, RadioRepresentation &radio)
{
    try{
        Document doc;
        doc.Parse(xml);

        //Pull out the root element
        Element* head = doc.FirstChildElement();
        string headValue = head->Value();
        if( headValue == "softwareradio")
        {
            readSoftwareRadio(*head, radio);
        }else{
            throw XmlParsingException("The top element of the xml configuration must be softwareradio.");
        }

        //Instruct the radio description to build a graph of the radio
        radio.buildGraphs();
    }
    catch( Exception& ex )
    {
        throw XmlParsingException(ex.what());
    }
}

void XmlParser::generateXmlFile( RadioRepresentation& radio, std::string filename)
{
    try{
        Document doc(filename);

        //Create a declaration and insert it
        Declaration decl("1.0", "utf-8", "yes");
        doc.InsertEndChild(decl);

        //Create the root softwareradio node and insert it
        Element softRad = writeSoftwareRadio(radio);
        doc.InsertEndChild(softRad);

        doc.SaveFile();
    }
    catch( Exception& ex )
    {
        LOG(LERROR) << ex.what();
    }
}

void XmlParser::generateXmlString( RadioRepresentation &radio, std::string &xml)
{
    try{
        Document doc;

        //Create a declaration and insert it
        Declaration decl("1.0", "utf-8", "yes");
        doc.InsertEndChild(decl);

        //Create the root softwareradio node and insert it
        Element softRad = writeSoftwareRadio(radio);
        doc.InsertEndChild(softRad);

        stringstream ss;
        ss << doc;
        xml = ss.str();
    }
    catch( Exception& ex )
    {
        LOG(LERROR) << ex.what();
    }
}

} /* namespace iris */
