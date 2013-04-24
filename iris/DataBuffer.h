/**
 * \file DataBuffer.h
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
 * The DataBuffer which exists on all links between components in Iris.
 */

#ifndef IRIS_DATABUFFER_H_
#define IRIS_DATABUFFER_H_

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#include "irisapi/DataBufferInterfaces.h"
#include "irisapi/Exceptions.h"
#include "irisapi/TypeInfo.h"

namespace iris
{

/** The DataBuffer class implements a buffer which exists between two
 * Iris components in different engines.
 *
 * The buffer consists of a number of DataSet objects which can be written
 * and read by the components. Components can get a DataSet to write to by
 * calling GetWriteSet(). When finished writing, the component releases the
 * DataSet by calling ReleaseWriteSet(). Components can get a DataSet to read
 * from by calling GetReadSet(). When finished reading, the component releases
 * the DataSet by calling ReleaseReadSet(). The DataBuffer is thread-safe. In
 * the event that the buffer is full, GetWriteSet() will block. In the event
 * that the buffer is empty, GetReadSet() will block.
 */
template <typename T>
class DataBuffer : public ReadBuffer<T>, public WriteBuffer<T>
{
public:

  /** Constructor
  *
  *   \param dataBufferLength   Number of DataSets in the buffer.
  */
  explicit DataBuffer(int dataBufferLength = 2)
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

  virtual ~DataBuffer(){};


  /** Set the link description (with info on source and sink components)
   * for this buffer
   *
   * @param desc    Description of the link.
   */
  void setLinkDescription(LinkDescription desc)
  {
    linkDesc = desc;
  };


  /// Get the link description for this buffer
  LinkDescription getLinkDescription() const
  {
    return linkDesc;
  };

  /// Get the identifier for the data type of this buffer
  int getTypeIdentifier() const
  {
    return typeIdentifier;
  };

  /// Is there any data in this buffer?
  bool hasData() const
  {
    boost::mutex::scoped_lock lock(mutex_);
    return is_not_empty();
  }

  /** Get the next DataSet to read.
  *
  *   \param setPtr   A DataSet pointer which will be set by the buffer.
  */
  void getReadData(DataSet<T>*& setPtr)
  {
    boost::mutex::scoped_lock lock(mutex_);
    if(isReadLocked_)
      throw DataBufferReleaseException("getReadData() called before previous DataSet was released");
    notEmptyCond_.wait(lock, boost::bind(&DataBuffer<T>::is_not_empty, this));
    isReadLocked_ = true;
    setPtr = &buffer_[readIndex_];
  };

  /** Get the next DataSet to be written.
  *
  *   \param setPtr   A DataSet pointer which will be set by the buffer
  *   \param size     The number of elements required in the DataSet
  */
  void getWriteData(DataSet<T>*& setPtr, std::size_t size)
  {
    boost::mutex::scoped_lock lock(mutex_);
    if(isWriteLocked_)
      throw DataBufferReleaseException("getWriteData() called before previous DataSet was released");
    notFullCond_.wait(lock, boost::bind(&DataBuffer<T>::is_not_full, this));
    isWriteLocked_ = true;
    if(buffer_[writeIndex_].data.size() != size)
      buffer_[writeIndex_].data.resize(size);
    buffer_[writeIndex_].timeStamp = 0;
    setPtr = &buffer_[writeIndex_];
  };

  /** Release a read DataSet.
  *
  *   \param setPtr   A pointer to the DataSet to be released.
  */
  void releaseReadData(DataSet<T>*& setPtr)
  {
    boost::mutex::scoped_lock lock(mutex_);
    if(++readIndex_ == buffer_.size())
      readIndex_ = 0;
    if(readIndex_ == writeIndex_)
      notEmpty_ = false;
    notFull_ = true;
    notFullCond_.notify_one();
    isReadLocked_ = false;
    setPtr = NULL;
  };

  /** Release a write DataSet.
  *
  *   \param setPtr   A pointer to the DataSet to be released
  */
  void releaseWriteData(DataSet<T>*& setPtr)
  {
    boost::mutex::scoped_lock lock(mutex_);
    if(++writeIndex_ == buffer_.size())
      writeIndex_ = 0;
    if(readIndex_ == writeIndex_)
      notFull_ = false;
    notEmpty_ = true;
    notEmptyCond_.notify_one();
    isWriteLocked_ = false;
    setPtr = NULL;
  };

private:
  bool is_not_empty() const { return notEmpty_; }
  bool is_not_full() const { return notFull_; }

  LinkDescription linkDesc; ///< Info about readers and writers of this buffer.
  int typeIdentifier;       ///< The data type of this buffer
  std::vector< DataSet<T> > buffer_;    ///< The vector of DataSets

  bool isReadLocked_;
  bool isWriteLocked_;

  std::size_t readIndex_;
  std::size_t writeIndex_;

  bool notEmpty_;
  bool notFull_;

  mutable boost::mutex mutex_;
  boost::condition notEmptyCond_;
  boost::condition notFullCond_;

};

} /* namespace iris */


#endif /* IRIS_DATABUFFER_H_ */
