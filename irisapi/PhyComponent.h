/**
 * \file PhyComponent.h
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
 * The Process Network Component base class.
 */

#ifndef IRISAPI_PHYCOMPONENT_H_
#define IRISAPI_PHYCOMPONENT_H_

#include <irisapi/ComponentBase.h>
#include <irisapi/DataBufferInterfaces.h>
#include <boost/date_time/posix_time/posix_time.hpp>


namespace iris
{

/** The PhyComponent class provides common functionality for all process
 * network components.
 *
 * PhyComponents run in a PhyEngine. They typically read data from one or
 * more input ports, process it and write to one or more output ports.
 * PhyComponents do not control the size of input data, but specify the
 * size of their output data.
 */
class PhyComponent
  : public ComponentBase
{
public:
  /** Constructor
   *
   * \param name         Name of the component
   * \param type         Type of the component
   * \param description  Brief description of what the component does
   * \param author       Component author
   * \param version      Component version
   */
  PhyComponent(std::string name,
              std::string type,
              std::string description,
              std::string author,
              std::string version )
    : ComponentBase(name, type, description, author, version)
      ,numRuns_(0)
  {};


  virtual ~PhyComponent()
  {
    if(numRuns_ > 0)
      LOG(LINFO) << "Average time taken per process() call = " << totalTime_/(float)numRuns_;
  }

  /** Set single input and output buffers for this component
   *
   * \param in   ReadBufferBase pointer
   * \param out  WriteBufferBase pointer
   */
  virtual void setBuffers(ReadBufferBase* in, WriteBufferBase* out)
  {
    inputBuffers.push_back(in);
    outputBuffers.push_back(out);

    std::vector<Port> inPorts = getInputPorts();
    std::vector<Port> outPorts = getOutputPorts();
    namedInputBuffers_[inPorts.front().portName] = in;
    namedOutputBuffers_[outPorts.front().portName] = out;
  };

  /** Set multiple input and output buffers for this component
   *
   * \param in   Vector of ReadBufferBase pointers
   * \param out  Vector of WriteBufferBase pointers
   */
  virtual void setBuffers(std::vector<ReadBufferBase*> in, std::vector<WriteBufferBase*> out)
  {
    inputBuffers = in;
    outputBuffers = out;

    std::vector<Port> inPorts = getInputPorts();
    std::vector<Port> outPorts = getOutputPorts();

    for(int i=0; i<in.size(); ++i)
      namedInputBuffers_[inPorts[i].portName] = in[i];
    for(int i=0; i<out.size(); ++i)
      namedOutputBuffers_[outPorts[i].portName] = out[i];
  };

  /// Called by the PhyEngine to process this component
  virtual void doProcess()
  {
    boost::posix_time::ptime t1(boost::posix_time::microsec_clock::local_time());
    process();
    boost::posix_time::ptime t2(boost::posix_time::microsec_clock::local_time());
    totalTime_ += (t2-t1);
    numRuns_++;
  };

  /// Post a command to this component
  void postCommand(Command command)
  {
      prison_.release(command);
  };

  /// \name To be implemented in derived classes.
  //@{
  virtual void calculateOutputTypes(std::map<std::string, int>& inputTypes,
                                    std::map<std::string, int>& outputTypes) = 0;
  virtual void registerPorts() = 0;
  virtual void initialize() = 0;
  virtual void process() = 0;
  //@}

  /// \name May be implemented in derived classes if required.
  //@{
  virtual void start(){};
  virtual void stop(){};
  //@}

  // Interface for template components
public:
  /** Sets up input and output types.
   *
   * This function is overridden by template components, which will return a new PhyComponent
   * instantiated with the right template parameters.
   * @param inputTypes Vector of input type identifiers, each element represents a port.
   * @param outputTypes Vector of output type identifiers, each element represents a port.
   * @return Pointer to this.
   */
  virtual PhyComponent* setupIO(const std::vector<int>& inputTypes, const std::vector<int>& outputTypes)
  {
    return this;
  }

protected:
  template <class T>
  void getInputDataSet(std::string portName, DataSet<T>*& data)
  {
    ReadBufferBase* b = namedInputBuffers_[portName];
    if( TypeInfo< T >::identifier != b->getTypeIdentifier() )
      throw InvalidDataTypeException("Data type mismatch in getInputDataSet.");
    (dynamic_cast< ReadBuffer<T>* >(b))->getReadData(data);
  }

  template <class T>
  void getOutputDataSet(std::string portName, DataSet<T>*& data, std::size_t size)
  {
    WriteBufferBase* b = namedOutputBuffers_[portName];
    if( TypeInfo< T >::identifier != b->getTypeIdentifier() )
      throw InvalidDataTypeException("Data type mismatch in getOutputDataSet.");
    (dynamic_cast< WriteBuffer<T>* >(b))->getWriteData(data, size);
  }

  template <class T>
  void releaseInputDataSet(std::string portName, DataSet<T>*& data)
  {
    ReadBufferBase* b = namedInputBuffers_[portName];
    if( TypeInfo< T >::identifier != b->getTypeIdentifier() )
      throw InvalidDataTypeException("Data type mismatch in releaseInputDataSet.");
    (dynamic_cast< ReadBuffer<T>* >(b))->releaseReadData(data);
  }

  template <class T>
  void releaseOutputDataSet(std::string portName, DataSet<T>*& data)
  {
    WriteBufferBase* b = namedOutputBuffers_[portName];
    if( TypeInfo< T >::identifier != b->getTypeIdentifier() )
      throw InvalidDataTypeException("Data type mismatch in releaseOutputDataSet.");
    (dynamic_cast< WriteBuffer<T>* >(b))->releaseWriteData(data);
  }

  /// Helper fuction to cast a non-templated base buffer to a templated derived buffer
  template <class T>
  WriteBuffer< T >* castToType( WriteBufferBase* buf ) {
    if( TypeInfo< T >::identifier != buf->getTypeIdentifier() )
    {
      throw InvalidDataTypeException("Attempted to cast WriteBuffer to incorrect type.");
    }
    return dynamic_cast< WriteBuffer<T>* >(buf);
  };

  /// Helper fuction to cast a non-templated base buffer to a templated derived buffer
  template <class T>
  ReadBuffer< T >* castToType( ReadBufferBase* buf ) {
    if( TypeInfo< T >::identifier != buf->getTypeIdentifier() )
    {
      throw InvalidDataTypeException("Attempted to cast WriteBuffer to incorrect type.");
    }
    return dynamic_cast< ReadBuffer<T>* >(buf);
  };

  std::vector<ReadBufferBase*> inputBuffers;    ///< Inputs to this component.
  std::vector<WriteBufferBase*> outputBuffers;  ///< Outputs from this component.

private:
  boost::posix_time::time_duration totalTime_; ///< Time taken in process() so far.
  int numRuns_;                                ///< Number of process() calls so far.

  std::map<std::string, ReadBufferBase*> namedInputBuffers_;
  std::map<std::string, WriteBufferBase*> namedOutputBuffers_;

};

} // namespace iris
#endif //IRISAPI_PHYCOMPONENT_H_

