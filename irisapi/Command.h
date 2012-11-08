/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file Command.h
 * The command object used to pass commands within the IRIS2.0 system
 *
 *  Created on: 17-May-2011
 *  Created by: suttonp
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <vector>
#include <boost/any.hpp>

namespace iris
{

struct Command
{
	//! the data passed with the command
	std::vector<boost::any> data;

	//! type of data passed with command
	int typeId;

	//! name of this command
    std::string commandName;

	//! name of component targeted by this command
	std::string componentName;

	//! name of engine targeted by this command
	std::string engineName;
};


} /* namespace iris */

#endif /* COMMAND_H_ */

