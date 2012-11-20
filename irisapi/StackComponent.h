/**
 * \file StackComponent.h
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
 * The Stack Component base class.
 */

#ifndef STACKCOMPONENT_H_
#define STACKCOMPONENT_H_

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/progress.hpp>
#include <boost/bind.hpp>

#include <irisapi/ComponentBase.h>
#include <irisapi/StackDataBuffer.h>
#include <irisapi/ReconfigurationDescriptions.h>
#include <irisapi/MessageQueue.h>
#include <irisapi/CommandPrison.h>


namespace iris
{

/** The StackComponent class provides common functionality for all stack components
*
*
*/
class StackComponent: public ComponentBase
{
private:
  std::map<std::string, StackDataBuffer*> aboveBuffers_;
  std::map<std::string, StackDataBuffer*> belowBuffers_;

  //! Handle for this StackComponent's thread of execution
  boost::scoped_ptr< boost::thread > thread_;

  //! The reconfiguration message queue for this StackComponent
  MessageQueue< ParametricReconfig > reconfigQueue_;

  //! The command prison for this StackComponent
  CommandPrison prison_;

  //! The StackDataBuffer for this StackComponent
  StackDataBuffer buffer_;


  //! The main thread loop for this stack component
  virtual void threadLoop()
  {
    //The main loop of this thread
    try{
      while(true)
      {
        boost::this_thread::interruption_point();

        //Get a DataSet
        boost::shared_ptr<StackDataSet> p = buffer_.popDataSet();

        //Check message queue for ParametricReconfigs
        ParametricReconfig currentReconfig;
        while(reconfigQueue_.tryPop(currentReconfig))
        {
          boost::mutex::scoped_lock lock(parameterMutex_);
          setValue(currentReconfig.parameterName, currentReconfig.parameterValue);
          parameterHasChanged(currentReconfig.parameterName);
          LOG(LINFO) << "Reconfigured parameter " << currentReconfig.parameterName << " : " << currentReconfig.parameterValue;
        }

        //Call the appropriate function for the DataSet
        switch(p->source)
        {
        case ABOVE:
          processMessageFromAbove(p);
          break;
        case BELOW:
          processMessageFromBelow(p);
          break;
        default:
          break;
        }
      }
    }
    catch(IrisException& ex)
    {
      LOG(LFATAL) << "Error in stack component: " << ex.what() << " - Component thread exiting.";
    }
    catch(boost::thread_interrupted&)
    {
      LOG(LINFO) << "Thread in stack component " << getName() << " interrupted";
    }
  };

protected:

  //! A mutex to protect the parameters of this component when using multiple threads
  mutable boost::mutex parameterMutex_;

  //! Pass a message down the stack using the first port
  virtual void sendDownwards(boost::shared_ptr<StackDataSet> set)
  {
    if(!belowBuffers_.empty())
    {
      set->source = ABOVE;
      belowBuffers_.begin()->second->pushDataSet(set);
    }
    else
    {
      LOG(LDEBUG) << "sendDownwards() failed. No buffers below.";
    }
  };

  //Pass a message up the stack using the first port
  virtual void sendUpwards(boost::shared_ptr<StackDataSet> set)
  {
    if(!aboveBuffers_.empty())
    {
      set->source = BELOW;
      aboveBuffers_.begin()->second->pushDataSet(set);
    }
    else
    {
      LOG(LDEBUG) << "sendUpwards() failed. No buffers above.";
    }
  };

  //! Pass a message down the stack using a named port
  virtual void sendDownwards(std::string portName, boost::shared_ptr<StackDataSet> set)
  {
    if(belowBuffers_.find(portName) != belowBuffers_.end())
    {
      set->source = ABOVE;
      belowBuffers_[portName]->pushDataSet(set);
    }
    else
    {
      LOG(LDEBUG) << "sendDownwards() failed. No buffer below called " << portName;
    }
  };

  //Pass a message up the stack using a named port
  virtual void sendUpwards(std::string portName, boost::shared_ptr<StackDataSet> set)
  {
    if(aboveBuffers_.find(portName) != aboveBuffers_.end())
    {
      set->source = BELOW;
      aboveBuffers_[portName]->pushDataSet(set);
    }
    else
    {
      LOG(LDEBUG) << "sendUpwards() failed. No buffer above called " << portName;
    }
  };

  //Wait for a command
  Command waitForCommand(std::string command)
  {
    return prison_.trap(command);
  }

public:
  /** Constructor
  *
  *   \param name     Name of the component
  *   \param type     Type of the component
  *   \param description  Brief description of what the component does
  *   \param author     Component author
  *   \param version    Component version
  */
  StackComponent(std::string name, std::string type, std::string description, std::string author, std::string version )
    :ComponentBase(name, type, description, author, version)
  {};

  //! Destructor
  virtual ~StackComponent() {}

  /** Add a buffer above this component
  *
  *  \param portName    Name of the port to add the buffer to
  *   \param above    Buffers for components above
  */
  virtual void addBufferAbove(std::string portName, StackDataBuffer* above)
  {
    aboveBuffers_[portName] = above;
  };

  /** Add a buffer below this component
  *
  *  \param portName    Name of the port to add the buffer to
  *   \param below    Buffers for components below
  */
  virtual void addBufferBelow(std::string portName, StackDataBuffer* below)
  {
    belowBuffers_[portName] = below;
  };

  /** Get the buffer for this component
  *
  *   \return    Pointer to this component's buffer
  */
  virtual StackDataBuffer* getBuffer()
  {
    return &buffer_;
  };

  /** Add reconfigurations to the queue
  *
  *   \param  reconfig  The parametric reconfiguration to be carried out
  */
  void addReconfiguration(ParametricReconfig reconfig)
  {
    reconfigQueue_.push(reconfig);
  };

  /** Post a command to this component
  *
  *   \param  command  The command to post
  */
  void postCommand(Command command)
  {
    prison_.release(command);
  };

  //! Create and start the thread for this stack component
  virtual void startComponent()
  {
    //Start the main component thread
    thread_.reset( new boost::thread( boost::bind( &StackComponent::threadLoop, this ) ) );
  };

  //! Stop the thread for this stack component
  virtual void stopComponent()
  {
    thread_->interrupt();
    thread_->join();
  };

  //Register the port of this component (Can be overridden to implement multiplex components)
  virtual void registerPorts()
  {
    std::vector<int> types;
    types.push_back( int(TypeInfo< uint8_t >::identifier) );

    //The port on top of the componen
    registerInputPort("topport1", types);

    //The port below the component
    registerInputPort("bottomport1", types);
  };

  virtual void initialize() = 0;
  virtual void processMessageFromAbove(boost::shared_ptr<StackDataSet> set) = 0;
  virtual void processMessageFromBelow(boost::shared_ptr<StackDataSet> set) = 0;

  //May be implemented in derived classes if required
  virtual void start(){};
  virtual void stop(){};
};

} /* namespace iris */
#endif /* STACKCOMPONENT_H_ */

