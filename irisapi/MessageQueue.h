/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file MessageQueue.h
 * A thread-safe queue for passing messages between threads
 *
 *  Created on: 2-Apr-2009
 *  Created by: suttonp
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
 *
 */

#ifndef MESSAGEQUEUE_H_
#define MESSAGEQUEUE_H_

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

#endif /* MESSAGEQUEUE_H_ */
