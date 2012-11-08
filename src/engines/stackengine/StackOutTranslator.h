/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file StackOutTranslator.h
 * A translator between the StackDataBuffer and the Iris DataBuffer
 *
 *  Created on: 2-Mar-2010
 *  Created by: suttonp
 *  $Revision: 864 $
 *  $LastChangedDate: 2010-03-17 10:12:43 +0000 (Wed, 17 Mar 2010) $
 *  $LastChangedBy: suttonp $
 *
 */

#ifndef STACKOUTTRANSLATOR_H_
#define STACKOUTTRANSLATOR_H_

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/progress.hpp>
#include <boost/bind.hpp>

#include <irisapi/ComponentBase.h>
#include <irisapi/DataBufferInterfaces.h>
#include <irisapi/StackDataBuffer.h>
#include <irisapi/MessageQueue.h>


namespace iris
{

/** The StackOutTranslator class translates between the StackDataBuffers of the StackEngine
*	and the DataBuffers of the Iris system
*
*/
class StackOutTranslator
{
private:

    WriteBuffer<uint8_t>* d_outputBuffer;

	//! The StackDataBuffer for this Translator
	StackDataBuffer d_buffer;

	//! Handle for this StackComponent's thread of execution
    boost::scoped_ptr< boost::thread > d_thread;

	//! The main thread loop for this translator
    virtual void threadLoop();

public:
    //! Constructor
    StackOutTranslator();

	//! Destructor
    virtual ~StackOutTranslator();

	/** Set the output buffer for this translator
    *
    *   \param out   WriteBufferBase pointer
    */
    virtual void setOutputBuffer(WriteBufferBase* out);

	/** Get the buffer for this component
    *
	*   \return		Pointer to this component's buffer
    */
	virtual StackDataBuffer* getBuffer();

	//! Create and start the thread for this translator
	virtual void startTranslator();

	//! Stop the thread for this translator
	virtual void stopTranslator();

	std::string getName(){return "StackOutTranslator";};
};

} /* namespace iris */
#endif /* STACKOUTTRANSLATOR_H_ */

