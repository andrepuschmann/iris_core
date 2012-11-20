/**
 * \file Controller.h
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
 * Contains base class for all Iris controllers.
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <vector>
#include <boost/any.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/algorithm/string.hpp>

#include <irisapi/TypeInfo.h>
#include <irisapi/Event.h>
#include <irisapi/Exceptions.h>
#include <irisapi/Logging.h>
#include <irisapi/MessageQueue.h>
#include <irisapi/ControllerCallbackInterface.h>

/** Macro for IRIS boilerplate code in each controller.
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

/*!
 * \brief The base class for all controllers
 *
 */
class Controller
{
private:
  std::string name_;
  std::string description_;
  std::string author_;
  std::string version_;

  //! MessageQueue of Event objects
  MessageQueue< Event > eventQueue_;

  //! Interface to the owner of this controller
  ControllerCallbackInterface* controllerManager_;

  //! Handle for this controller's thread of execution
  boost::scoped_ptr< boost::thread > thread_;

  bool started_;
  bool loaded_;
  mutable boost::mutex mutex_;
  boost::condition_variable conditionVar_;

protected:
  //! Called when this controller is created to allow it to subscribe to events - called by controller thread
  virtual void subscribeToEvents() = 0;

  //! Initialize this controller - called by controller thread
  virtual void initialize() = 0;

  //! Destroy this controller - called by controller thread
  virtual void destroy() = 0;

public:

  virtual ~Controller() {};

  /*!
   * \brief Construct this controller
   *
   * \param name      controller name
   * \param description   brief description of this component
   * \param author    component author
   * \param version     component version
   */
  Controller(std::string name, std::string description, std::string author, std::string version )
    : name_(name), description_(description), author_(author), version_(version)
    ,controllerManager_(NULL)
    ,thread_(NULL)
    ,started_(false)
    ,loaded_(false)
  {
  }

  //! Process an event - should be overwritten in the controller subclass
  virtual void processEvent(Event &e)
  {
    LOG(LERROR) << "Function processEvent has not been implemented in controller " << name_;
  }

  void reconfigureRadio(ReconfigSet reconfigs)
  {
    if(controllerManager_ == NULL)
      return;

    controllerManager_->reconfigureRadio(reconfigs);
  }

  void postCommand(Command command)
  {
    if(controllerManager_ == NULL)
      return;

    controllerManager_->postCommand(command);
  }

  std::string getParameterValue(std::string paramName, std::string componentName)
  {
    if(controllerManager_ == NULL)
      return "";

    boost::to_lower(paramName);
    boost::to_lower(componentName);
    return controllerManager_->getParameterValue(paramName, componentName);
  }

  void subscribeToEvent(std::string eventName, std::string componentName)
  {
    if(controllerManager_ == NULL)
      return;

    boost::to_lower(eventName);
    boost::to_lower(componentName);
    controllerManager_->subscribeToEvent(eventName, componentName, this);
  }

  //! Set an interface to the ControllerManager
  void setCallbackInterface(ControllerCallbackInterface* c)
  {
    controllerManager_ = c;
  }

  //! Pass an event to this Controller
  void postEvent(Event &e)
  {
    eventQueue_.push(e);
  }

  //! Load the controller thread
  void load()
  {
    //Load the controller thread (if it hasn't already been loaded)
    if( thread_ == NULL)
    {
      loaded_ = true;
      thread_.reset( new boost::thread( boost::bind( &Controller::eventLoop, this ) ) );
    }
  }

  //! Start this controller
  void start()
  {
    boost::mutex::scoped_lock lock(mutex_);
    started_ = true;
    lock.unlock();
    conditionVar_.notify_one();
  }

  //! Stop this controller
  void stop()
  {
    boost::mutex::scoped_lock lock(mutex_);
    started_ = false;
    lock.unlock();
    conditionVar_.notify_one();

    thread_->interrupt();
  }

  //! Unload the controller thread
  void unload()
  {
    //unload the controller thread
    loaded_ = false;
    thread_->interrupt();
    thread_->join();
  }

  //! The main loop for the Controller thread
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

  /** Set the logging policy for this controller
  *
  *   \param policy   The logging policy to set
  */
  virtual void setLoggingPolicy(LoggingPolicy* policy) const
  {
    Logger::getPolicy() = policy;
  };

};

} /* namespace iris */


#endif /* COMPONENTBASE_H_ */
