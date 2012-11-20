/**
 * \file SharedLibrary_posix.cpp
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
 * Implementation of SharedLibrary class for POSIX systems (using dlopen)
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
    filename_(filename), library_(NULL)
{
    this->open(filename);
}

void
SharedLibrary::open(boost::filesystem::path filename) throw (LibraryLoadException,
                FileNotFoundException)
{
    if (library_ != NULL)
    {
        if (dlclose(library_) != 0)
        {
            // there should normally be no error on close
            // -> So we just output a message in that case
            LOG(LERROR) << "Error: could not close library " << filename_ << ". Message: " << dlerror()
                    << std::endl;

        }
        library_ = NULL;
        filename_ = "";
    }

    using namespace boost::filesystem;

    if (!exists(filename))
    {
        std::stringstream sstr;
        sstr << "The library " << filename << " could not be found.";
        throw FileNotFoundException(sstr.str());
    }

    filename_ = filename;
    std::string fString = filename_.string();
    const char* system_filename = fString.c_str();

    library_ = dlopen(system_filename, RTLD_NOW | RTLD_GLOBAL);

    if (library_ == NULL)
    {
        std::stringstream sstr;
        sstr << "An error occurred during load of library " << filename << ": " << dlerror();
        throw LibraryLoadException(sstr.str());
    }

    filename_ = filename;
}

SharedLibrary::SymbolPointer
SharedLibrary::getSymbol(std::string symbolName) throw (LibrarySymbolException)
{
    void* tmp = dlsym(library_, symbolName.c_str());

    if (tmp == NULL)
    {
        std::stringstream sstr;
        sstr << "Could not resolve symbol " << symbolName << " in library " << filename_
                << ". Error: " << dlerror();
        throw LibrarySymbolException(sstr.str());
    }

    return tmp;
}

SharedLibrary::~SharedLibrary()
{
    if (library_ == NULL)
        return;
    if (dlclose(library_) != 0)
    {
        // cannot throw an exception here, but there should normally be no error on close
        // -> So we just output a message in that case
        LOG(LERROR) << "Error: could not close library " << filename_ << ". Message: " << dlerror()
                << std::endl;
    }
}


}  // end of iris namespace
