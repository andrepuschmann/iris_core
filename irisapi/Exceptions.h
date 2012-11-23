/**
 * \file Exceptions.h
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
 *  Declaration of all Exceptions used in Iris.
 */

#ifndef IRISAPI_EXCEPTIONS_H_
#define IRISAPI_EXCEPTIONS_H_


#include <string>
#include <stdexcept>

namespace iris
{

/** The base exception class for Iris
*
*   All other exceptions within Iris inherit from this base class
*/
class IrisException
  : public std::exception
{
public:
  IrisException(const std::string &message) throw()
    :exception(), message_(message)
  {};
  virtual const char* what() const throw()
  {
    return message_.c_str();
  };
  virtual ~IrisException() throw()
  {};

private:
  std::string message_;   ///< Message describing the cause of the exception.
};

/// Exception thrown when some value is out of range
class ParameterOutOfRangeException : public IrisException
{
public:
  ParameterOutOfRangeException(const std::string &message) throw()
    :IrisException(message)
  {};
};

/// Exception thrown when a parameter is not found in the registered parameters
class ParameterNotFoundException : public IrisException
{
public:
  ParameterNotFoundException(const std::string &message) throw()
    :IrisException(message)
  {};
};

/// Exception thrown when an event is not found in the registered events
class EventNotFoundException : public IrisException
{
public:
  EventNotFoundException(const std::string &message) throw()
    :IrisException(message)
  {};
};

/// Exception thrown when the requested type is not the right one, or not allowed
class InvalidDataTypeException : public IrisException
{
public:
  InvalidDataTypeException(const std::string &message) throw()
    :IrisException(message)
  {};
};

/// Exception thrown when memory allocation fails.
class OutOfMemoryException : public IrisException
{
public:
  OutOfMemoryException(const std::string &message) throw()
    :IrisException(message)
  {};
};

/// Exception thrown by DataBuffer when a DataSet isn't released
class DataBufferReleaseException : public IrisException
{
public:
  DataBufferReleaseException(const std::string &message) throw()
    :IrisException(message)
  {};
};

/// Exception thrown by XmlParser when an xml error is detected
class XmlParsingException : public IrisException
{
public:
  XmlParsingException(const std::string &message) throw()
    :IrisException(message)
  {};
};

/// Exception thrown at the top level
class SystemException : public IrisException
{
public:
  SystemException(const std::string &message) throw()
    :IrisException(message)
  {};
};

/// API mismatch between core and module
class ApiVersionException : public IrisException
{
public:
  ApiVersionException(const std::string &message) throw()
    :IrisException(message)
  {};
};

/// General exception for when a resource can't be found
class ResourceNotFoundException : public IrisException
{
public:
  ResourceNotFoundException(const std::string &message) throw()
    :IrisException(message)
  {};
};

/// If a file could not be found
class FileNotFoundException : public ResourceNotFoundException
{
public:
  FileNotFoundException(const std::string& message) throw()
    : ResourceNotFoundException(message)
  {}
};

/// If a library load has failed
class LibraryLoadException : public IrisException
{
public:
  LibraryLoadException(const std::string& message) throw()
    : IrisException(message)
  {}
};

/// Thrown if a symbol could not be found in a shared library
class LibrarySymbolException : public ResourceNotFoundException
{
public:
  LibrarySymbolException(const std::string& message) throw()
    : ResourceNotFoundException(message)
  {}
};

/// Error with the given graph structure
class GraphStructureErrorException : public IrisException
{
public:
  GraphStructureErrorException(const std::string &message) throw()
    :IrisException(message)
  {};
};

} // iris namespace

#endif /* IRISAPI_EXCEPTIONS_H_ */
