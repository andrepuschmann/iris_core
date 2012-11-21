 /**
 * \file XmlParser.h
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
 * The XmlParser allows us to parse xml radio configuration files and
 * generate RadioRepresentation objects for use in the Iris engine.
 */

#ifndef IRIS_XMLPARSER_H_
#define IRIS_XMLPARSER_H_

#include "iris/RadioRepresentation.h"
#include "irisapi/Exceptions.h"
#include "irisapi/Logging.h"

namespace iris
{

/** The XmlParser allows us to parse xml radio configuration files and
 *  generate RadioRepresentation objects for use in Iris.
 *
 *  It also supports the generation of xml files and strings from RadioRepresentation
 *  objects.
 */
class XmlParser
{
public:
  /** Parse an xml file and generate a RadioRepresentation
  *
  *   \param  filename  The configuration file to parse
  *   \param  radio     The RadioRepresentation object to be filled in
  *
  *   \throw  XMLParsingException       Thrown if an error exists in the xml configuration
  *   \throw  GraphStructureErrorException  Thrown if the radio graphs cannot be built
  */
  static void parseXmlFile( std::string filename, RadioRepresentation &radio) throw (XmlParsingException, GraphStructureErrorException);

  /** Generate an xml file from a given RadioRepresentation
  *
  *   \param  radio     The RadioRepresentation object for the radio
  *   \param  filename  The configuration file to generate
  */
  static void generateXmlFile( RadioRepresentation &radio, std::string filename);

  /** Parse an xml string and generate a RadioRepresentation
  *
  *   \param  xml     The string containing the xml to parse
  *   \param  radio     The RadioRepresentation object to be filled in
  *
  *   \throw  XMLParsingException       Thrown if an error exists in the xml configuration
  *   \throw  GraphStructureErrorException  Thrown if the radio graphs cannot be built
  */
  static void parseXmlString( std::string &xml, RadioRepresentation &radio) throw (XmlParsingException, GraphStructureErrorException);

  /** Generate an xml string from a given RadioRepresentation
  *
  *   \param  radio     The RadioRepresentation object for the radio
  *   \param  xml      The string to hold the generated xml configuration
  */
  static void generateXmlString( RadioRepresentation &radio, std::string &xml);

private:
  /// Disable constructor - all functions are static
  XmlParser() {};
};

} // namespace iris

#endif // IRIS_XMLPARSER_H_
