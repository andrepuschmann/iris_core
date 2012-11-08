/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file StackDataBuffer.h
 * The StackDataBuffer which exists on all links between components in an iris StackEngine.
 *
 *  Created on: 1-Mar-2010
 *  Created by: suttonp
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
 *
 */

#ifndef STACKDATABUFFER_H_
#define STACKDATABUFFER_H_

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#include "irisapi/Exceptions.h"
#include "irisapi/TypeInfo.h"

namespace iris
{

//! Property of a StackDataSet to determine source
enum Source { ABOVE, BELOW };

/*!
*   \brief The StackDataSet class implements a set of data which is passed between StackComponents.
*
*	A dequeue of uint8_t is used to store data - this provides efficient addition of data at the front and back.
*/
struct StackDataSet
{
	Source source;
	std::deque<uint8_t> data;
	double timeStamp;
	std::string lastComponent;
	
	//! Constructor initializes our variables
    StackDataSet(double t=0)
        :timeStamp(t){}
};

/*!
*   \brief The StackDataBuffer class implements a buffer which exists between two IRIS components in a StackEngine.
*
*	The buffer consists of a queue of pointers to StackDataSets.
*	Components can push a StackDataSet to the buffer by calling pushDataSet().
*	Components can get a DataSet from the buffer by calling popDataSet().
*	The DataBuffer is thread-safe. In the event that the buffer is full, PushDataSet() will block.
*	In the event that the buffer is empty, PopDataSet() will block.
*/
class StackDataBuffer
{

private:
    //! The queue of StackDataSet pointers
	std::queue< boost::shared_ptr<StackDataSet> > d_buffer;

	//! Max number of items in the queue
	unsigned d_maxBufferSize;

    mutable boost::mutex d_mutex;
	boost::condition_variable notEmptyVariable;
	boost::condition_variable notFullVariable;

public:

    /*!
    *   \brief Constructor
	*
	*   \param maxSize		Max number of elements in queue
    */
    explicit StackDataBuffer(unsigned maxSize = 10)
		:d_maxBufferSize(maxSize)
	{};

    virtual ~StackDataBuffer(){};

    //! Is there any data in this buffer?
    bool hasData() const
    {
        boost::mutex::scoped_lock lock(d_mutex);
		return d_buffer.empty();
    }

    //! Is the buffer full?
    bool isFull() const
    {
	boost::mutex::scoped_lock(d_mutex);
	return (d_buffer.size() >= d_maxBufferSize);
    }
	
    /*!
    *   \brief Get a StackDataSet from the queue
    *
	*	\return A boost::shared_ptr to a StackDataSet
    */
	boost::shared_ptr<StackDataSet> popDataSet() throw(boost::thread_interrupted)
    {
        boost::mutex::scoped_lock lock(d_mutex);
        while(d_buffer.empty())
        {
            notEmptyVariable.wait(lock);
        }
		boost::shared_ptr<StackDataSet> p = d_buffer.front();
		d_buffer.pop();
		lock.unlock();
		notFullVariable.notify_one();
		return p;
    };

    /*!
    *   \brief Add a StackDataSet to a queue
    *
    *   \param set	A boost::shared_ptr to a StackDataSet
    */
	void pushDataSet( boost::shared_ptr<StackDataSet> set) throw(boost::thread_interrupted)
    {
        boost::mutex::scoped_lock lock(d_mutex);
		while(d_buffer.size() >= d_maxBufferSize)
        {
            notFullVariable.wait(lock);
        }
		d_buffer.push(set);
		lock.unlock();
		notEmptyVariable.notify_one();
    };
};

} /* namespace iris */

#endif /* STACKDATABUFFER_H_ */
