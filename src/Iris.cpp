/**
 * @file Iris.cpp
 * @version 1.0
 *
 * @section COPYRIGHT
 *
 * Copyright 2012 The Iris Project Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution
 * and at http://www.softwareradiosystems.com/iris/copyright.html.
 *
 * @section LICENSE
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
 * @section DESCRIPTION
 *
 * Implementation of the top-level C interface to Iris defined in Iris.h.
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


