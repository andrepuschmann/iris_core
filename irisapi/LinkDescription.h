/**
 * \file LinkDescription.h
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
 * Define a link between components of a radio flowgraph.
 */

#ifndef LINKDESCRIPTION_H_
#define LINKDESCRIPTION_H_

#include <string>
#include <boost/shared_ptr.hpp>

namespace iris{

  //Forward declaration 
  class DataBufferBase;

  //! A link between two components
  struct LinkDescription
  {
    boost::shared_ptr< DataBufferBase > theBuffer;
    std::string sourceEngine;
    std::string sinkEngine;
    std::string sourceComponent;
    std::string sinkComponent;
    std::string sourcePort;
    std::string sinkPort;

    bool operator==(const LinkDescription& link) const
    {
      return (sourceEngine == link.sourceEngine && sinkEngine == link.sinkEngine &&
        sourceComponent == link.sourceComponent && sinkComponent == link.sinkComponent &&
        sourcePort == link.sourcePort && sinkPort == link.sinkPort);   
    }
  };

} /* namespace iris */

#endif /* LINKDESCRIPTION_H_ */
