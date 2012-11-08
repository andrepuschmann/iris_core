/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file StackInTranslator.h
 * A translator between the Iris DataBuffer and the StackDataBuffer
 *
 *  Created on: 2-Mar-2010
 *  Created by: suttonp
 *  $Revision: 851 $
 *  $LastChangedDate: 2010-03-05 12:37:47 +0000 (Fri, 05 Mar 2010) $
 *  $LastChangedBy: suttonp $
 *
 */

#ifndef STACKINTRANSLATOR_H_
#define STACKINTRANSLATOR_H_

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/progress.hpp>
#include <boost/bind.hpp>

#include <irisapi/ComponentBase.h>
#include <irisapi/DataBufferInterfaces.h>
#include <irisapi/StackDataBuffer.h>


namespace iris
{

/** The StackInTranslator class translates between the DataBuffers of the Iris system
*	and the StackDataBuffers of the StackEngine
*
*/
class StackInTranslator
{
private:

    ReadBuffer<uint8_t>* d_inputBuffer;
    StackDataBuffer* d_aboveBuffer;

	//! Handle for this StackComponent's thread of execution
    boost::scoped_ptr< boost::thread > d_thread;

	//! The main thread loop for this stack component
    virtual void threadLoop();

public:
    //! Constructor
    StackInTranslator();

	//! Destructor
    virtual ~StackInTranslator();

    /** Set the buffers above this component
    *
	*   \param above		Buffers for components above
    */
    virtual void setBufferAbove(StackDataBuffer* above);

	/** Set the input buffers for this translator
    *
    *   \param in   ReadBufferBase pointer
    */
    virtual void setInputBuffer(ReadBufferBase* in);

	//! Create and start the thread for this stack component
	virtual void startTranslator();

	//! Stop the thread for this stack component
	virtual void stopTranslator();

	//! Send a StackDataSet to the neighbour above
	virtual void sendUpwards(boost::shared_ptr<StackDataSet> set);

	std::string getName(){return "StackInTranslator";};
};

} /* namespace iris */
#endif /* STACKINTRANSLATOR_H_ */

