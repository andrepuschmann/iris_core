/**
 * @file ComponentInformation.h
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
 * Holds information about an Iris Component.
 */

#ifndef COMPONENTINFORMATION_H_
#define COMPONENTINFORMATION_H_

#include <string>

namespace iris {

/*!
 * \brief Hold information about this component
 *
 * The ComponentInformation is initialized in the ComponentBase constructor.
 *
 */

class ComponentInformation
{
private:
    const std::string d_name;
    const std::string d_type;
    const std::string d_description;
    const std::string d_author;
    const std::string d_version;

    //! private default constructor -> cannot be called
    ComponentInformation() {};

public:

    /** Constructs a ComponentInformation object.
     *
     * @param name Name of the component
     * @param type Type of this component
     * @param description What does the component do
     * @param author Author of the component
     * @param version Version number of the component
     */
    ComponentInformation(std::string name, std::string type, std::string description, std::string author,
            std::string version) :
        d_name(name), d_type(type), d_description(description), d_author(author), d_version(version)
    {
    }

    //! Return the component name
    std::string getName() const { return d_name; }

    //! Return the component type
    std::string getType() const { return d_type; }

    //! Return the author of the component
    std::string getAuthor() const { return d_author; }

    //! Return the description of what the component does
    std::string getDescription() const {return d_description; }

    //! Return component version information
    std::string getVersion() const { return d_version; }

};


} // end of namespace iris


#endif /* COMPONENTINFORMATION_H_ */
