/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file ComponentInformation.h
 * Contains class ComponentInformation.
 *
 *  Created on: 20-Nov-2008
 *  Created by: jlotze
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
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
