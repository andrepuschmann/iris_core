/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file System.cpp
 * Implementation of System class.
 *
 *  Created on: 12-jan-2008
 *  Created by: suttonp
 *  $Revision: 1316 $
 *  $LastChangedDate: 2011-09-13 12:41:16 +0100 (Tue, 13 Sep 2011) $
 *  $LastChangedBy: suttonp $
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
        :d_status(RADIOUNLOADED),
        pFile(NULL)
    {
        //Set up logging
        pFile = fopen("iris2.log", "a+");
        if(pFile != NULL)
            LoggingPolicy::getPolicyInstance()->setFileStream(pFile);

        LoggingPolicy::getPolicyInstance()->ReportingLevel() = LDEBUG;
    }

    System::~System()
    {
        if(pFile != NULL)
            fclose(pFile);
    }

	void System::setStackRepository(std::string rep)
    {
        d_reps.stackRepository = rep;
    }

    void System::setPnRepository(std::string rep)
    {
        d_reps.pnRepository = rep;
    }

    void System::setSdfRepository(std::string rep)
    {
        d_reps.sdfRepository = rep;
    }

	void System::setContRepository(std::string rep)
    {
        d_reps.contRepository = rep;
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
        switch(d_status)
        {
        case RADIOUNLOADED:
            LOG(LINFO) << "Loading radio: " << radioConfig;

            try{
                XmlParser::parseXmlFile(radioConfig, rad);
                d_engineManager.setRepositories(d_reps);
                d_engineManager.loadRadio(rad);
                d_status = RADIOLOADED;
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
        switch(d_status)
        {
        case RADIOUNLOADED:
            LOG(LWARNING) << "There is no radio loaded";
            break;
        case RADIOLOADED:
            LOG(LINFO) << "Starting radio";

            try{
                d_engineManager.startRadio();
                d_status = RADIORUNNING;
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
        switch(d_status)
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
                d_engineManager.stopRadio();
                d_status = RADIOLOADED;
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
        switch(d_status)
        {
        case RADIOUNLOADED:
            LOG(LWARNING) << "There is no radio loaded";
            break;
        case RADIOLOADED:
            LOG(LINFO) << "Unloading radio";

            try{
                d_engineManager.unloadRadio();
                d_status = RADIOUNLOADED;
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
        switch(d_status)
        {
        case RADIOUNLOADED:
            LOG(LWARNING) << "No radio has been loaded - loading new configuration";
            result = loadRadio(radioConfig);
            break;
        default:
            LOG(LINFO) << "Reconfiguring radio: " << radioConfig;

            try{
                XmlParser::parseXmlFile(radioConfig, rad);
                ReconfigSet reconfigs = ReconfigurationManager::compareRadios(d_engineManager.getCurrentRadio(), rad);
                d_engineManager.reconfigureRadio(reconfigs);
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
        return d_status == RADIOLOADED;
    }

    bool System::isRadioRunning() const
    {
        return d_status == RADIORUNNING;
    }

    bool System::isRadioSuspended() const
    {
        return d_status == RADIOSUSPENDED;
    }


} /* namespace iris */
