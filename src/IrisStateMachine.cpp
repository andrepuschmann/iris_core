/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

#include "IrisStateMachine.h"
#include <iostream>

#include "iris/IRISV2.h"

Active::Active(my_context ctx) : my_base(ctx)
{
	std::string stackRadioRepository = context<IrisStateMachine>().getStackRadioRepository();
    std::string pnRadioRepository = context<IrisStateMachine>().getPnRadioRepository();
    std::string sdfRadioRepository = context<IrisStateMachine>().getSdfRadioRepository();
	std::string contRadioRepository = context<IrisStateMachine>().getContRadioRepository();
	std::string logLevel = context<IrisStateMachine>().getLogLevel();
    IRISInitSystem();
	IRISSetStackRepository(stackRadioRepository);
    IRISSetPnRepository(pnRadioRepository);
    IRISSetSdfRepository(sdfRadioRepository);
	IRISSetContRepository(contRadioRepository);
	IRISSetLogLevel(logLevel);
}

Loaded::Loaded(my_context ctx) throw (LauncherException)
    : my_base(ctx) 
{
	std::string radioConfig = context<IrisStateMachine>().getRadioConfig();
    if(!IRISLoadRadio(radioConfig))
        throw LauncherException("Failed to load radio - exiting");
}

void Loaded::exit() throw (LauncherException)
{
    if(!IRISUnloadRadio())
        throw LauncherException("Failed to unload radio - exiting");
}

Running::Running() throw (LauncherException)
{
    if(!IRISStartRadio())
        throw LauncherException("Failed to start radio - exiting");
}

void Running::exit() throw (LauncherException)
{
    if(!IRISStopRadio())
        throw LauncherException("Failed to stop radio - exiting");
}

void IrisStateMachine::reconfigureRadio()
{
    std::string radioConfig = context<IrisStateMachine>().getRadioConfig();
    IRISReconfigureRadio(radioConfig);
}

