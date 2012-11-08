/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file StackComponent.h
 * The Stack Component base class
 *
 *  Created on: 1-Mar-2010
 *  Created by: suttonp
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
 *
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
	std::map<std::string, StackDataBuffer*> d_aboveBuffers;
	std::map<std::string, StackDataBuffer*> d_belowBuffers;

	//! Handle for this StackComponent's thread of execution
    boost::scoped_ptr< boost::thread > d_thread;

	//! The reconfiguration message queue for this StackComponent
    MessageQueue< ParametricReconfig > d_reconfigQueue;

    //! The command prison for this StackComponent
    CommandPrison d_prison;

	//! The StackDataBuffer for this StackComponent
	StackDataBuffer d_buffer;


	//! The main thread loop for this stack component
    virtual void threadLoop()
    {
		//The main loop of this thread
        try{
            while(true)
            {
				boost::this_thread::interruption_point();

				//Get a DataSet
				boost::shared_ptr<StackDataSet> p = d_buffer.popDataSet();

                //Check message queue for ParametricReconfigs
                ParametricReconfig currentReconfig;
                while(d_reconfigQueue.tryPop(currentReconfig))
                {
					boost::mutex::scoped_lock lock(d_parameterMutex);
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
		catch(boost::thread_interrupted)
        {
            LOG(LINFO) << "Thread in stack component " << getName() << " interrupted";
        }
    };

protected:

	//! A mutex to protect the parameters of this component when using multiple threads
	mutable boost::mutex d_parameterMutex;

	//! Pass a message down the stack using the first port
	virtual void sendDownwards(boost::shared_ptr<StackDataSet> set)
	{
		if(!d_belowBuffers.empty())
		{
			set->source = ABOVE;
			d_belowBuffers.begin()->second->pushDataSet(set);
		}
		else
		{
			LOG(LDEBUG) << "sendDownwards() failed. No buffers below.";
		}
	};

	//Pass a message up the stack using the first port
	virtual void sendUpwards(boost::shared_ptr<StackDataSet> set)
	{
		if(!d_aboveBuffers.empty())
		{
			set->source = BELOW;
			d_aboveBuffers.begin()->second->pushDataSet(set);
		}
		else
		{
			LOG(LDEBUG) << "sendUpwards() failed. No buffers above.";
		}
	};

	//! Pass a message down the stack using a named port
	virtual void sendDownwards(std::string portName, boost::shared_ptr<StackDataSet> set)
	{
		if(d_belowBuffers.find(portName) != d_belowBuffers.end())
		{
			set->source = ABOVE;
			d_belowBuffers[portName]->pushDataSet(set);
		}
		else
		{
			LOG(LDEBUG) << "sendDownwards() failed. No buffer below called " << portName;
		}
	};

	//Pass a message up the stack using a named port
	virtual void sendUpwards(std::string portName, boost::shared_ptr<StackDataSet> set)
	{
		if(d_aboveBuffers.find(portName) != d_aboveBuffers.end())
		{
			set->source = BELOW;
			d_aboveBuffers[portName]->pushDataSet(set);
		}
		else
		{
			LOG(LDEBUG) << "sendUpwards() failed. No buffer above called " << portName;
		}
	};

	//Wait for a command
	Command waitForCommand(std::string command)
	{
		return d_prison.trap(command);
	}

public:
    /** Constructor
    *
    *   \param name         Name of the component
    *   \param type         Type of the component
    *   \param description  Brief description of what the component does
    *   \param author       Component author
    *   \param version      Component version
    */
    StackComponent(std::string name, std::string type, std::string description, std::string author, std::string version )
        :ComponentBase(name, type, description, author, version)
    {};

	//! Destructor
    virtual ~StackComponent() {}

    /** Add a buffer above this component
    *
    *	\param portName		Name of the port to add the buffer to
	*   \param above		Buffers for components above
    */
	virtual void addBufferAbove(std::string portName, StackDataBuffer* above)
    {
		d_aboveBuffers[portName] = above;
    };

	/** Add a buffer below this component
    *
    *	\param portName		Name of the port to add the buffer to
    *   \param below		Buffers for components below
    */
    virtual void addBufferBelow(std::string portName, StackDataBuffer* below)
    {
        d_belowBuffers[portName] = below;
    };

	/** Get the buffer for this component
    *
	*   \return		Pointer to this component's buffer
    */
	virtual StackDataBuffer* getBuffer()
	{
		return &d_buffer;
	};

	/** Add reconfigurations to the queue
    *
	*   \param	reconfig	The parametric reconfiguration to be carried out
    */
	void addReconfiguration(ParametricReconfig reconfig)
    {
        d_reconfigQueue.push(reconfig);
    };

	/** Post a command to this component
	*
	*   \param	command	The command to post
	*/
	void postCommand(Command command)
	{
		d_prison.release(command);
	};

	//! Create and start the thread for this stack component
	virtual void startComponent()
	{
		//Start the main component thread
		d_thread.reset( new boost::thread( boost::bind( &StackComponent::threadLoop, this ) ) );
	};

	//! Stop the thread for this stack component
	virtual void stopComponent()
	{
		d_thread->interrupt();
        d_thread->join();
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

