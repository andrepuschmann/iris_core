/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file ReconfigurationManager.cpp
 * Implementation of ReconfigurationManager class.
 *
 *  Created on: 2-Apr-2009
 *  Created by: suttonp
 *  $Revision: 1316 $
 *  $LastChangedDate: 2011-09-13 12:41:16 +0100 (Tue, 13 Sep 2011) $
 *  $LastChangedBy: suttonp $
 */

#include "iris/ReconfigurationManager.h"

using namespace std;

namespace iris
{

    ReconfigurationManager::ReconfigurationManager()
    {}

    ReconfigSet ReconfigurationManager::compareRadios(const RadioRepresentation& currentRadio,
        const RadioRepresentation& newRadio)
    {
        ReconfigSet theReconfigs;
        checkParameters(currentRadio, newRadio, theReconfigs);
        return theReconfigs;
    }

    void ReconfigurationManager::checkParameters(const RadioRepresentation& first, const RadioRepresentation& second, ReconfigSet& reconfigs)
    {
        //Go through engines and compare each
        vector< EngineDescription > firstEngines = first.getEngines();
        vector< EngineDescription > secondEngines = second.getEngines();
        vector< EngineDescription >::iterator it1, it2;
        for(it1 = firstEngines.begin(); it1 != firstEngines.end(); ++it1)
        {
            for(it2 = secondEngines.begin(); it2 != secondEngines.end(); ++it2)
            {
                if(*it1 == *it2)
                {
                    checkParameters(*it1, *it2, reconfigs);
                    break;
                }
            }
        }
    }

    void ReconfigurationManager::checkParameters(EngineDescription& first, EngineDescription& second, ReconfigSet& reconfigs)
    {
        //Go through components and compare each
        vector< ComponentDescription >::iterator it1, it2;
        for(it1 = first.components.begin(); it1 != first.components.end(); ++it1)
        {
            for(it2 = second.components.begin(); it2 != second.components.end(); ++it2)
            {
                if(*it1 == *it2)
                {
                    checkParameters(*it1, *it2, reconfigs);
                    break;
                }
            }
        }
    }

    void ReconfigurationManager::checkParameters(ComponentDescription& first, ComponentDescription& second, ReconfigSet& reconfigs)
    {
        //Go through parameters and compare each
        vector< ParameterDescription >::iterator it1, it2;
        for(it1 = first.parameters.begin(); it1 != first.parameters.end(); ++it1)
        {
            for(it2 = second.parameters.begin(); it2 != second.parameters.end(); ++it2)
            {
                if(it1->name == it2->name)
                {
                    if(it1->value != it2->value)
                    {
                        ParametricReconfig r;
                        r.engineName = first.engineName;
                        r.componentName = first.name;
                        r.parameterName = it1->name;
                        r.parameterValue = it2->value;
                        reconfigs.d_paramReconfigs.push_back(r);
                    }
                }
            }
        }
    }

} /* namespace iris */
