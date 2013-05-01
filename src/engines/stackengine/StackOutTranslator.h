/**
 * \file StackOutTranslator.h
 * \version 1.0
 *
 * \section COPYRIGHT
 *
 * Copyright 2012-2013 The Iris Project Developers. See the
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
 *  A translator between the StackDataBuffer and the Iris DataBuffer.
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
*  and the DataBuffers of the Iris system
*
*/
class StackOutTranslator
{
private:

  WriteBuffer<uint8_t>* outputBuffer_;

  /// The StackDataBuffer for this Translator
  StackDataBuffer buffer_;

  /// Handle for this StackComponent's thread of execution
  boost::scoped_ptr< boost::thread > thread_;

  /// The main thread loop for this translator
  virtual void threadLoop();

public:
  /// Constructor
  StackOutTranslator();

  /// Destructor
  virtual ~StackOutTranslator();

  /** Set the output buffer for this translator
  *
  *   \param out   WriteBufferBase pointer
  */
  virtual void setOutputBuffer(WriteBufferBase* out);

  /** Get the buffer for this component
  *
  *   \return    Pointer to this component's buffer
  */
  virtual StackDataBuffer* getBuffer();

  /// Create and start the thread for this translator
  virtual void startTranslator();

  /// Stop the thread for this translator
  virtual void stopTranslator();

  std::string getName(){return "StackOutTranslator";};
};

} /* namespace iris */
#endif /* STACKOUTTRANSLATOR_H_ */

