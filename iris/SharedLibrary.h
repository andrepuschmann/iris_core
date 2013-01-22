/**
 * \file SharedLibrary.h
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
 * The SharedLibrary manages shared libraries in a portable fashion.
 */

#ifndef IRIS_SHAREDLIBRARY_H
#define IRIS_SHAREDLIBRARY_H

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

/** Manages shared libraries in a portable fashion.
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
    typedef FARPROC SymbolPointer;   ///< Type that can hold a pointer to a library symbol
#else
    typedef void* SymbolPointer;   ///< Type that can hold a pointer to a library symbol
#endif

  // ---------- standard usage

  /** Constructs a SharedLibrary instance and loads a shared library.
   *
   * \param filename Path to the library file.
   * \throw LibraryLoadException If an error occurred during library loading.
   * \throw FileNotFoundException If the given file could not be found.
   */
  SharedLibrary(boost::filesystem::path filename) throw (LibraryLoadException,
    FileNotFoundException);

  /** Gets the address of a symbol from the dynamic library.
   *
   * \param symbolName Name of the symbol (function or variable) to search for
   * \return Return value should be cast to the function prototype pointer or variable pointer
   * \throw LibrarySymbolException If an error occurred during resolution of the symbol name.
   */
  SymbolPointer getSymbol(std::string symbolName) throw (LibrarySymbolException);

  /// Destructor. Closes the library.
  ~SharedLibrary();

  std::string getName(){return "SharedLibrary";}



  // ---------- Usage without giving filename to constructor

  /// Default constructor. Does not open a library.
  SharedLibrary() : library_(NULL) {};

  /** Opens a shared library.
   *
   * If a different library is already open and held by this object, it is closed
   * automatically before opening the new one. See constructor for description
   * of parameters.
   *
   * \param filename    The path of the file to open.
   */
  void open(boost::filesystem::path filename) throw (LibraryLoadException,
      FileNotFoundException);

  /// Check whether the library has been loaded.
  bool isLoaded() {return library_ == NULL;};

  // ----------- Accessors

  /// Retrieve filename of currently open file.
  boost::filesystem::path getFilename() const {  return filename_; }

  // ----------- Information

  /** Returns the standard system extension for shared libraries.
   *
   * That is, in Windows, returns "dll", Linux: "so", Mac: "dylib", etc.
   */
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

  /** Returns the standard system prefix for shared libraries.
   *
   * That is, in Windows, returns "", Linux: "lib", Mac: "", etc.
   */
  static std::string getSystemPrefix()
  {
    #if defined(_WIN32) || defined(__CYGWIN__)
      return "";
    #elif defined(__unix__)
      return "lib";
    #elif defined(__APPLE__)
      return "lib";
    #endif
  }

private:

#ifdef BOOST_WINDOWS
  typedef HMODULE LibraryHandle;  ///< Handle for library
#else
  typedef void* LibraryHandle;  ///< Handle for library
#endif

  boost::filesystem::path filename_;    ///< Name of library file.
  LibraryHandle library_;               ///< Handle to library.
};

} // namespace iris
#endif // IRIS_SHAREDLIBRARY_H
