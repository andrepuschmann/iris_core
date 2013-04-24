/**
 * \file System.cpp
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
 * Implementation of the System class - the core class of
 * the Iris architecture.
 */

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "irisapi/Logging.h"
#include "iris/System.h"
#include "iris/XmlParser.h"
#include "iris/ReconfigurationManager.h"

using namespace std;
using namespace boost::filesystem;

namespace iris
{
    //ctor
    System::System()
        :status_(RADIOUNLOADED),
        pFile_(NULL)
    {
        //Set up logging
        pFile_ = fopen("iris2.log", "a+");
        if(pFile_ != NULL)
            LoggingPolicy::getPolicyInstance()->setFileStream(pFile_);

        LoggingPolicy::getPolicyInstance()->ReportingLevel() = LDEBUG;
    }

    System::~System()
    {
        if(pFile_ != NULL)
            fclose(pFile_);
    }

    void System::setStackRepository(std::string rep)
    {
        reps_.stackRepository = rep;
    }

    void System::setPhyRepository(std::string rep)
    {
        reps_.phyRepository = rep;
    }

    void System::setSdfRepository(std::string rep)
    {
        reps_.sdfRepository = rep;
    }

    void System::setContRepository(std::string rep)
    {
        reps_.contRepository = rep;
    }

    void System::setLogLevel(std::string level)
    {
        boost::to_lower(level);
        if(level == "debug")
        {
            LoggingPolicy::getPolicyInstance()->ReportingLevel() = LDEBUG;
        }
        else if(level == "info")
        {
            LoggingPolicy::getPolicyInstance()->ReportingLevel() = LINFO;
        }
        else if(level == "warning")
        {
            LoggingPolicy::getPolicyInstance()->ReportingLevel() = LWARNING;
        }
        else if(level == "error")
        {
            LoggingPolicy::getPolicyInstance()->ReportingLevel() = LERROR;
        }
        else if(level == "fatal")
        {
            LoggingPolicy::getPolicyInstance()->ReportingLevel() = LFATAL;
        }
    }

    bool System::loadRadio(std::string radioConfig)
    {
        RadioRepresentation rad;
        bool result = false;

        //Can only load if current status is RADIOUNLOADED
        switch(status_)
        {
        case RADIOUNLOADED:
            LOG(LINFO) << "Loading radio: " << radioConfig;

            try{
                XmlParser::parseXmlFile(radioConfig, rad);
                engineManager_.setRepositories(reps_);
                engineManager_.loadRadio(rad);
                status_ = RADIOLOADED;
                result = true;
            }
            catch(std::exception& ex)
            {
                LOG(LFATAL) << "Error loading radio: " << ex.what();
            }
            break;
        default:
            LOG(LWARNING) << "A radio is already loaded";
            break;
        }

        return result;
    }

    bool System::startRadio()
    {
        bool result = false;

        //Can only start if current status is RADIOLOADED
        switch(status_)
        {
        case RADIOUNLOADED:
            LOG(LWARNING) << "There is no radio loaded";
            break;
        case RADIOLOADED:
            LOG(LINFO) << "Starting radio";

            try{
                engineManager_.startRadio();
                status_ = RADIORUNNING;
                result = true;
            }
            catch(std::exception& ex)
            {
                LOG(LFATAL) << "Error starting radio: " << ex.what();
            }
            break;
        default:
            LOG(LWARNING) << "A radio is already started";
            break;
        }

        return result;
    }

    bool System::stopRadio()
    {
        bool result = false;

        //Can only stop if current status is RADIORUNNING
        switch(status_)
        {
        case RADIOUNLOADED:
            LOG(LWARNING) << "There is no radio loaded";
            break;
        case RADIOLOADED:
            LOG(LWARNING) << "The radio is not running";
            break;
        case RADIORUNNING:
            LOG(LINFO) << "Stopping radio";

            try{
                engineManager_.stopRadio();
                status_ = RADIOLOADED;
                result = true;
            }
            catch(std::exception& ex)
            {
                LOG(LFATAL) << "Error stopping radio: " << ex.what();
            }
            break;
        default:
            LOG(LWARNING) << "A radio is already started";
            break;
        }

        return result;
    }

    bool System::unloadRadio()
    {
        bool result = false;

        //Can only unload if current status is RADIOLOADED
        switch(status_)
        {
        case RADIOUNLOADED:
            LOG(LWARNING) << "There is no radio loaded";
            break;
        case RADIOLOADED:
            LOG(LINFO) << "Unloading radio";

            try{
                engineManager_.unloadRadio();
                status_ = RADIOUNLOADED;
                result = true;
            }
            catch(std::exception& ex)
            {
                LOG(LFATAL) << "Error unloading radio: " << ex.what();
            }
            break;
        default:
            LOG(LWARNING) << "The radio has not been stopped";
            break;
        }

        return result;
    }

    bool System::reconfigureRadio(std::string radioConfig)
    {
        RadioRepresentation rad;
        bool result = false;

        //Can only load if current status is RADIOUNLOADED
        switch(status_)
        {
        case RADIOUNLOADED:
            LOG(LWARNING) << "No radio has been loaded - loading new configuration";
            result = loadRadio(radioConfig);
            break;
        default:
            LOG(LINFO) << "Reconfiguring radio: " << radioConfig;

            try{
                XmlParser::parseXmlFile(radioConfig, rad);
                ReconfigSet reconfigs = ReconfigurationManager::compareRadios(engineManager_.getCurrentRadio(), rad);
                engineManager_.reconfigureRadio(reconfigs);
                result = true;
            }
            catch(std::exception& ex)
            {
                LOG(LERROR) << "Error reconfiguring radio: " << ex.what();
            }
            break;
        }

        return result;
    }

    bool System::isRadioLoaded() const
    {
        return status_ == RADIOLOADED;
    }

    bool System::isRadioRunning() const
    {
        return status_ == RADIORUNNING;
    }

    bool System::isRadioSuspended() const
    {
        return status_ == RADIOSUSPENDED;
    }


} /* namespace iris */
