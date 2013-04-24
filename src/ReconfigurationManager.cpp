/**
 * \file ReconfigurationManager.cpp
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
 * Implementation of ReconfigurationManager class - used to manage
 * all reconfigurations in Iris.
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
                        reconfigs.paramReconfigs.push_back(r);
                    }
                }
            }
        }
    }

} /* namespace iris */
