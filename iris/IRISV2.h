/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

#ifndef _IRISV2_H
#define _IRISV2_H

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

//! Initialize the system
IRIS_DLL void IRISInitSystem();

//! Set the Stack component repository
IRIS_DLL bool IRISSetStackRepository(std::string rep);

//! Set the PN component repository
IRIS_DLL bool IRISSetPnRepository(std::string rep);

//! Set the SDF component repository
IRIS_DLL bool IRISSetSdfRepository(std::string rep);

//! Set the controller repository
IRIS_DLL bool IRISSetContRepository(std::string rep);

//! Set the log level
IRIS_DLL bool IRISSetLogLevel(std::string level);

/*!
*   \brief Load the radio
*
*   \param  radioConfig     The radio configuration to load
*/
IRIS_DLL bool IRISLoadRadio(std::string radioConfig);

//! Start the radio
IRIS_DLL bool IRISStartRadio();

//! Stop the radio
IRIS_DLL bool IRISStopRadio();

//! Unload the radio
IRIS_DLL bool IRISUnloadRadio();

//! Check whether the radio is loaded
IRIS_DLL bool IRISisRadioLoaded();

//! Check whether the radio is running
IRIS_DLL bool IRISisRadioRunning();

//! Check whether the radio is suspended
IRIS_DLL bool IRISisRadioSuspended();

//! Reconfigure the radio
IRIS_DLL bool IRISReconfigureRadio(std::string radioConfig);

#endif //_IRISV2_H


