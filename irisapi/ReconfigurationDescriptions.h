/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file ReconfigurationDescriptions.h
 * Structs which describe reconfigurations to be carried out within a running radio
 *
 *  Created on: 2-Apr-2009
 *  Created by: suttonp
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
 *
 */

#ifndef RECONFIGURATIONDESCRIPTIONS_H_
#define RECONFIGURATIONDESCRIPTIONS_H_

#include <string>
#include <vector>

#include "irisapi/LinkDescription.h"

namespace iris
{

//! A parametric reconfiguration 
struct ParametricReconfig
{
    std::string engineName;
    std::string componentName;
    std::string parameterName;
    std::string parameterValue;
};

//! An altered link
struct AlteredLink
{
    LinkDescription oldLink;
    LinkDescription newLink;
};

//! A structural reconfiguration
struct StructuralReconfig
{
    std::string engineName;
    std::vector< std::string > removedComponents;
    std::vector< LinkDescription > removedLinks;
    //std::vector< ComponentDescription > newComponents;
    std::vector< LinkDescription > newLinks;
    std::vector< AlteredLink > alteredLinks;
};

/** A set of radio reconfigurations
*
*   A ReconfigSet contains a number of reconfigurations which must be carried out 
*   atomically by an engine with respect to the data flow.
*/
struct ReconfigSet
{
    //! A set of parametric reconfigurations 
    std::vector< ParametricReconfig > d_paramReconfigs;

    //! The set of structural reconfigurations
    std::vector< StructuralReconfig > d_structReconfigs;
};

} /* namespace iris */

#endif /* RECONFIGURATIONDESCRIPTIONS_H_ */
