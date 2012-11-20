/**
 * \file ComponentInformation.h
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
    const std::string name_;
    const std::string type_;
    const std::string description_;
    const std::string author_;
    const std::string version_;

    //! private default constructor -> cannot be called
    ComponentInformation() {};

public:

    /** Constructs a ComponentInformation object.
     *
     * \param name Name of the component
     * \param type Type of this component
     * \param description What does the component do
     * \param author Author of the component
     * \param version Version number of the component
     */
    ComponentInformation(std::string name, std::string type, std::string description, std::string author,
            std::string version) :
        name_(name), type_(type), description_(description), author_(author), version_(version)
    {
    }

    //! Return the component name
    std::string getName() const { return name_; }

    //! Return the component type
    std::string getType() const { return type_; }

    //! Return the author of the component
    std::string getAuthor() const { return author_; }

    //! Return the description of what the component does
    std::string getDescription() const {return description_; }

    //! Return component version information
    std::string getVersion() const { return version_; }

};


} // end of namespace iris


#endif /* COMPONENTINFORMATION_H_ */
