/*
 * This file is part of Iris 2.
 *
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin.
 * All rights reserved.
 *
 */

/**
 * \file Version.h
 * A simple version number system to ensure core and modules are built with the same API
 *
 *  Created on: 07-Aug-2009
 *  Created by: suttonp
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
 */

#ifndef VERSION_H_
#define VERSION_H_

namespace iris
{
	class Version
	{
	public:
		static const char* getApiVersion()
		{
			return "0.0.16";
		};

	};

} /* namespace iris */

#endif /* VERSION_H_ */

