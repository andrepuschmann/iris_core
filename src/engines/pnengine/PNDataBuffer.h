/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file PNDataBuffer.h
 * The PNDataBuffer which exists on links internal to a PNEngine.
 *
 *  Created on: 24-Aug-2009
 *  Created by: suttonp
 *  $Revision: 1317 $
 *  $LastChangedDate: 2011-09-13 13:01:51 +0100 (Tue, 13 Sep 2011) $
 *  $LastChangedBy: suttonp $
 *
 */

#ifndef PNDATABUFFER_H_
#define PNDATABUFFER_H_

#include "irisapi/DataBufferInterfaces.h"
#include "irisapi/Exceptions.h"
#include "irisapi/TypeInfo.h"

namespace iris
{

/*!
*   \brief The PNDataBuffer class implements a buffer which exists between two IRIS components within a single 
*	PNEngine.
*
*	The buffer consists of a number of DataSet objects which can be written and read by the components.
*	Components can get a DataSet to write to by calling GetWriteSet(). When finished writing, the component
*	releases the DataSet by calling ReleaseWriteSet().
*	Components can get a DataSet to read from by calling GetReadSet(). When finished reading, the component
*	releases the DataSet by calling ReleaseReadSet().
*	The PNDataBuffer is NOT thread-safe. It should only be used within a single PNEngine. It is non-blocking.
*	If a component wishes to write more data sets than are available, the PNDataBuffer will grow.
*/
template <typename T>
class PNDataBuffer : public ReadBuffer<T>, public WriteBuffer<T>
{
public:

    /*!
    *   \brief Constructor
    *
    *   \param dataBufferLength     number of DataSets in the buffer
    */
    explicit PNDataBuffer(int dataBufferLength = 2) throw (InvalidDataTypeException)
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

    virtual ~PNDataBuffer(){};


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
        return is_not_empty();
    }

    /*!
    *   \brief Get the next DataSet to read
    *
    *   \param setPtr   A DataSet pointer which will be set by the buffer
    */
    void getReadData(DataSet<T>*& setPtr) throw(DataBufferReleaseException, boost::thread_interrupted)
    {
        if(d_isReadLocked)
            throw DataBufferReleaseException("getReadData() called before previous DataSet was released");
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
        if(d_isWriteLocked)
			throw DataBufferReleaseException("getWriteData() called before previous DataSet was released");
		if(!d_notFull)
		{
			//If buffer is full, add a new DataSet
			d_buffer.push_back(DataSet<T>());
			d_writeIndex = d_buffer.size()-1;
		}
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
        if(++d_readIndex == d_buffer.size())
            d_readIndex = 0;
        if(d_readIndex == d_writeIndex)
            d_notEmpty = false;
        d_notFull = true;
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
        if(++d_writeIndex == d_buffer.size())
            d_writeIndex = 0;
        if(d_readIndex == d_writeIndex)
            d_notFull = false;
        d_notEmpty = true;
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

};

} /* namespace iris */


#endif /* DATABUFFER_H_ */
