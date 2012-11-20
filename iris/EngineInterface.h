/**
 * \file EngineInterface.h
 * \version 1.0
 *
 * \section COPYRIGHT
 *
 * Copyright 2012 The Iris Project Developers. See the
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
 * Interface implemented by all IRIS engines
 */

#ifndef IRIS_ENGINEINTERFACE_H_
#define IRIS_ENGINEINTERFACE_H_

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

} // namespace iris

#endif // IRIS_ENGINEINTERFACE_H_
