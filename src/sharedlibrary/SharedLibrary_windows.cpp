/**
 * @file SharedLibrary_windows.cpp
 * @version 1.0
 *
 * @section COPYRIGHT
 *
 * Copyright 2012 The Iris Project Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution
 * and at http://www.softwareradiosystems.com/iris/copyright.html.
 *
 * @section LICENSE
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
 * @section DESCRIPTION
 *
 * Implementation of SharedLibrary class for Windows systems (using LoadLibrary)
 */

#include <sstream>
#include <iostream>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    //need to define the following to avoid socket and macro issues with windows.h
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
#endif

#include "windows.h"

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
        if (FreeLibrary(d_library) != 0)
        {
            // there should normally be no error on close
            // -> So we just output a message in that case
            LOG(LERROR) << "Error: could not close library " << d_filename;
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

    //Need a wide character string for LoadLibrary when compiling with UNICODE
    size_t origsize = strlen(system_filename) + 1;
    const size_t newsize = 1024;
    size_t convertedChars = 0;
    wchar_t wcstring[newsize];
    mbstowcs_s(&convertedChars, wcstring, origsize, system_filename, _TRUNCATE);

    //Load the library
    d_library = LoadLibrary(wcstring);

    if (d_library == NULL)
    {
        std::stringstream sstr;
        sstr << "An error occurred during load of library " << filename;
        throw LibraryLoadException(sstr.str());
    }

    d_filename = filename;
}

SharedLibrary::SymbolPointer
SharedLibrary::getSymbol(std::string symbolName) throw (LibrarySymbolException)
{
    SymbolPointer tmp = GetProcAddress(d_library, symbolName.c_str());

    if (tmp == NULL)
    {
        std::stringstream sstr;
        sstr << "Could not resolve symbol " << symbolName << " in library " << d_filename;
        throw LibrarySymbolException(sstr.str());
    }

    return tmp;
}

SharedLibrary::~SharedLibrary()
{
    if (d_library == NULL)
        return;
    if (FreeLibrary(d_library) == 0)
    {
        // cannot throw an exception here, but there should normally be no error on close
        // -> So we just output a message in that case
        LOG(LERROR) << "Error: could not close library " << d_filename;
    }
}


}  // end of iris namespace
