/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file Exceptions.h
 * Declaration of all Exceptions
 *
 *  Created on: 21-Nov-2008
 *  Created by: jlotze, suttonp
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
 */

#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_


#include <string>
#include <stdexcept>

namespace iris
{

/** The base exception class for IRIS 2.0
*
*   All other exceptions within IRIS 2.0 inherit from this base class
*/
class IrisException : public std::exception
{
private:
    std::string d_message;
public:
    IrisException(const std::string &message) throw()
        :exception(), d_message(message)
    {};
    virtual const char* what() const throw()
    {
        return d_message.c_str();
    };
    virtual ~IrisException() throw()
    {};
};

//! Exception thrown when some value is out of range
class ParameterOutOfRangeException : public IrisException
{
public:
    ParameterOutOfRangeException(const std::string &message) throw()
        :IrisException(message)
    {};
};

//! Exception thrown when a parameter is not found in the registered parameters
class ParameterNotFoundException : public IrisException
{
public:
    ParameterNotFoundException(const std::string &message) throw()
        :IrisException(message)
    {};
};

//! Exception thrown when an event is not found in the registered events
class EventNotFoundException : public IrisException
{
public:
    EventNotFoundException(const std::string &message) throw()
        :IrisException(message)
    {};
};

//! Exception thrown when the requested type is not the right one, or not allowed
class InvalidDataTypeException : public IrisException
{
public:
    InvalidDataTypeException(const std::string &message) throw()
        :IrisException(message)
    {};
};

//! Exception thrown when memory allocation fails.
class OutOfMemoryException : public IrisException
{
public:
    OutOfMemoryException(const std::string &message) throw()
        :IrisException(message)
    {};
};

//! Exception thrown by DataBuffer when a DataSet isn't released
class DataBufferReleaseException : public IrisException
{
public:
    DataBufferReleaseException(const std::string &message) throw()
        :IrisException(message)
    {};
};

//! Exception thrown by XmlParser when an xml error is detected
class XmlParsingException : public IrisException
{
public:
    XmlParsingException(const std::string &message) throw()
        :IrisException(message)
    {};
};

//! Exception thrown at the top level
class SystemException : public IrisException
{
public:
    SystemException(const std::string &message) throw()
        :IrisException(message)
    {};
};

//! API mismatch between core and module 
class ApiVersionException : public IrisException
{
public:
    ApiVersionException(const std::string &message) throw()
        :IrisException(message)
    {};
};

//! General exception for when a resource can't be found
class ResourceNotFoundException : public IrisException
{
public:
    ResourceNotFoundException(const std::string &message) throw()
        :IrisException(message)
    {};
};

//! If a file could not be found
class FileNotFoundException : public ResourceNotFoundException
{
public:
    FileNotFoundException(const std::string& message) throw()
        : ResourceNotFoundException(message)
    {}
};

//! If a library load has failed
class LibraryLoadException : public IrisException
{
public:
    LibraryLoadException(const std::string& message) throw()
        : IrisException(message)
    {}
};

//! Thrown if a symbol could not be found in a shared library
class LibrarySymbolException : public ResourceNotFoundException
{
public:
    LibrarySymbolException(const std::string& message) throw()
        : ResourceNotFoundException(message)
    {}
};

//! Error with the given graph structure
class GraphStructureErrorException : public IrisException
{
public:
    GraphStructureErrorException(const std::string &message) throw()
        :IrisException(message)
    {};
};

} // iris namespace

#endif /* EXCEPTIONS_H_ */
