/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file XmlParser.h
 *
 *  Created on: 18-Dec-2008
 *  Created by: sutttonp
 *  $Revision: 712 $
 *  $LastChangedDate: 2009-10-20 19:28:06 +0100 (Tue, 20 Oct 2009) $
 *  $LastChangedBy: lotzej $
 *
 *  ToDo:
 *      Add error checking and log messages.
 */

#ifndef XMLPARSER_H_
#define XMLPARSER_H_

#include "iris/RadioRepresentation.h"
#include "irisapi/Exceptions.h"
#include "irisapi/Logging.h"

namespace iris
{

    /*! \class XmlParser
     *  \brief The XmlParser allows us to parse xml radio configuration files and
     *  generate RadioRepresentation objects for use in the Iris v2.0 engine.
     *  It also supports the generation of xml files and strings from RadioRepresentation
     *  objects.
     */
    class XmlParser
    {
    private:
        //! ctor - all static function, so we can't construct this
        XmlParser() {};

	public:


		/** Parse an xml file and generate a RadioRepresentation
        *
        *   \param  filename    The configuration file to parse
        *   \param  radio       The RadioRepresentation object to be filled in
        *
        *   \throw  XMLParsingException             Thrown if an error exists in the xml configuration
        *   \throw  GraphStructureErrorException    Thrown if the radio graphs cannot be built
        */
        static void parseXmlFile( std::string filename, RadioRepresentation &radio) throw (XmlParsingException, GraphStructureErrorException);

        /** Generate an xml file from a given RadioRepresentation
        *
        *   \param  radio       The RadioRepresentation object for the radio
        *   \param  filename    The configuration file to generate
        */
		static void generateXmlFile( RadioRepresentation &radio, std::string filename);

		/** Parse an xml string and generate a RadioRepresentation
        *
        *   \param  xml         The string containing the xml to parse
        *   \param  radio       The RadioRepresentation object to be filled in
        *
        *   \throw  XMLParsingException             Thrown if an error exists in the xml configuration
        *   \throw  GraphStructureErrorException    Thrown if the radio graphs cannot be built
        */
		static void parseXmlString( std::string &xml, RadioRepresentation &radio) throw (XmlParsingException, GraphStructureErrorException);

        /** Generate an xml string from a given RadioRepresentation
        *
        *   \param  radio       The RadioRepresentation object for the radio
        *   \param  xml		    The string to hold the generated xml configuration
        */
		static void generateXmlString( RadioRepresentation &radio, std::string &xml);
    };

} /* namespace iris */

#endif /* XMLPARSER_H_ */
