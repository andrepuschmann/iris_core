/*
 * This file is part of Iris 2.
 *
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin.
 * All rights reserved.
 *
 */

/**
 * \file CommandPrison.h
 * A CommandPrison holds one or more threads while they wait for named commands
 *
 *  Created on: 17-May-2011
 *  Created by: suttonp
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
 *
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
