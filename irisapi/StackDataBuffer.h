/**
 * \file StackDataBuffer.h
 * \version 1.0
 *
 * \section COPYRIGHT
 *
 * Copyright 2012-2013 The Iris Project Developers. See the
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
 * The StackDataBuffer which exists on all links between components in an
 * Iris StackEngine.
 */

#ifndef IRISAPI_STACKDATABUFFER_H_
#define IRISAPI_STACKDATABUFFER_H_

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#include "irisapi/Exceptions.h"
#include "irisapi/TypeInfo.h"

namespace iris
{

/// Property of a StackDataSet to determine where it came from.
enum Source { ABOVE, BELOW };

/** The StackDataSet class implements a set of data which is passed
 * between StackComponents.
*
*  A dequeue of uint8_t is used to store data - this provides efficient
*  addition of data at the front and back.
*/
struct StackDataSet
{
  Source source;              ///< Where did this come from? (Above/Below)
  std::string sourcePortName; ///< Name of the port this was sent from.
  std::string destPortName;   ///< Name of the port this arrived on.
  std::deque<uint8_t> data;   ///< The actual data.
  double timeStamp;           ///< Timestamp for this data.
  std::string lastComponent;  ///< ??
  
  /// Constructor initializes our variables
  StackDataSet(double t=0)
    : timeStamp(t)
  {}
};

/** The StackDataBuffer class is used inside a StackComponent to hold a queue
 * of StackDataSets that need to be processed.
 *
 * The buffer consists of a queue of pointers to StackDataSets. Components
 * can push a StackDataSet to the buffer by calling pushDataSet(). Components
 * can get a DataSet from the buffer by calling popDataSet(). The DataBuffer
 * is thread-safe. In the event that the buffer is full, PushDataSet() will block.
 * In the event that the buffer is empty, PopDataSet() will block.
 */
class StackDataBuffer
{
public:

  /** Construct a StackDataBuffer
  *
  *  \param maxSize    Max number of elements in queue
  */
  explicit StackDataBuffer(unsigned maxSize = 10)
    :maxBufferSize_(maxSize)
  {};

  virtual ~StackDataBuffer(){};

  /// Return current size
  unsigned size()
  {
    boost::mutex::scoped_lock lock(mutex_);
    return buffer_.size();
  }

  /// Return maximum size
  unsigned capacity()
  {
    boost::mutex::scoped_lock lock(mutex_);
    return maxBufferSize_;
  }

  /// Is there any data in this buffer?
  bool hasData() const
  {
    boost::mutex::scoped_lock lock(mutex_);
    return buffer_.empty();
  }

  /// Is the buffer full?
  bool isFull() const
  {
  boost::mutex::scoped_lock(mutex_);
  return (buffer_.size() >= maxBufferSize_);
  }
  
  /** Get a StackDataSet from the queue
  *
  *  \return A boost::shared_ptr to a StackDataSet
  */
  boost::shared_ptr<StackDataSet> popDataSet()
  {
    boost::mutex::scoped_lock lock(mutex_);
    while(buffer_.empty())
    {
      notEmptyVariable_.wait(lock);
    }
    boost::shared_ptr<StackDataSet> p = buffer_.front();
    buffer_.pop();
    lock.unlock();
    notFullVariable_.notify_one();
    return p;
  }

  /** Add a StackDataSet to a queue
  *
  *   \param set  A boost::shared_ptr to a StackDataSet
  */
  void pushDataSet( boost::shared_ptr<StackDataSet> set)
  {
    boost::mutex::scoped_lock lock(mutex_);
    while(buffer_.size() >= maxBufferSize_)
    {
      notFullVariable_.wait(lock);
    }
    buffer_.push(set);
    lock.unlock();
    notEmptyVariable_.notify_one();
  }

private:
  /// The queue of StackDataSet pointers
  std::queue< boost::shared_ptr<StackDataSet> > buffer_;

  unsigned maxBufferSize_;        ///< Max number of items in the queue.
  mutable boost::mutex mutex_;    ///< Provide thread safety.
  boost::condition_variable notEmptyVariable_;  ///< Used to block if queue is empty.
  boost::condition_variable notFullVariable_;   ///< Used to block if queue is full.

};

} /* namespace iris */

#endif /* IRISAPI_STACKDATABUFFER_H_ */
