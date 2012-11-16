/**
 * @file DataBuffer.h
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
 * The DataBuffer which exists on all links between components in Iris.
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
* The buffer consists of a number of DataSet objects which can be written and read by the components.
* Components can get a DataSet to write to by calling GetWriteSet(). When finished writing, the component
* releases the DataSet by calling ReleaseWriteSet().
* Components can get a DataSet to read from by calling GetReadSet(). When finished reading, the component
* releases the DataSet by calling ReleaseReadSet().
* The DataBuffer is thread-safe. In the event that the buffer is full, GetWriteSet() will block.
* In the event that the buffer is empty, GetReadSet() will block.
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
