/*
 * This file is part of Iris 2.
 *
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin.
 * All rights reserved.
 *
 */

/*
 * file TestLibrary.cpp
 * A simple shared library used by SharedLibrary_test.cpp
 *
 *  Created on: 21-Nov-2011
 *  Created by: suttonp
 *  $Revision: 1377 $
 *  $LastChangedDate: 2011-11-22 18:40:00 +0000 (Tue, 22 Nov 2011) $
 *  $LastChangedBy: suttonp $
 */

// Platform-dependent library declaration
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#   define EXPORT_DECLSPEC   __declspec (dllexport)
#else
#   define EXPORT_DECLSPEC
#endif // _WIN32

#include <string>
#include "TestLibrary.h"

using namespace std;

namespace iris
{

extern "C" EXPORT_DECLSPEC string GetName()
{
	return "TestLibrary";
}

TestLibrary::TestLibrary()
{}

} /* namespace iris */
