/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file EngineInterface.h
 * Interface implemented by all IRIS engines
 *
 *  Created on: 2-Jan-2009
 *  Created by: suttonp
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
 *
 */

#ifndef ENGINEINTERFACE_H_
#define ENGINEINTERFACE_H_

#include <boost/shared_ptr.hpp>

#include "iris/RadioRepresentation.h"
#include "iris/EngineCallbackInterface.h"
#include "irisapi/ReconfigurationDescriptions.h"
#include "irisapi/Command.h"
#include "irisapi/DataBufferInterfaces.h"

namespace iris
{

/** The EngineInterface interface is implemented by all engines within the IRIS framework
*
*/
class EngineInterface{
public:
    virtual ~EngineInterface(){};
	virtual void setEngineManager(EngineCallbackInterface *e) =0;
	virtual std::vector< boost::shared_ptr< DataBufferBase > > loadEngine(EngineDescription eng, std::vector< boost::shared_ptr< DataBufferBase > > inputLinks) = 0;
    virtual void unloadEngine() = 0;
    virtual void startEngine() = 0;
    virtual void stopEngine() = 0;
    virtual std::string getName() const = 0;
    virtual void addReconfiguration(ReconfigSet reconfigs) = 0;
    virtual void postCommand(Command command) = 0;

};

} /* namespace iris */

#endif /* ENGINEINTERFACE_H_ */
