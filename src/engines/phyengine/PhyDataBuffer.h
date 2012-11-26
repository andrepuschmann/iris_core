/**
 * \file PhyDataBuffer.h
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
 * The PhyDataBuffer which exists on links internal to a PhyEngine.
 */

#ifndef PHYDATABUFFER_H_
#define PHYDATABUFFER_H_

#include "irisapi/DataBufferInterfaces.h"
#include "irisapi/Exceptions.h"
#include "irisapi/TypeInfo.h"

namespace iris
{

/*!
*   \brief The PhyDataBuffer class implements a buffer which exists between two IRIS components within a single
*  PhyEngine.
*
*  The buffer consists of a number of DataSet objects which can be written and read by the components.
*  Components can get a DataSet to write to by calling GetWriteSet(). When finished writing, the component
*  releases the DataSet by calling ReleaseWriteSet().
*  Components can get a DataSet to read from by calling GetReadSet(). When finished reading, the component
*  releases the DataSet by calling ReleaseReadSet().
*  The PhyDataBuffer is NOT thread-safe. It should only be used within a single PhyEngine. It is non-blocking.
*  If a component wishes to write more data sets than are available, the PhyDataBuffer will grow.
*/
template <typename T>
class PhyDataBuffer : public ReadBuffer<T>, public WriteBuffer<T>
{
public:

  /*!
  *   \brief Constructor
  *
  *   \param dataBufferLength   number of DataSets in the buffer
  */
  explicit PhyDataBuffer(int dataBufferLength = 2) throw (InvalidDataTypeException)
    :buffer_(dataBufferLength, DataSet<T>()),
    isReadLocked_(false),
    isWriteLocked_(false),
    readIndex_(0),
    writeIndex_(0),
    notEmpty_(false),
    notFull_(true)
  {
    //Set the type identifier for this buffer
    typeIdentifier = TypeInfo<T>::identifier;
    if( typeIdentifier == -1)
      throw InvalidDataTypeException("Data type not supported");
  };

  virtual ~PhyDataBuffer(){};


  //! Set the link description (with info on source and sink components) for this buffer
  void setLinkDescription(LinkDescription desc)
  {
    linkDesc = desc;
  };


  //! Get the link description for this buffer
  LinkDescription getLinkDescription() const
  {
    return linkDesc;
  };

  //! Get the identifier for the data type of this buffer
  int getTypeIdentifier() const
  {
    return typeIdentifier;
  };

  //! Is there any data in this buffer?
  bool hasData() const
  {
    return is_not_empty();
  }

  /*!
  *   \brief Get the next DataSet to read
  *
  *   \param setPtr   A DataSet pointer which will be set by the buffer
  */
  void getReadData(DataSet<T>*& setPtr) throw(DataBufferReleaseException, boost::thread_interrupted)
  {
    if(isReadLocked_)
      throw DataBufferReleaseException("getReadData() called before previous DataSet was released");
    isReadLocked_ = true;
    setPtr = &buffer_[readIndex_];
  };

  /*!
  *   \brief Get the next DataSet to be written
  *
  *   \param setPtr   A DataSet pointer which will be set by the buffer
  *   \param size   The number of elements required in the DataSet
  */
  void getWriteData(DataSet<T>*& setPtr, std::size_t size) throw(DataBufferReleaseException, boost::thread_interrupted)
  {
    if(isWriteLocked_)
      throw DataBufferReleaseException("getWriteData() called before previous DataSet was released");
    if(!notFull_)
    {
      //If buffer is full, add a new DataSet
      buffer_.push_back(DataSet<T>());
      writeIndex_ = buffer_.size()-1;
    }
    isWriteLocked_ = true;
    if(buffer_[writeIndex_].data.size() != size)
      buffer_[writeIndex_].data.resize(size);
    buffer_[writeIndex_].timeStamp = 0;
    setPtr = &buffer_[writeIndex_];
  };

  /*!
  *   \brief Release a read DataSet
  *
  *   \param setPtr   A pointer to the DataSet to be released
  */
  void releaseReadData(DataSet<T>*& setPtr)
  {
    if(++readIndex_ == buffer_.size())
      readIndex_ = 0;
    if(readIndex_ == writeIndex_)
      notEmpty_ = false;
    notFull_ = true;
    isReadLocked_ = false;
    setPtr = NULL;
  };

  /*!
  *   \brief Release a write DataSet
  *
  *   \param setPtr   A pointer to the DataSet to be released
  */
  void releaseWriteData(DataSet<T>*& setPtr)
  {
    if(++writeIndex_ == buffer_.size())
      writeIndex_ = 0;
    if(readIndex_ == writeIndex_)
      notFull_ = false;
    notEmpty_ = true;
    isWriteLocked_ = false;
    setPtr = NULL;
  };

private:
  //! The source and sink component details for this DataBuffer
  LinkDescription linkDesc;

  //! The data type of this buffer
  int typeIdentifier;

  //! The vector of DataSets
  std::vector< DataSet<T> > buffer_;

  bool isReadLocked_;
  bool isWriteLocked_;

  std::size_t readIndex_;
  std::size_t writeIndex_;

  bool notEmpty_;
  bool notFull_;

  bool is_not_empty() const { return notEmpty_; }
  bool is_not_full() const { return notFull_; }

};

} /* namespace iris */


#endif /* PHYDATABUFFER_H_ */
