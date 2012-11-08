/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file EngineManager.h
 *
 *  Created on: 12-Jan-2008
 *  Created by: sutttonp
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
 *
 */

#ifndef ENGINEMANAGER_H_
#define ENGINEMANAGER_H_

#include <boost/graph/topological_sort.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>   //Autodeleting, Exception-safe container of pointers
#include "boost/filesystem.hpp"

#include "iris/RadioRepresentation.h"
#include "iris/PNEngine.h"
#include "iris/ControllerManager.h"
#include "irisapi/Exceptions.h"
#include "irisapi/Logging.h"

namespace iris
{
    //! A container for component repository strings
    struct Repositories
    {
		std::string stackRepository;
        std::string pnRepository;
        std::string sdfRepository;
		std::string contRepository;
    };

    /*!
     *  \brief The EngineManager controls all engines running within the IRIS architecture.
     */
	class EngineManager:public EngineCallbackInterface, public ControllerManagerCallbackInterface
    {
    private:
		//! The ControllerManager for this radio
		ControllerManager d_controllerManager;

        //! The engines of the current radio
        boost::ptr_vector<EngineInterface> d_engines;

        //! The paths to the radio component repositories
        Repositories d_reps;

        //! A graph representing each of the radio engines and the links between them
        EngineGraph d_engineGraph;

        //! The current radio representation - this is updated with any reconfigurations which occur
        RadioRepresentation d_radioRep;

        /*!
        *   \brief Create an engine
        *
        *   \param  d    An EngineDescription containing the details of the engine to be created
        *
        *   \returns A pointer to the engine which is created
        */
        EngineInterface* createEngine(EngineDescription& d) throw (ResourceNotFoundException);

        /*!
        *   \brief Check are both links equivalent
        *
        *   \param  first    The first link to be compared
        *   \param  second   The second link to be compared
        *
        *   \returns Whether the links are equivalent
        */
        bool sameLink(LinkDescription first, LinkDescription second);
	public:

        EngineManager();
        void setRepositories(Repositories reps){d_reps = reps;}
        Repositories getRepositories() const {return d_reps;}
        void loadRadio(RadioRepresentation rad);
        void startRadio();
        void stopRadio();
        void unloadRadio();
        RadioRepresentation& getCurrentRadio();
        void reconfigureRadio(ReconfigSet reconfigs);
        void postCommand(Command command);
        std::string getParameterValue(std::string paramName, std::string componentName);
		void activateEvent(Event &e);
    };

} /* namespace iris */

#endif /* ENGINEMANAGER_H_ */
