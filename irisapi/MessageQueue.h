/**
 * \file MessageQueue.h
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
 * A thread-safe queue for passing messages between threads.
 */

#ifndef IRISAPI_MESSAGEQUEUE_H_
#define IRISAPI_MESSAGEQUEUE_H_

#include <queue>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

namespace iris
{

/// A thread-safe queue used to pass messages between threads.
template<typename T>
class MessageQueue
{
public:
  /// Push an object into the queue
  void push(T const& data)
  {
    boost::mutex::scoped_lock lock(mutex_);
    queue_.push(data);
    lock.unlock();
    conditionVariable_.notify_one();
  }

  /// Check if the queue is empty
  bool empty() const
  {
    boost::mutex::scoped_lock lock(mutex_);
    return queue_.empty();
  }

  /// Try to pop an object from the queue - returns false if queue is empty.
  bool tryPop(T& poppedValue)
  {
    boost::mutex::scoped_lock lock(mutex_);
    if(queue_.empty())
    {
      return false;
    }
    
    poppedValue=queue_.front();
    queue_.pop();
    return true;
  }

  /// Pop an object from the queue - block if the queue is empty.
  void waitAndPop(T& poppedValue)
  {
    boost::mutex::scoped_lock lock(mutex_);
    while(queue_.empty())
    {
      conditionVariable_.wait(lock);
    }
    
    poppedValue=queue_.front();
    queue_.pop();
  }

private:
  std::queue<T> queue_;         ///< Our internal queue container.
  mutable boost::mutex mutex_;  ///< Mutex used for thread safety.
  boost::condition_variable conditionVariable_;   ///< Used for blocking threads.

};


} /* namespace iris */

#endif /* IRISAPI_MESSAGEQUEUE_H_ */
