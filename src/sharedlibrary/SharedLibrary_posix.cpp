/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file SharedLibrary_posix.cpp
 * Implementation of SharedLibrary class for POSIX systems (using dlopen)
 *
 *  Created on: 14-Mar-2009
 *  Created by: lotzej
 *  $Revision: 1377 $
 *  $LastChangedDate: 2011-11-22 18:40:00 +0000 (Tue, 22 Nov 2011) $
 *  $LastChangedBy: suttonp $
 */

#include <dlfcn.h>
#include <sstream>
#include <iostream>

#include "iris/SharedLibrary.h"
#include "irisapi/Logging.h"

namespace iris
{

SharedLibrary::SharedLibrary(boost::filesystem::path filename) throw (LibraryLoadException,
        FileNotFoundException) :
    d_filename(filename), d_library(NULL)
{
    this->open(filename);
}

void
SharedLibrary::open(boost::filesystem::path filename) throw (LibraryLoadException,
                FileNotFoundException)
{
    if (d_library != NULL)
    {
        if (dlclose(d_library) != 0)
        {
            // there should normally be no error on close
            // -> So we just output a message in that case
            LOG(LERROR) << "Error: could not close library " << d_filename << ". Message: " << dlerror()
                    << std::endl;

        }
        d_library = NULL;
        d_filename = "";
    }

    using namespace boost::filesystem;

    if (!exists(filename))
    {
        std::stringstream sstr;
        sstr << "The library " << filename << " could not be found.";
        throw FileNotFoundException(sstr.str());
    }

    d_filename = filename;
    std::string fString = d_filename.file_string();
    const char* system_filename = fString.c_str();

    d_library = dlopen(system_filename, RTLD_NOW | RTLD_GLOBAL);

    if (d_library == NULL)
    {
        std::stringstream sstr;
        sstr << "An error occurred during load of library " << filename << ": " << dlerror();
        throw LibraryLoadException(sstr.str());
    }

    d_filename = filename;
}

SharedLibrary::SymbolPointer
SharedLibrary::getSymbol(std::string symbolName) throw (LibrarySymbolException)
{
    void* tmp = dlsym(d_library, symbolName.c_str());

    if (tmp == NULL)
    {
        std::stringstream sstr;
        sstr << "Could not resolve symbol " << symbolName << " in library " << d_filename
                << ". Error: " << dlerror();
        throw LibrarySymbolException(sstr.str());
    }

    return tmp;
}

SharedLibrary::~SharedLibrary()
{
    if (d_library == NULL)
        return;
    if (dlclose(d_library) != 0)
    {
        // cannot throw an exception here, but there should normally be no error on close
        // -> So we just output a message in that case
        LOG(LERROR) << "Error: could not close library " << d_filename << ". Message: " << dlerror()
                << std::endl;
    }
}


}  // end of iris namespace
