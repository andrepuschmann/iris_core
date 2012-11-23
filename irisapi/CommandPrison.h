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

#ifndef IRISAPI_COMMANDPRISON_H_
#define IRISAPI_COMMANDPRISON_H_

#include <irisapi/Command.h>

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

namespace iris
{

/** A Cage holds a single thread while it blocks, waiting for a named command.
 *
 * A Cage exists within a CommandPrison. The CommandPrison can hold multiple
 * Cages, each holding a single thread waiting for a named command.
 */
class Cage
{
public:
  Cage()
    :locked_(false)
  {}

  /** Trap a thread inside this Cage.
   *
   * \param l   Reference to the CommandPrison lock which gets released.
   */
  Command trap(boost::mutex::scoped_lock &l)
  {
    boost::mutex::scoped_lock lock(mutex_);
    locked_ = true;
    l.unlock();
    while(locked_)
    {
      conditionVariable_.wait(lock);
    }
    return command_;
  }

  /** Release the thread inside this Cage.
   *
   * @param c   The Command object which releases the trapped thread.
   */
  void release(Command c)
  {
    boost::mutex::scoped_lock lock(mutex_);
    locked_ = false;
    command_ = c;
    lock.unlock();
    conditionVariable_.notify_one();
  }

private:
  mutable boost::mutex mutex_;   ///< Mutex protecting this Cage.
  boost::condition_variable conditionVariable_;   ///< The thread waits for this.
  Command command_;              ///< The command which releases the thread.
  bool locked_;                  ///< Is this Cage locked?

};

/** A container which holds multiple threads, each waiting for a named Command
 * to release it.
 *
 * The CommandPrison holds multiple Cages, each containing a single blocked
 * thread. The threads are released when the Command they are waiting for is
 * issued. The CommandPrison is used within StackComponents where a thread
 * can wait for a named Command. It will block within the CommandPrison
 * until that named Command is issued by a Controller.
 */
class CommandPrison
{
public:
  CommandPrison()
  {}

  /** Trap a thread by blocking in a Cage until a named Command is issued by
   * a Controller.
   *
   * @param c   The name of the Command to wait for.
   * @return  The Command object which has released the thread.
   */
  Command trap(std::string c)
  {
    boost::mutex::scoped_lock lock(mutex_);
    boost::shared_ptr< Cage > cage(new Cage);
    cages_.insert(CagePair(c,cage));
    return cage->trap(lock);
  }

  /** Release any threads which are waiting for a named Command.
   *
   * @param c   The Command which has been issued by a Controller.
   */
  void release(Command c)
  {
    boost::mutex::scoped_lock lock(mutex_);
    std::pair<CageMM::iterator, CageMM::iterator> found;
    CageMM::iterator it;
    found = cages_.equal_range(c.commandName);
    for (it=found.first; it!=found.second; ++it)
    {
      it->second->release(c);
    }
    cages_.erase(found.first, found.second);
  }

  /// Get the number of threads currently held in this CommandPrison.
  int size()
  {
    boost::mutex::scoped_lock lock(mutex_);
    return cages_.size();
  }

private:
  typedef std::multimap<std::string, boost::shared_ptr< Cage > > CageMM;
  typedef std::pair<std::string, boost::shared_ptr< Cage > > CagePair;

  mutable boost::mutex mutex_;  ///< Mutex protecting this CommandPrison.
  CageMM cages_;                ///< The cages within this CommandPrison.

};

} // namespace iris

#endif // IRISAPI_COMMANDPRISON_H_
