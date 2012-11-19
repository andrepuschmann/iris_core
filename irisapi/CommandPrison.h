/**
 * \file CommandPrison.h
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
 * A CommandPrison holds one or more threads while they wait for
 * named Commands.
 */

#ifndef COMMANDPRISON_H_
#define COMMANDPRISON_H_

#include <irisapi/Command.h>

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

namespace iris
{

class Cage
{
private:
    mutable boost::mutex cageMutex;
    boost::condition_variable theConditionVariable;
    Command command;
    bool locked;
public:
    Cage()
    :locked(false)
    {}

    Command trap(boost::mutex::scoped_lock &l)
    {
        boost::mutex::scoped_lock lock(cageMutex);
        locked = true;
        l.unlock();
        while(locked)
        {
            theConditionVariable.wait(lock);
        }
        return command;
    }

    void release(Command c)
    {
        boost::mutex::scoped_lock lock(cageMutex);
        locked = false;
        command = c;
        lock.unlock();
        theConditionVariable.notify_one();
    }

};

class CommandPrison
{
private:
    typedef std::multimap<std::string, boost::shared_ptr< Cage > > CageMM;
    typedef std::pair<std::string, boost::shared_ptr< Cage > > CagePair;
    mutable boost::mutex theMutex;
    CageMM theCages;

public:
    CommandPrison()
    {}

    Command trap(std::string c)
    {
        boost::mutex::scoped_lock lock(theMutex);
        boost::shared_ptr< Cage > cage(new Cage);
        theCages.insert(CagePair(c,cage));
        return cage->trap(lock);
    }

    void release(Command c)
    {
        boost::mutex::scoped_lock lock(theMutex);
        std::pair<CageMM::iterator, CageMM::iterator> found;
        CageMM::iterator it;
        found = theCages.equal_range(c.commandName);
        for (it=found.first; it!=found.second; ++it)
        {
            it->second->release(c);
        }
        theCages.erase(found.first, found.second);
    }

    int size()
    {
        boost::mutex::scoped_lock lock(theMutex);
        return theCages.size();
    }

};

} /* namespace iris */

#endif /* COMMANDPRISON_H_ */
