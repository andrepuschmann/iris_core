/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file SharedLibrary.h
 * Class for managing shared libraries
 *
 *  Created on: 08-Mar-2009
 *  Created by: lotzej
 *  $Revision: 1377 $
 *  $LastChangedDate: 2011-11-22 18:40:00 +0000 (Tue, 22 Nov 2011) $
 *  $LastChangedBy: suttonp $
 */

#ifndef _IRIS_SHAREDLIBRARY_H
#define _IRIS_SHAREDLIBRARY_H

#include <boost/filesystem.hpp>
#include <boost/config.hpp>
#include <boost/utility.hpp>
#include <string>

#ifdef BOOST_WINDOWS
#include <windows.h>
#endif

#include "irisapi/Exceptions.h"

namespace iris
{

    /** \brief Manages shared libraries in a portable fashion.
     *
     * Multiple implementations of this class must be provided for each platform with
     * different native functions. Instances of this class cannot be copied because
     * the library is closed in the destructor and no reference counting is performed.
     *
     * Make sure that no symbols or objects from within the library are open and used
     * when the object of this class is destructed (i.e., when the library is unloaded
     * from the application).
     */
    class SharedLibrary : boost::noncopyable
    {
    public:

#ifdef BOOST_WINDOWS
            typedef FARPROC SymbolPointer;   //!< Type that can hold a pointer to a library symbol
#else
            typedef void* SymbolPointer;     //!< Type that can hold a pointer to a library symbol
#endif

        // ---------- standard usage

        /** Constructs a SharedLibrary instance and loads a shared library.
         *
         * @param filename Path to the library file.
         * @throw LibraryLoadException If an error occurred during library loading.
         * @throw FileNotFoundException If the given file could not be found.
         */
        SharedLibrary(boost::filesystem::path filename) throw (LibraryLoadException,
            FileNotFoundException);

        /** Gets the address of a symbol from the dynamic library.
         *
         * @param symbolName Name of the symbol (function or variable) to search for
         * @return Return value should be cast to the function prototype pointer or variable pointer
         * @throw LibrarySymbolException If an error occurred during resolution of the symbol name.
         */
        SymbolPointer getSymbol(std::string symbolName) throw (LibrarySymbolException);

        /** Destructor. Closes the library.    */
        ~SharedLibrary();

        std::string getName(){return "SharedLibrary";}



        // ---------- Usage without giving filename to constructor

        //! Default constructor. Does not open a library.
        SharedLibrary() : d_library(NULL) {};

        //! \brief Opens a shared library.
        //! If a different library is already open and held by this object, it is closed
        //! automatically before opening the new one. See constructor for description
        //! of parameters.
        void open(boost::filesystem::path filename) throw (LibraryLoadException,
                FileNotFoundException);

        //! Check whether the library has been loaded
        bool isLoaded() {return d_library == NULL;};

        // ----------- Accessors

        //! retrieves filename of currently open file
        boost::filesystem::path getFilename() const {  return d_filename; }

        // ----------- Information

        //! Returns the standard system extension for shared libraries.
        //! That is, in Windows, returns "dll", Linux: "so", Mac: "dylib", etc.
        static std::string getSystemExtension()
        {
			#if defined(_WIN32) || defined(__CYGWIN__)
				return ".dll";
			#elif defined(__unix__)
				return ".so";
			#elif defined(__APPLE__)
				return ".dylib";
			#endif
        }

        //! Returns the standard system prefix for shared libraries.
		//! That is, in Windows, returns "", Linux: "lib", Mac: "", etc.
		static std::string getSystemPrefix()
		{
			#if defined(_WIN32) || defined(__CYGWIN__)
				return "";
			#elif defined(__unix__)
				return "lib";
			#elif defined(__APPLE__)
				return "";
			#endif
		}

    private:

#ifdef BOOST_WINDOWS
        typedef HMODULE LibraryHandle;  //!< Handle for library
#else
        typedef void* LibraryHandle;    //!< Handle for library
#endif

        //! holds the name of the library file
        boost::filesystem::path d_filename;

        //! handle to the library
        LibraryHandle d_library;
    };


}


#endif
