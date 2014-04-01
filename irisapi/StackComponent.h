/**
 * \file StackComponent.h
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
 * The Stack Component base class.
 */

#ifndef IRISAPI_STACKCOMPONENT_H_
#define IRISAPI_STACKCOMPONENT_H_

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/ptr_container/ptr_map.hpp>
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

/** The StackLink struct is used by a StackComponent to hold a pointer to
 * the StackDataBuffer of a neighbour component, along with the associated
 * port names.
 */
struct StackLink
{
  std::string myPort;           ///< Name of this component's port.
  std::string neighbourPort;    ///< Destination port name.
  StackDataBuffer* buffer;      ///< Neighbour component buffer.

  StackLink(std::string mP = "",
            std::string nP = "",
            StackDataBuffer* b = NULL)
      :myPort(mP)
      ,neighbourPort(nP)
      ,buffer(b)
  {}

};

/** The StackComponent class provides common functionality for all stack components.
 *
 * StackComponents run their own thread of execution which loops, checking
 * an internal queue for messages. These messages can come from neighbour
 * components above or below this one. User-defined functions are called
 * depending upon whether the message came from above or below. Data flow
 * between StackComponents is bidirectional. StackComponents can generate new
 * messages and send them up or down at any time.
 */
class StackComponent: public ComponentBase
{
public:
  /** Constructor
  *
  *   \param name         Name of the component.
  *   \param type         Type of the component.
  *   \param description  Brief description of what the component does.
  *   \param author       Component author.
  *   \param version      Component version.
  */
  StackComponent(std::string name, std::string type, std::string description, std::string author, std::string version )
    :ComponentBase(name, type, description, author, version)
  {}

  /// Destructor
  virtual ~StackComponent() {}

  /** Add a buffer above this component
  *
  *  \param myPort          Name of the port on this component.
  *  \param neighbourPort   Name of the port on neighbour component.
  *  \param above           Neighbour component buffer.
  */
  virtual void addBufferAbove(std::string myPort,
                              std::string neighbourPort,
                              StackDataBuffer* above)
  {
    aboveBuffers_[myPort] = StackLink(myPort, neighbourPort, above);
  }

  /** Add a buffer below this component
  *
  *  \param myPort          Name of the port on this component.
  *  \param neighbourPort   Name of the port on neighbour component.
  *  \param below           Neighbour component buffer.
  */
  virtual void addBufferBelow(std::string myPort,
                              std::string neighbourPort,
                              StackDataBuffer* below)
  {
    belowBuffers_[myPort] = StackLink(myPort, neighbourPort, below);
  }

  /** Get the buffer for this component
  *
  *   \return    Pointer to this component's buffer
  */
  virtual StackDataBuffer* getBuffer(std::string port)
  {
    return &buffers_[port];
  }

  /** Add reconfigurations to the queue
  *
  *   \param  reconfig  The parametric reconfiguration to be carried out
  */
  void addReconfiguration(ParametricReconfig reconfig)
  {
    reconfigQueue_.push(reconfig);
  }

  /** Post a command to this component
  *
  *   \param  command  The command to post
  */
  void postCommand(Command command)
  {
    prison_.release(command);
  }

  /// Create and start the thread for this stack component.
  virtual void startComponent()
  {
    std::vector<Port> inPorts = getInputPorts();
    std::vector<Port>::iterator it;
    for (it = inPorts.begin(); it != inPorts.end(); ++it) {
      std::string name((*it).portName);
      // determine whether this is a port on top or below a component
      Source source = ((name.find("top") != std::string::npos) ? ABOVE : BELOW);
      // create thread for this input port
      threads_.push_back( new boost::thread( boost::bind( &StackComponent::threadLoop, this, name, source, boost::ref(buffers_[name]) ) ) );
    }

    // start reconfiguration thread
    threads_.push_back( new boost::thread( boost::bind( &StackComponent::reconfigThread, this ) ) );
  }

  /// Stop the thread for this stack component.
  virtual void stopComponent()
  {
    boost::ptr_vector<boost::thread>::iterator it;
    for (it = threads_.begin(); it != threads_.end(); ++it) {
      it->interrupt();
      it->join();
    }
  }

  /** Register the default ports for this component.
   *
   * This function can be overridden in derived classes to implement components
   * with multiple top and/or bottom ports.
   */
  virtual void registerPorts()
  {
    std::vector<int> types;
    types.push_back( int(TypeInfo< uint8_t >::identifier) );

    registerInputPort("topport1", types);
    registerInputPort("bottomport1", types);
  }

  /// \name Should be implemented in derived classes
  //@{
  virtual void initialize() = 0;
  virtual void processMessageFromAbove(boost::shared_ptr<StackDataSet> set) = 0;
  virtual void processMessageFromBelow(boost::shared_ptr<StackDataSet> set) = 0;
  //@}

