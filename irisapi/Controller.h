/**
 * \file Controller.h
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
 * Contains base class for all Iris controllers.
 */

#ifndef IRISAPI_CONTROLLER_H_
#define IRISAPI_CONTROLLER_H_

#include <vector>
#include <boost/any.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/algorithm/string.hpp>
#include <irisapi/ModuleParameters.h>
#include <irisapi/ComponentEvents.h>
#include <irisapi/TypeInfo.h>
#include <irisapi/Event.h>
#include <irisapi/Exceptions.h>
#include <irisapi/Logging.h>
#include <irisapi/MessageQueue.h>
#include <irisapi/ControllerCallbackInterface.h>
#include <irisapi/CommandPrison.h>

/** Macro for Iris boilerplate code in each controller.
 *  \param ControllerClass Class of the controller class to be exported by the library
 *
 *  This macro defines the GetApiVersion(), CreateController(), and ReleaseController()
 *  functions with the correct C-style library export specifiers. It must be called
 *  in each controller's source file.
 *  Example, for a controller called ExampleController:
 *  \code
 *  IRIS_CONTROLLER_EXPORTS(ExampleController)
 *
 *  ExampleController::~ExampleController()
 *   ...
 *  \endcode
 */
#define IRIS_CONTROLLER_EXPORTS(ControllerClass) \
  extern "C" EXPORT_DECLSPEC const char* GetApiVersion() \
  { \
    return Version::getApiVersion(); \
  } \
  extern "C" EXPORT_DECLSPEC Controller* CreateController() \
  { \
    return new ControllerClass(); \
  } \
  extern "C" EXPORT_DECLSPEC void ReleaseController(Controller* cont) \
  { \
    delete cont; \
  }

