/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file LinkDescription.h
 * Define a link between components of a radio flowgraph
 *
 *  Created on: 13-Mar-2009
 *  Created by: suttonp
 *  $Revision: 712 $
 *  $LastChangedDate: 2009-10-20 19:28:06 +0100 (Tue, 20 Oct 2009) $
 *  $LastChangedBy: lotzej $
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
