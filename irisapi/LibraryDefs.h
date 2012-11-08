/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file LibraryDefs.h
 * Platform-dependent preprocessor defs required for exporting libraries.
 *
 *  Created on: 26-Mar-2009
 *  Created by: suttonp
 *  $Revision: 712 $
 *  $LastChangedDate: 2009-10-20 19:28:06 +0100 (Tue, 20 Oct 2009) $
 *  $LastChangedBy: lotzej $
 *
 */

#ifndef LIBRARYDEFS_H_
#define LIBRARYDEFS_H_

// Platform-dependent library declaration
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#   define EXPORT_DECLSPEC   __declspec (dllexport)
#else
#   define EXPORT_DECLSPEC
#endif // _WIN32

#endif // LIBRARYDEFS_H_

