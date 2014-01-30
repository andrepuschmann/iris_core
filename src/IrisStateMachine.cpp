/**
 * \file IrisStateMachine.cpp
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
 * Implementation of the state machine for the Iris Launcher using
 * the boost statechart library.
 */

#include "IrisStateMachine.h"
#include <iostream>

#include "iris/Iris.h"

Active::Active(my_context ctx) : my_base(ctx)
{
    std::string stackRadioRepository = context<IrisStateMachine>().getStackRadioRepository();
    std::string phyRadioRepository = context<IrisStateMachine>().getPhyRadioRepository();
    std::string sdfRadioRepository = context<IrisStateMachine>().getSdfRadioRepository();
    std::string contRadioRepository = context<IrisStateMachine>().getContRadioRepository();
    std::string logLevel = context<IrisStateMachine>().getLogLevel();
    std::string loggingFile = context<IrisStateMachine>().getLoggingFile();
    IRISInitSystem();
    IRISSetStackRepository(stackRadioRepository);
    IRISSetPhyRepository(phyRadioRepository);
    IRISSetSdfRepository(sdfRadioRepository);
    IRISSetContRepository(contRadioRepository);
    IRISSetLogLevel(logLevel);
    IRISSetLoggingFile(loggingFile);
}

Loaded::Loaded(my_context ctx)
    : my_base(ctx) 
{
    std::string radioConfig = context<IrisStateMachine>().getRadioConfig();
    if(!IRISLoadRadio(radioConfig))
        throw LauncherException("Failed to load radio - exiting");
}

void Loaded::exit()
{
    if(!IRISUnloadRadio())
        throw LauncherException("Failed to unload radio - exiting");
}

Running::Running()
{
    if(!IRISStartRadio())
        throw LauncherException("Failed to start radio - exiting");
}

void Running::exit()
{
    if(!IRISStopRadio())
        throw LauncherException("Failed to stop radio - exiting");
}

void IrisStateMachine::reconfigureRadio()
{
    std::string radioConfig = context<IrisStateMachine>().getRadioConfig();
    IRISReconfigureRadio(radioConfig);
}

