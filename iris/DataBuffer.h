/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file DataBuffer.h
 * The DataBuffer which exists on all links between components in iris.
 *
 *  Created on: 4-Dec-2008
 *  Created by: suttonp
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
 *
 */

#ifndef DATABUFFER_H_
#define DATABUFFER_H_

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#include "irisapi/DataBufferInterfaces.h"
#include "irisapi/Exceptions.h"
#include "irisapi/TypeInfo.h"

namespace iris
{

/*!
*   \brief The DataBuffer class implements a buffer which exists between two IRIS components in different engines.
*
*	The buffer consists of a number of DataSet objects which can be written and read by the components.
*	Components can get a DataSet to write to by calling GetWriteSet(). When finished writing, the component
*	releases the DataSet by calling ReleaseWriteSet().
*	Components can get a DataSet to read from by calling GetReadSet(). When finished reading, the component
*	releases the DataSet by calling ReleaseReadSet().
*	The DataBuffer is thread-safe. In the event that the buffer is full, GetWriteSet() will block.
*	In the event that the buffer is empty, GetReadSet() will block.
*/
template <typename T>
class DataBuffer : public ReadBuffer<T>, public WriteBuffer<T>
{
public:

    /*!
    *   \brief Constructor
    *
    *   \param dataBufferLength     number of DataSets in the buffer
    */
    explicit DataBuffer(int dataBufferLength = 2) throw (InvalidDataTypeException)
        :d_buffer(dataBufferLength, DataSet<T>()),
        d_isReadLocked(false),
        d_isWriteLocked(false),
        d_readIndex(0),
        d_writeIndex(0),
        d_notEmpty(false),
        d_notFull(true)
    {
        //Set the type identifier for this buffer
        typeIdentifier = TypeInfo<T>::identifier;
        if( typeIdentifier == -1)
            throw InvalidDataTypeException("Data type not supported");
    };

    virtual ~DataBuffer(){};


    //! Set the link description (with info on source and sink components) for this buffer
    void setLinkDescription(LinkDescription desc)
    {
        linkDesc = desc;
    };


    //! Get the link description for this buffer
    LinkDescription getLinkDescription() const
    {
        return linkDesc;
    };

    //! Get the identifier for the data type of this buffer
    int getTypeIdentifier() const
    {
        return typeIdentifier;
    };

    //! Is there any data in this buffer?
    bool hasData() const
    {
        boost::mutex::scoped_lock lock(d_mutex);
        return is_not_empty();
    }

    /*!
    *   \brief Get the next DataSet to read
    *
    *   \param setPtr   A DataSet pointer which will be set by the buffer
    */
    void getReadData(DataSet<T>*& setPtr) throw(DataBufferReleaseException, boost::thread_interrupted)
    {
        boost::mutex::scoped_lock lock(d_mutex);
        if(d_isReadLocked)
            throw DataBufferReleaseException("getReadData() called before previous DataSet was released");
        d_notEmptyCond.wait(lock, boost::bind(&DataBuffer<T>::is_not_empty, this));
        d_isReadLocked = true;
        setPtr = &d_buffer[d_readIndex];
    };

    /*!
    *   \brief Get the next DataSet to be written
    *
    *   \param setPtr   A DataSet pointer which will be set by the buffer
    *   \param size     The number of elements required in the DataSet
    */
    void getWriteData(DataSet<T>*& setPtr, std::size_t size) throw(DataBufferReleaseException, boost::thread_interrupted)
    {
        boost::mutex::scoped_lock lock(d_mutex);
        if(d_isWriteLocked)
            throw DataBufferReleaseException("getWriteData() called before previous DataSet was released");
        d_notFullCond.wait(lock, boost::bind(&DataBuffer<T>::is_not_full, this));
        d_isWriteLocked = true;
        if(d_buffer[d_writeIndex].data.size() != size)
            d_buffer[d_writeIndex].data.resize(size);
		d_buffer[d_writeIndex].timeStamp = 0;
        setPtr = &d_buffer[d_writeIndex];
    };

    /*!
    *   \brief Release a read DataSet
    *
    *   \param setPtr   A pointer to the DataSet to be released
    */
    void releaseReadData(DataSet<T>*& setPtr)
    {
        boost::mutex::scoped_lock lock(d_mutex);
        if(++d_readIndex == d_buffer.size())
            d_readIndex = 0;
        if(d_readIndex == d_writeIndex)
            d_notEmpty = false;
        d_notFull = true;
        d_notFullCond.notify_one();
        d_isReadLocked = false;
        setPtr = NULL;
    };

    /*!
    *   \brief Release a write DataSet
    *
    *   \param setPtr   A pointer to the DataSet to be released
    */
    void releaseWriteData(DataSet<T>*& setPtr)
    {
        boost::mutex::scoped_lock lock(d_mutex);
        if(++d_writeIndex == d_buffer.size())
            d_writeIndex = 0;
        if(d_readIndex == d_writeIndex)
            d_notFull = false;
        d_notEmpty = true;
        d_notEmptyCond.notify_one();
        d_isWriteLocked = false;
        setPtr = NULL;
    };

private:
    //! The source and sink component details for this DataBuffer
    LinkDescription linkDesc;

    //! The data type of this buffer
    int typeIdentifier;

    //! The vector of DataSets
    std::vector< DataSet<T> > d_buffer;

    bool d_isReadLocked;
    bool d_isWriteLocked;

    std::size_t d_readIndex;
    std::size_t d_writeIndex;

    bool d_notEmpty;
    bool d_notFull;

    bool is_not_empty() const { return d_notEmpty; }
    bool is_not_full() const { return d_notFull; }

    mutable boost::mutex d_mutex;
    boost::condition d_notEmptyCond;
    boost::condition d_notFullCond;

};

} /* namespace iris */


#endif /* DATABUFFER_H_ */
