/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file StackInTranslator.cpp
 * Implementation of StackInTranslator class.
 *
 *  Created on: 2-Mar-2010
 *  Created by: suttonp
 *  $Revision: 1190 $
 *  $LastChangedDate: 2010-11-17 16:28:24 +0000 (Wed, 17 Nov 2010) $
 *  $LastChangedBy: suttonp $
 */

#include "StackInTranslator.h"

namespace iris
{

	StackInTranslator::StackInTranslator()
    {}

	StackInTranslator::~StackInTranslator() 
	{}

	void StackInTranslator::setBufferAbove(StackDataBuffer* above)
    {
        d_aboveBuffer = above;
    }

	void StackInTranslator::setInputBuffer(ReadBufferBase* in)
    {
		if(in->getTypeIdentifier() != TypeInfo<uint8_t>::identifier)
		{
			throw InvalidDataTypeException("Only uint8_t data types are valid in the StackEngine");
		}
		d_inputBuffer = dynamic_cast< ReadBuffer<uint8_t>* >(in);
    }

	void StackInTranslator::startTranslator()
	{
		//Start the main component thread
		d_thread.reset( new boost::thread( boost::bind( &StackInTranslator::threadLoop, this ) ) );
	}

	void StackInTranslator::stopTranslator()
	{
		d_thread->interrupt();
        d_thread->join();
	}

	void StackInTranslator::threadLoop()
    {
		//The main loop of this engine thread
        try{
            while(true)
            {
				boost::this_thread::interruption_point();

				//Get a DataSet from the input DataBuffer
				DataSet< uint8_t >* inData = NULL;
				d_inputBuffer->getReadData(inData);

				//Create a StackDataSet and fill it with the info from the DataSet
				boost::shared_ptr<StackDataSet> set(new StackDataSet);
				set->data.resize(inData->data.size());
				std::copy(inData->data.begin(), inData->data.end(), set->data.begin());
				set->timeStamp = inData->timeStamp;

				//Send up to the StackComponent
				sendUpwards(set);

				//Release the DataSet
				d_inputBuffer->releaseReadData(inData);
            }
        }
        catch(IrisException& ex)
        {
            LOG(LFATAL) << "Error in stack translator: " << ex.what() << " - Translator thread exiting.";
        }
		catch(boost::thread_interrupted)
        {
            LOG(LINFO) << "Thread in stack input translator interrupted";
        }
    }

	void StackInTranslator::sendUpwards(boost::shared_ptr<StackDataSet> set)
	{
		if(d_aboveBuffer != NULL)
		{
			set->source = BELOW;
			d_aboveBuffer->pushDataSet(set);
		}
	}

} /* namespace iris */