  /// \name May be implemented in derived classes if required
  //@{
  virtual void start(){}
  virtual void stop(){}
  //@}

protected:

  /// Pass a message down the stack using the first port
  virtual void sendDownwards(boost::shared_ptr<StackDataSet> set)
  {
    if(!belowBuffers_.empty())
    {
      set->source = ABOVE;
      std::map<std::string, StackLink>::iterator it = belowBuffers_.begin();
      set->sourcePortName = it->first;
      set->destPortName = it->second.neighbourPort;
      it->second.buffer->pushDataSet(set);
    }
    else
    {
      LOG(LDEBUG) << "sendDownwards() failed. No buffers below.";
    }
  }

  /// Pass a message up the stack using the first port
  virtual void sendUpwards(boost::shared_ptr<StackDataSet> set)
  {
    if(!aboveBuffers_.empty())
    {
      set->source = BELOW;
      std::map<std::string, StackLink>::iterator it = aboveBuffers_.begin();
      set->sourcePortName = it->first;
      set->destPortName = it->second.neighbourPort;
      it->second.buffer->pushDataSet(set);
    }
    else
    {
      LOG(LDEBUG) << "sendUpwards() failed. No buffers above.";
    }
  }

  /// Pass a message down the stack using a named port
  virtual void sendDownwards(std::string portName, boost::shared_ptr<StackDataSet> set)
  {
    if(belowBuffers_.find(portName) != belowBuffers_.end())
    {
      set->source = ABOVE;
      set->sourcePortName = portName;
      set->destPortName = belowBuffers_[portName].neighbourPort;
      belowBuffers_[portName].buffer->pushDataSet(set);
    }
    else
    {
      LOG(LDEBUG) << "sendDownwards() failed. No buffer below called " << portName;
    }
  }

  /// Pass a message up the stack using a named port
  virtual void sendUpwards(std::string portName, boost::shared_ptr<StackDataSet> set)
  {
    if(aboveBuffers_.find(portName) != aboveBuffers_.end())
    {
      set->source = BELOW;
      set->sourcePortName = portName;
      set->destPortName = aboveBuffers_[portName].neighbourPort;
      aboveBuffers_[portName].buffer->pushDataSet(set);
    }
    else
    {
      LOG(LDEBUG) << "sendUpwards() failed. No buffer above called " << portName;
    }
  }

  /// Wait for a named command
  Command waitForCommand(std::string command)
  {
    return prison_.trap(command);
  }

  /// Protects the parameters of this component when using multiple threads
  mutable boost::mutex parameterMutex_;

private:
  /// The thread loop for a input port of this stack component
  virtual void threadLoop(std::string portName, Source source, StackDataBuffer &buffer)
  {
    try{
      while(true)
      {
        boost::this_thread::interruption_point();

        //Get a DataSet
        boost::shared_ptr<StackDataSet> p = buffer.popDataSet();

        if (source == ABOVE) {
            processMessageFromAbove(p);
        } else {
            processMessageFromBelow(p);
        }
      }
    }
    catch(IrisException& ex)
    {
      LOG(LFATAL) << "Error in stack component: " << ex.what() << " - Exiting thread for " << portName;
    }
    catch(boost::thread_interrupted&)
    {
      LOG(LINFO) << "Thread for " << portName << " in stack component " << getName() << " interrupted";
    }
  }

  virtual void reconfigThread()
  {
    try{
      while(true)
      {
        boost::this_thread::interruption_point();

        //Check message queue for ParametricReconfigs
        ParametricReconfig currentReconfig;
        reconfigQueue_.waitAndPop(currentReconfig);
        {
          boost::mutex::scoped_lock lock(parameterMutex_);
          setValue(currentReconfig.parameterName, currentReconfig.parameterValue);
          parameterHasChanged(currentReconfig.parameterName);
          LOG(LINFO) << "Reconfigured parameter " << currentReconfig.parameterName << " : " << currentReconfig.parameterValue;
        }
      }
    }
    catch(IrisException& ex)
    {
      LOG(LFATAL) << "Error in stack component: " << ex.what() << " - Exiting reconfiguration thread.";
    }
    catch(boost::thread_interrupted&)
    {
      LOG(LINFO) << "Thread for parameter reconfiguration for stack component " << getName() << " interrupted";
    }
  }

  std::map<std::string, StackLink> aboveBuffers_;         ///< Pointers to neighbours above.
  std::map<std::string, StackLink> belowBuffers_;         ///< Pointers to neighbours below.
  boost::ptr_vector<boost::thread> threads_;              ///< This component's threads.
  MessageQueue< ParametricReconfig > reconfigQueue_;      ///< Reconfigs for this component.
  CommandPrison prison_;                                  ///< Used to wait for commands issued by a controller.
  boost::ptr_map<std::string, StackDataBuffer > buffers_; ///< Buffers containing data messages for this component.
};

} /* namespace iris */
#endif /* IRISAPI_STACKCOMPONENT_H_ */

