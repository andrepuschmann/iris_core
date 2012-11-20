/**
 * \file ReconfigurationManager.h
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
 * The Reconfiguration Manager handles all Iris radio reconfigurations.
 */

#ifndef IRIS_RECONFIGURATIONMANAGER_H_
#define IRIS_RECONFIGURATIONMANAGER_H_

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

} // namespace iris

#endif // IRIS_RECONFIGURATIONMANAGER_H_
