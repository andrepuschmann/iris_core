/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file System.h
 *
 *  Created on: 12-Jan-2008
 *  Created by: sutttonp
 *  $Revision: 1223 $
 *  $LastChangedDate: 2011-02-17 16:12:19 +0000 (Thu, 17 Feb 2011) $
 *  $LastChangedBy: suttonp $
 *
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "irisapi/Exceptions.h"
#include "iris/EngineManager.h"

namespace iris
{

    //! The radio status
    enum RadioStatus {RADIOUNLOADED, RADIOLOADED, RADIORUNNING, RADIOSUSPENDED };

    /*! \class System
     *  \brief This is the core part of the IRISv2 architecture. It uses the other elements of the
     *  architecture to run and reconfigure radios.
     */
    class System
    {
    private:
        //! The EngineManager controls all engines running within IRISv2
        EngineManager d_engineManager;

        //! The current radio status
        RadioStatus d_status;

        //! The repositories
        Repositories d_reps;

        //! Pointer to the log file
        FILE* pFile;

	public:
        //! ctor
		System();

        //! dtor
        ~System();

		//! Set the Stack Component repository
        void setStackRepository(std::string rep);

        //! Set the PN Component repository
        void setPnRepository(std::string rep);

        //! Set the SDF Component repository
        void setSdfRepository(std::string rep);

		//! Set the Controller repository
        void setContRepository(std::string rep);

        //! Set the log level
        void setLogLevel(std::string level);

        //! Load a radio given a configuration file name
        bool loadRadio(std::string radioConfig);

        //! Start a loaded radio
        bool startRadio();

        //! Stop a running radio
        bool stopRadio();

        //! Unload a loaded radio
        bool unloadRadio();

        //! Reconfigure the radio
        bool reconfigureRadio(std::string radioConfig);

        bool isRadioLoaded() const;

        bool isRadioRunning() const;

        bool isRadioSuspended() const;

        std::string getName() const
        {   return "System"; };
    };

} /* namespace iris */

#endif /* SYSTEM_H_ */
