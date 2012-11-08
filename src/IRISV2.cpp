/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

#include <boost/scoped_ptr.hpp>

#include "iris/IRISV2.h"
#include "iris/System.h"

using namespace std;
using namespace iris;

//Using scoped pointer to ensure deletion on exit
boost::scoped_ptr< System > theSystem;

string getName()
{   
    return "IRISV2";
}

void IRISInitSystem()
{
    if(theSystem == NULL)
    {
        //Create a new IRISv2 system
        theSystem.reset(new System());
    }else{
        //System already exists - only support one instance at a time
        LOG(LWARNING) << "System already initialized - only one instance permitted.";
    }
}

bool IRISSetStackRepository(std::string rep)
{
    if(theSystem == NULL)
    {
        LOG(LERROR) << "System has not been initialized.";
        return false;
    }else{
        theSystem->setStackRepository(rep);
        return true;
    }
}

bool IRISSetPnRepository(std::string rep)
{
    if(theSystem == NULL)
    {
        LOG(LERROR) << "System has not been initialized.";
        return false;
    }else{
        theSystem->setPnRepository(rep);
        return true;
    }
}

bool IRISSetSdfRepository(std::string rep)
{
    if(theSystem == NULL)
    {
        LOG(LERROR) << "System has not been initialized.";
        return false;
    }else{
        theSystem->setSdfRepository(rep);
        return true;
    }
}

bool IRISSetContRepository(std::string rep)
{
    if(theSystem == NULL)
    {
        LOG(LERROR) << "System has not been initialized.";
        return false;
    }else{
        theSystem->setContRepository(rep);
        return true;
    }
}

bool IRISSetLogLevel(std::string level)
{
    if(theSystem == NULL)
    {
        LOG(LERROR) << "System has not been initialized.";
        return false;
    }else{
        theSystem->setLogLevel(level);
        return true;
    }
}

bool IRISLoadRadio(std::string radioConfig)
{
    if(theSystem == NULL)
    {
        LOG(LERROR) << "System has not been initialized.";
        return false;
    }else{
        return theSystem->loadRadio(radioConfig);
    }
}

bool IRISStartRadio()
{
    if(theSystem == NULL)
    {
        LOG(LERROR) << "System has not been initialized.";
        return false;
    }else{
        return theSystem->startRadio();
    }
}

bool IRISStopRadio()
{
    if(theSystem == NULL)
    {
        LOG(LERROR) << "System has not been initialized.";
        return false;
    }else{
        return theSystem->stopRadio();
    }
}

bool IRISUnloadRadio()
{
    if(theSystem == NULL)
    {
        LOG(LERROR) << "System has not been initialized.";
        return false;
    }else{
        return theSystem->unloadRadio();
    }
}

bool IRISisRadioLoaded()
{
    if(theSystem == NULL)
    {
        LOG(LERROR) << "System has not been initialized.";
        return false;
    }else{
        return theSystem->isRadioLoaded();
    }
}

bool IRISisRadioRunning()
{
    if(theSystem == NULL)
    {
        LOG(LERROR) << "System has not been initialized.";
        return false;
    }else{
        return theSystem->isRadioRunning();
    }
}

bool IRISisRadioSuspended()
{
    if(theSystem == NULL)
    {
        LOG(LERROR) << "System has not been initialized.";
        return false;
    }else{
        return theSystem->isRadioSuspended();
    }
}

bool IRISReconfigureRadio(std::string radioConfig)
{
    if(theSystem == NULL)
    {
        LOG(LERROR) << "System has not been initialized.";
        return false;
    }else{
        return theSystem->reconfigureRadio(radioConfig);
    }
}