namespace iris
{

/** The base class for all Controllers.
 *
 * Controllers in Iris have a global view of the running radio. They
 * can subscribe to events on any component and carry out reconfigurations
 * of the running radio.
 */
class Controller
  : public ModuleParameters,
    public ComponentEvents
{
public:

  virtual ~Controller() {};

  /** Construct this controller
   *
   * \param name      controller name
   * \param description   brief description of this component
   * \param author    component author
   * \param version     component version
   */
  Controller(std::string name, std::string description, std::string author, std::string version )
    : name_(name), description_(description), author_(author), version_(version)
    ,controllerManager_(NULL)
    ,eventThread_(NULL)
    ,workThread_(NULL)
    ,started_(false)
    ,loaded_(false)
  {
  }

  /// Process an event - should be overwritten in the controller subclass
  virtual void processEvent(Event &e)
  {
    LOG(LERROR) << "Function processEvent has not been implemented in controller " << name_;
  }

  //! Execute separate working thread - should be overwritten in the controller subclass
  virtual void workFunction()
  {
    LOG(LINFO) << "Function workFuntion has not been implemented in controller " << name_;
  }

  /// Called by derived controller to reconfigure the radio.
  void reconfigureRadio(ReconfigSet reconfigs)
  {
    if(controllerManager_ == NULL)
      return;

    controllerManager_->reconfigureRadio(reconfigs);
  }

  /// Called by derived controller to issue a command.
  void postCommand(Command command)
  {
    if(controllerManager_ == NULL)
      return;

    controllerManager_->postCommand(command);
  }

  /// Called by controller manager to post a command for this controller
  void postLocalCommand(Command command)
  {
      this->prison_.release(command);
  }


  /// Wait for a named command
  Command waitForCommand(std::string command)
  {
    return prison_.trap(command);
  }

  /// Called by a derived controller to get the current value of a parameter.
  std::string getParameterValue(std::string paramName, std::string componentName)
  {
    if(controllerManager_ == NULL)
      return "";

    boost::to_lower(paramName);
    boost::to_lower(componentName);
    return controllerManager_->getParameterValue(paramName, componentName);
  }

  /// Called by a derived controller to subscribe to an event on a component.
  void subscribeToEvent(std::string eventName, std::string componentName)
  {
    if(controllerManager_ == NULL)
      return;

    boost::to_lower(eventName);
    boost::to_lower(componentName);
    controllerManager_->subscribeToEvent(eventName, componentName, this);
  }


  /// Called by ControllerManager to set the callback interface.
  void setCallbackInterface(ControllerCallbackInterface* c)
  {
    controllerManager_ = c;
  }

  /// Called by ControllerManager to pass an event to this Controller
  void postEvent(Event &e)
  {
    eventQueue_.push(e);
  }

  /// Called by ControllerManager to load the controller thread.
  void load()
  {
    //Load the controller thread (if it hasn't already been loaded)
    if( eventThread_ == NULL)
    {
      loaded_ = true;
      eventThread_.reset( new boost::thread( boost::bind( &Controller::eventLoop, this ) ) );
    }
    //Load worker thread
    if ( workThread_ == NULL)
    {
      workThread_.reset( new boost::thread( boost::bind( &Controller::workLoop, this ) ) );
    }
  }

  /// Called by ControllerManager to start this controller
  void start()
  {
    boost::mutex::scoped_lock lock(mutex_);
    started_ = true;
    lock.unlock();
    conditionVar_.notify_one();
  }

  /// Called by ControllerManager to stop this controller
  void stop()
  {
    boost::mutex::scoped_lock lock(mutex_);
    started_ = false;
    lock.unlock();
    conditionVar_.notify_one();

    if (workThread_) workThread_->interrupt();
    eventThread_->interrupt();
  }

  /// Called by ControllerManager to unload the controller thread
  void unload()
  {
    //unload the work thread first
    if (workThread_)
    {
      workThread_->interrupt();
      workThread_->join();
    }
    //unload the controller thread
    loaded_ = false;
    eventThread_->interrupt();
    eventThread_->join();
  }

  /// The main loop for the Controller thread
  void eventLoop()
  {
    //Initialize the controller and subscribe to events
    initialize();
    subscribeToEvents();

    try{
      while(loaded_)
      {
        try{
          //Hold here if !d_started
          boost::mutex::scoped_lock lock(mutex_);
          while(!started_)
          {
            conditionVar_.wait(lock);
          }
          lock.unlock();

          //Interrupt thread here if necessary
          boost::this_thread::interruption_point();

          //Check message queue for Events
          Event currentEvent;
          eventQueue_.waitAndPop(currentEvent);  //Blocks if queue is empty
          processEvent(currentEvent);
        }
        catch(boost::thread_interrupted)
        {
          LOG(LINFO) << "Controller thread in " << name_ << " interrupted";
        }
      }
    }
    catch(IrisException& ex)
    {
      LOG(LERROR) << "Error in controller " << name_ << ": " << ex.what() << std::endl << "Controller thread exiting.";
    }

    //Destroy the controller
    destroy();
  }

  //! The main loop for the worker thread
  void workLoop()
  {
      try {
          try{
              // Just call workFuntion from here
              workFunction();
          }
          catch(boost::thread_interrupted)
          {
              LOG(LINFO) << "Work thread of controller " << name_ << " interrupted";
          }
      }
      catch(IrisException& ex)
      {
          LOG(LERROR) << "Error in controller " << name_ << ": " << ex.what() << std::endl << "Worker thread exiting.";
      }
  }

  //! Activate an event
  template<typename T>
  inline void activateEvent(std::string name, T &data)
    throw (EventNotFoundException, InvalidDataTypeException);

  //! Activate an event
  template<typename T>
  inline void activateEvent(std::string name, std::vector<T> &data)
    throw (EventNotFoundException, InvalidDataTypeException);

  std::string getEngineName(std::string componentName, int *engineIndex, int *compIndex)
  {
      return controllerManager_->getEngineName(componentName, engineIndex, compIndex);
  }

  int getNrEngines()
  {
      return controllerManager_->getNrEngines();
  }

  int  getNrComponents()
  {
      return controllerManager_->getNrComponents();
  }

  std::string getEngineNameFromIndex(int index)
  {
      return controllerManager_->getEngineNameFromIndex(index);
  }

  std::string getComponentName(int index)
  {
      return controllerManager_->getComponentName(index);
  }

  int  getNrParameters(std::string componentName)
  {
      return controllerManager_->getNrParameters(componentName);
  }

  std::string getParameterName(std::string componentName, int paramIndex, std::string &paramValue)
  {
      return controllerManager_->getParameterName(componentName, paramIndex, paramValue);
  }

  std::string getName() const
  {
    return name_;
  }

  std::string getDescription() const
  {
    return description_;
  }

  std::string getAuthor() const
  {
    return author_;
  }

  std::string getVersion() const
  {
    return version_;
  }

  /// Called by the ControllerManager to set the logging policy.
  virtual void setLoggingPolicy(LoggingPolicy* policy) const
  {
    Logger::getPolicy() = policy;
  };

protected:
  /// Subscribe to events on Iris components - called by controller thread.
  virtual void subscribeToEvents() = 0;

  /// Initialize this controller - called by controller thread.
  virtual void initialize() = 0;

  /// Destroy this controller - called by controller thread.
  virtual void destroy() = 0;

private:
  std::string name_;
  std::string description_;
  std::string author_;
  std::string version_;

  MessageQueue< Event > eventQueue_;                ///< Queue of incoming Event objects.
  ControllerCallbackInterface* controllerManager_;  ///< Interface to the ControllerManager.
  boost::scoped_ptr< boost::thread > eventThread_;  ///< This controller's thread for handling events.
  boost::scoped_ptr< boost::thread > workThread_;   ///< An additional thread that a controller may use.

  bool started_;
  bool loaded_;
  mutable boost::mutex mutex_;
  boost::condition_variable conditionVar_;
  CommandPrison prison_;      ///< Used to wait for commands issued by another controller.
};

// Get the name of this component and pass everything on to ComponentEvents
template<typename T>
inline void Controller::activateEvent(std::string name, T &data)
    throw (EventNotFoundException, InvalidDataTypeException)
{
    if(controllerManager_ == NULL)
        return;

    boost::to_lower(name);

    Event e;
    e.data.push_back(boost::any(data));
    e.eventName = name;
    e.componentName = this->getName();
    e.typeId = TypeInfo<T>::identifier;
    controllerManager_->activateEvent(e);
    return;
}

// Get the name of this component and pass everything on to ComponentEvents
template<typename T>
inline void Controller::activateEvent(std::string name, std::vector<T> &data)
    throw (EventNotFoundException, InvalidDataTypeException)
{
    if(controllerManager_ == NULL)
        return;

    boost::to_lower(name);

    Event e;
    e.data.resize(data.size());
    std::copy(data.begin(), data.end(), e.data.begin());
    e.eventName = name;
    e.componentName = this->getName();
    e.typeId = TypeInfo<T>::identifier;
    controllerManager_->activateEvent(e);
    return;
}

} /* namespace iris */


#endif /* IRISAPI_COMPONENTBASE_H_ */
