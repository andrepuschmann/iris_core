/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file PNComponent.h
 * The Process Network Component base class
 *
 *  Created on: 5-Jan-2009
 *  Created by: suttonp
 *  $Revision: 1308 $
 *  $LastChangedDate: 2011-09-12 13:19:19 +0100 (Mon, 12 Sep 2011) $
 *  $LastChangedBy: suttonp $
 *
 */

#ifndef PNCOMPONENT_H_
#define PNCOMPONENT_H_

#include <irisapi/ComponentBase.h>
#include <irisapi/DataBufferInterfaces.h>
#include <boost/date_time/posix_time/posix_time.hpp>


namespace iris
{

/** The PNComponent class provides common functionality for all process network components
*
*
*/
class PNComponent: public ComponentBase
{
private:
	//Performance counters
	boost::posix_time::time_duration totalTime;
	int numRuns;

protected:
    //! The inputs of this component
    std::vector<ReadBufferBase*> inputBuffers;
    //! The outputs of this component
    std::vector<WriteBufferBase*> outputBuffers;

    //! Helper fuction to cast a non-templated base buffer to a templated derived buffer
    template <class T>
    WriteBuffer< T >* castToType( WriteBufferBase* buf ) {
        if( TypeInfo< T >::identifier != buf->getTypeIdentifier() )
        {
            throw InvalidDataTypeException("Attempted to cast WriteBuffer to incorrect type.");
        }
        return dynamic_cast< WriteBuffer<T>* >(buf);
    };

    //! Helper fuction to cast a non-templated base buffer to a templated derived buffer
    template <class T>
    ReadBuffer< T >* castToType( ReadBufferBase* buf ) {
        if( TypeInfo< T >::identifier != buf->getTypeIdentifier() )
        {
            throw InvalidDataTypeException("Attempted to cast WriteBuffer to incorrect type.");
        }
        return dynamic_cast< ReadBuffer<T>* >(buf);
    };

public:
    /** Constructor
    *
    *   \param name         Name of the component
    *   \param type         Type of the component
    *   \param description  Brief description of what the component does
    *   \param author       Component author
    *   \param version      Component version
    */
    PNComponent(std::string name, std::string type, std::string description, std::string author, std::string version )
        :ComponentBase(name, type, description, author, version)
    {};


    virtual ~PNComponent()
    {
    	if(numRuns > 0)
    		LOG(LINFO) << "Average time taken per process() call = " << totalTime/(float)numRuns;
    }

    /** Set the input and output buffers for this component
    *
    *   \param in   Vector of ReadBufferBase pointers
    *   \param out  Vector of WriteBufferBase pointers
    */
    virtual void setBuffers(std::vector<ReadBufferBase*> in, std::vector<WriteBufferBase*> out)
    {
        inputBuffers = in;
        outputBuffers = out;
    };

    //! Called by the PNEngine to process this component
    virtual void doProcess()
    {
    	boost::posix_time::ptime t1(boost::posix_time::microsec_clock::local_time());
        process();
        boost::posix_time::ptime t2(boost::posix_time::microsec_clock::local_time());
        totalTime += (t2-t1);
        numRuns++;
    };

    //To be implemented in derived classes
    virtual std::map<std::string, int> calculateOutputTypes(std::map<std::string, int> inputTypes) = 0;
    virtual void registerPorts() = 0;
    virtual void initialize() = 0;
    virtual void process() = 0;

    //May be implemented in derived classes if required (Used mainly by IO components)
    virtual void start(){};
    virtual void stop(){};

    // Interface for template components
public:
    /** Sets up input and output types. Simply returns a pointer to this instance of the class.
     * This function is overridden by template components, which will return a new PNComponent
     * instantiated with the right template parameters.
     * @param inputTypes Vector of input type identifiers, each element represents a port
     * @param outputTypes Vector of output type identifiers, each element represents a port
     * @return this
     */
    virtual PNComponent* setupIO(const std::vector<int>& inputTypes, const std::vector<int>& outputTypes)
    {
        return this;
    }

};

}
#endif

