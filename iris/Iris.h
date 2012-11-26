/**
 * \file Iris.h
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
 * Iris.h provides the top level C interface to the Iris architecture.
 */

#ifndef IRIS_IRIS_H_
#define IRIS_IRIS_H_

#ifdef WIN32
#ifdef IRIS_EXPORT
#define IRIS_DLL __declspec(dllexport)
#else
#define IRIS_DLL __declspec(dllimport)
#endif
#else
#define IRIS_DLL
#endif
#include <string>

/// Initialize the system
IRIS_DLL void IRISInitSystem();

/// Set the Stack component repository
IRIS_DLL bool IRISSetStackRepository(std::string rep);

/// Set the Phy component repository
IRIS_DLL bool IRISSetPhyRepository(std::string rep);

/// Set the SDF component repository
IRIS_DLL bool IRISSetSdfRepository(std::string rep);

/// Set the controller repository
IRIS_DLL bool IRISSetContRepository(std::string rep);

/// Set the log level
IRIS_DLL bool IRISSetLogLevel(std::string level);

/** Load the radio
*
*   \param  radioConfig   The radio configuration to load.
*/
IRIS_DLL bool IRISLoadRadio(std::string radioConfig);

/// Start the radio
IRIS_DLL bool IRISStartRadio();

/// Stop the radio
IRIS_DLL bool IRISStopRadio();

/// Unload the radio
IRIS_DLL bool IRISUnloadRadio();

/// Check whether the radio is loaded
IRIS_DLL bool IRISisRadioLoaded();

/// Check whether the radio is running
IRIS_DLL bool IRISisRadioRunning();

/// Check whether the radio is suspended
IRIS_DLL bool IRISisRadioSuspended();

/** Reconfigure the radio
 *
 * @param radioConfig   The new radio configuration
 * @return  Was the reconfiguration successful?
 */
IRIS_DLL bool IRISReconfigureRadio(std::string radioConfig);

#endif //IRIS_IRIS_H_



