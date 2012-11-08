/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file DataBufferInterfaces.h
 * Interfaces to the DataBuffer class for reading and writing clients.
 *
 *  Created on: 12-Dec-2008
 *  Created by: suttonp
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
 *
 */

#ifndef DATABUFFERINTERFACES_H_
#define DATABUFFERINTERFACES_H_

#include <boost/utility.hpp>
#include <vector>
#include <irisapi/LinkDescription.h>

namespace iris
{

/** The DataSet struct wraps a block of data being used within the IRIS system.
*
*	Each DataBuffer between two components will contain a vector of DataSets which can be written and read
*	by those components.
*/
template <typename T>
struct DataSet{
    std::vector< T, std::allocator<T> > data;    //We can change this to another container or
                                                //use a custom allocator if we want
    double sampleRate;
    double timeStamp;

    //! Constructor initializes our variables
    DataSet(int l=10, double s=0, double t=0)
        :data(l), sampleRate(s), timeStamp(t){}
};

/** The DataBufferBase class allows us to store vectors of DataBuffers of different types
*
*	The DataBuffer class inherits from this base class. The typeIdentifier specifies
*   the type of the derived class.
*/
class DataBufferBase : boost::noncopyable
{
public:
    virtual ~DataBufferBase(){};
    virtual int getTypeIdentifier() const = 0;
    virtual void setLinkDescription(LinkDescription desc) = 0;
    virtual LinkDescription getLinkDescription() const = 0;
    virtual bool hasData() const = 0;
};

/** The ReadBufferBase class allows us to store vectors of ReadBuffers of different types
*
*	The ReadBuffer and DataBuffer inherit from this base class. The typeIdentifier specifies
*   the type of the derived classes.
*/
class ReadBufferBase:public virtual DataBufferBase
{};

/** The WriteBufferBase class allows us to store vectors of WriteBuffers of different types
*
*	The WriteBuffer and DataBuffer inherit from this base class. The typeIdentifier specifies
*   the type of the derived classes.
*/
class WriteBufferBase:public virtual DataBufferBase
{};

//! The ReadBuffer interface limits the use of a DataBuffer for a writing client.
template <typename T>
class ReadBuffer:public virtual ReadBufferBase
{
public:
    virtual ~ReadBuffer(){};
    virtual void getReadData(DataSet<T>*& setPtr) = 0;
    virtual void releaseReadData(DataSet<T>*& setPtr) = 0;
};

//! The WriteBuffer interface limits the use of a DataBuffer for a writing client.
template <typename T>
class WriteBuffer:public virtual WriteBufferBase
{
public:
    virtual ~WriteBuffer(){};
    virtual void getWriteData(DataSet<T>*& setPtr, std::size_t size) = 0;
    virtual void releaseWriteData(DataSet<T>*& setPtr) = 0;
};

}


#endif
