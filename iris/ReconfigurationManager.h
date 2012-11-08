/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file ReconfigurationManager.h
 * The Reconfiguration Manager which handles all radio reconfigurations
 *
 *  Created on: 2-Jan-2009
 *  Created by: suttonp
 *  $Revision: 1195 $
 *  $LastChangedDate: 2010-12-21 13:56:46 +0000 (Tue, 21 Dec 2010) $
 *  $LastChangedBy: suttonp $
 *
 */

#ifndef RECONFIGURATIONMANAGER_H_
#define RECONFIGURATIONMANAGER_H_

#include <cstdlib>
#include <string>

#include "iris/RadioRepresentation.h"
#include "irisapi/ReconfigurationDescriptions.h"

namespace iris
{

    /*! \class ReconfigurationManager
     *  \brief The ReconfigurationManager allows us to compare radio configurations and
     *  generate reconfiguration sets to be carried out.
     */
    class ReconfigurationManager
    {
    private:
        //! ctor - all static functions so prevent creation of a ReconfigurationManager object
        ReconfigurationManager();

        static void checkParameters(const RadioRepresentation& first, const RadioRepresentation& second, ReconfigSet& reconfigs);
        static void checkParameters(EngineDescription& first, EngineDescription& second, ReconfigSet& reconfigs);
        static void checkParameters(ComponentDescription& first, ComponentDescription& second, ReconfigSet& reconfigs);
    public:

        /** Compare two radio representations and generate a set of reconfigurations
        *
        *   \param  currentRadio    The currently loaded radio configuration
        *   \param  newRadio        The new radio configuration
        */
        static ReconfigSet compareRadios(const RadioRepresentation& currentRadio, const RadioRepresentation& newRadio);

    };

}

#endif
