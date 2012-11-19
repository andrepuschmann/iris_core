/**
 * \file StackOutTranslator.cpp
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
 * Implementation of StackOutTranslator class - a translator between the
 * StackDataBuffer and the Iris DataBuffer.
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
        catch(boost::thread_interrupted&)
        {
            LOG(LINFO) << "Thread in stack output translator interrupted";
        }
    }

} /* namespace iris */
