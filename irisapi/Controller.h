/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file Controller.h
 * Contains base class for all controllers.
 *
 *  Created on: 19-May-2008
 *  Created by: suttonp
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
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
 *  @param ControllerClass Class of the controller class to be exported by the library
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
	std::string d_name;
	std::string d_description;
	std::string d_author;
	std::string d_version;

	//! MessageQueue of Event objects
	MessageQueue< Event > d_eventQueue;

	//! Interface to the owner of this controller
	ControllerCallbackInterface* d_controllerManager;

	//! Handle for this controller's thread of execution
    boost::scoped_ptr< boost::thread > d_thread;

	bool d_started;
	bool d_loaded;
	mutable boost::mutex theMutex;
	boost::condition_variable theConditionVariable;

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
     * \param name          controller name
     * \param description   brief description of this component
     * \param author        component author
     * \param version       component version
     */
    Controller(std::string name, std::string description, std::string author, std::string version )
		: d_name(name), d_description(description), d_author(author), d_version(version)
		,d_controllerManager(NULL)
		,d_thread(NULL)
		,d_started(false)
		,d_loaded(false)
	{
	}

	//! Process an event - should be overwritten in the controller subclass
	virtual void processEvent(Event &e)
	{
		LOG(LERROR) << "Function processEvent has not been implemented in controller " << d_name;
	}

	void reconfigureRadio(ReconfigSet reconfigs)
	{
		if(d_controllerManager == NULL)
			return;

		d_controllerManager->reconfigureRadio(reconfigs);
	}

	void postCommand(Command command)
	{
		if(d_controllerManager == NULL)
			return;

		d_controllerManager->postCommand(command);
	}

	std::string getParameterValue(std::string paramName, std::string componentName)
	{
		if(d_controllerManager == NULL)
			return "";

		boost::to_lower(paramName);
		boost::to_lower(componentName);
		return d_controllerManager->getParameterValue(paramName, componentName);
	}

	void subscribeToEvent(std::string eventName, std::string componentName)
	{
		if(d_controllerManager == NULL)
			return;

		boost::to_lower(eventName);
		boost::to_lower(componentName);
		d_controllerManager->subscribeToEvent(eventName, componentName, this);
	}

	//! Set an interface to the ControllerManager
	void setCallbackInterface(ControllerCallbackInterface* c)
	{
		d_controllerManager = c;
	}

	//! Pass an event to this Controller
	void postEvent(Event &e)
	{
		d_eventQueue.push(e);
	}

	//! Load the controller thread
	void load()
	{
		//Load the controller thread (if it hasn't already been loaded)
		if( d_thread == NULL)
		{
			d_loaded = true;
			d_thread.reset( new boost::thread( boost::bind( &Controller::eventLoop, this ) ) );
		}
	}

	//! Start this controller
	void start()
	{
		boost::mutex::scoped_lock lock(theMutex);
		d_started = true;
		lock.unlock();
		theConditionVariable.notify_one();
	}

	//! Stop this controller
	void stop()
	{
		boost::mutex::scoped_lock lock(theMutex);
		d_started = false;
		lock.unlock();
		theConditionVariable.notify_one();

		d_thread->interrupt();
	}

	//! Unload the controller thread
	void unload()
	{
		//unload the controller thread
		d_loaded = false;
		d_thread->interrupt();
        d_thread->join();
	}

	//! The main loop for the Controller thread
	void eventLoop()
	{
		//Initialize the controller and subscribe to events
		initialize();
		subscribeToEvents();

        try{
            while(d_loaded)
            {
				try{
					//Hold here if !d_started
					boost::mutex::scoped_lock lock(theMutex);
					while(!d_started)
					{
						theConditionVariable.wait(lock);
					}
					lock.unlock();

					//Interrupt thread here if necessary
					boost::this_thread::interruption_point();

					//Check message queue for Events
					Event currentEvent;
					d_eventQueue.waitAndPop(currentEvent);	//Blocks if queue is empty
					processEvent(currentEvent);
				}
				catch(boost::thread_interrupted)
				{
					LOG(LINFO) << "Controller thread in " << d_name << " interrupted";
				}
            }
        }
        catch(IrisException& ex)
        {
			LOG(LERROR) << "Error in controller " << d_name << ": " << ex.what() << std::endl << "Controller thread exiting.";
        }

		//Destroy the controller
		destroy();
	}

	std::string getName() const
	{
		return d_name;
	}

	std::string getDescription() const
	{
		return d_description;
	}

	std::string getAuthor() const
	{
		return d_author;
	}

	std::string getVersion() const
	{
		return d_version;
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
