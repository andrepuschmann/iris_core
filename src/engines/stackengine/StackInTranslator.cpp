/**
 * \file StackInTranslator.cpp
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
 * Implementation of StackInTranslator class - a translator between the
 * Iris DataBuffer and the StackDataBuffer.
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
    catch(boost::thread_interrupted&)
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
