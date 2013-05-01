/**
 * \file StackInTranslator.h
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
 * A translator between the Iris DataBuffer and the StackDataBuffer.
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
*  and the StackDataBuffers of the StackEngine
*
*/
class StackInTranslator
{
private:

  ReadBuffer<uint8_t>* inputBuffer_;
  StackDataBuffer* aboveBuffer_;

  //! Handle for this StackComponent's thread of execution
  boost::scoped_ptr< boost::thread > thread_;

  //! The main thread loop for this stack component
  virtual void threadLoop();

public:
  //! Constructor
  StackInTranslator();

  //! Destructor
  virtual ~StackInTranslator();

  /** Set the buffers above this component
  *
  *   \param above    Buffers for components above
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

