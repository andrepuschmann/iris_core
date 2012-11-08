/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file StackOutTranslator.cpp
 * Implementation of StackOutTranslator class.
 *
 *  Created on: 2-Mar-2010
 *  Created by: suttonp
 *  $Revision: 1190 $
 *  $LastChangedDate: 2010-11-17 16:28:24 +0000 (Wed, 17 Nov 2010) $
 *  $LastChangedBy: suttonp $
 */

#include "StackOutTranslator.h"

namespace iris
{

	StackOutTranslator::StackOutTranslator()
    {}

	StackOutTranslator::~StackOutTranslator() 
	{}

	void StackOutTranslator::setOutputBuffer(WriteBufferBase* out)
    {
		if(out->getTypeIdentifier() != TypeInfo<uint8_t>::identifier)
		{
			throw InvalidDataTypeException("Only uint8_t data types are valid in the StackEngine");
		}
		d_outputBuffer = dynamic_cast< WriteBuffer<uint8_t>* >(out);
    }

	StackDataBuffer* StackOutTranslator::getBuffer()
	{
		return &d_buffer;
	}

	void StackOutTranslator::startTranslator()
	{
		//Start the main component thread
		d_thread.reset( new boost::thread( boost::bind( &StackOutTranslator::threadLoop, this ) ) );
	}

	void StackOutTranslator::stopTranslator()
	{
		d_thread->interrupt();
        d_thread->join();
	}

	void StackOutTranslator::threadLoop()
    {
		//The main loop of this engine thread
        try{
            while(true)
            {
				boost::this_thread::interruption_point();

				//Get a StackDataSet
				boost::shared_ptr<StackDataSet> p = d_buffer.popDataSet();

				//Get a DataSet to write from the output buffer
				DataSet< uint8_t >* outData = NULL;
				d_outputBuffer->getWriteData(outData, p->data.size());

				//Fill the DataSet with info from the StackDataSet
				std::copy(p->data.begin(), p->data.end(), outData->data.begin());
				outData->timeStamp = p->timeStamp;
				outData->sampleRate = 0;

				//Release the DataSet
				d_outputBuffer->releaseWriteData(outData);
            }
        }
        catch(IrisException& ex)
        {
            LOG(LFATAL) << "Error in stack translator: " << ex.what() << " - Translator thread exiting.";
        }
		catch(boost::thread_interrupted)
        {
            LOG(LINFO) << "Thread in stack output translator interrupted";
        }
    }

} /* namespace iris */