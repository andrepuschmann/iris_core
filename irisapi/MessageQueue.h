/**
 * \file MessageQueue.h
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
 * A thread-safe queue for passing messages between threads.
 */

#ifndef IRISAPI_MESSAGEQUEUE_H_
#define IRISAPI_MESSAGEQUEUE_H_

#include <queue>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

namespace iris
{

template<typename T>
class MessageQueue
{
private:
  std::queue<T> theQueue;
  mutable boost::mutex theMutex;
  boost::condition_variable theConditionVariable;
public:
  void push(T const& data)
  {
    boost::mutex::scoped_lock lock(theMutex);
    theQueue.push(data);
    lock.unlock();
    theConditionVariable.notify_one();
  }

  bool empty() const
  {
    boost::mutex::scoped_lock lock(theMutex);
    return theQueue.empty();
  }

  bool tryPop(T& poppedValue)
  {
    boost::mutex::scoped_lock lock(theMutex);
    if(theQueue.empty())
    {
      return false;
    }
    
    poppedValue=theQueue.front();
    theQueue.pop();
    return true;
  }

  void waitAndPop(T& poppedValue)
  {
    boost::mutex::scoped_lock lock(theMutex);
    while(theQueue.empty())
    {
      theConditionVariable.wait(lock);
    }
    
    poppedValue=theQueue.front();
    theQueue.pop();
  }

};


} /* namespace iris */

#endif /* IRISAPI_MESSAGEQUEUE_H_ */
