/**
 * @file System.h
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
 * System is the core part of the Iris architecture. It uses the other elements of the
 * architecture to run and reconfigure radios.
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
