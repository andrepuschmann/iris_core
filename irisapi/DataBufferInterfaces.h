/**
 * \file DataBufferInterfaces.h
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
 * Interfaces to the DataBuffer class for reading and writing clients.
 */

#ifndef IRISAPI_DATABUFFERINTERFACES_H_
#define IRISAPI_DATABUFFERINTERFACES_H_

#include <boost/utility.hpp>
#include <vector>
#include <irisapi/LinkDescription.h>
#include "Metadata.h"

namespace iris
{

/** The DataSet struct wraps a block of data being used within the IRIS system.
*
*  Each DataBuffer between two components will contain a vector of DataSets which can be written and read
*  by those components.
*/
template <typename T>
struct DataSet{
  std::vector< T, std::allocator<T> > data;  //We can change this to another container or
                                             //use a custom allocator if we want
  MetadataMap metadata;

  //! Constructor initializes our variables
  DataSet(int l=10, double s=0, double t=0)
    :data(l), metadata(s, t){}
};

/** The DataBufferBase class allows us to store vectors of DataBuffers of different types
*
*  The DataBuffer class inherits from this base class. The typeIdentifier specifies
*   the type of the derived class.
*/
class DataBufferBase
  : boost::noncopyable
{
public:
  virtual ~DataBufferBase(){};
  virtual int getTypeIdentifier() const = 0;
  virtual void setLinkDescription(LinkDescription desc) = 0;
  virtual LinkDescription getLinkDescription() const = 0;
  virtual bool hasData() const = 0;
};

/** The ReadBufferBase class allows us to store vectors of ReadBuffers of different types
*
*  The ReadBuffer and DataBuffer inherit from this base class. The typeIdentifier specifies
*   the type of the derived classes.
*/
class ReadBufferBase
  : public virtual DataBufferBase
{};

/** The WriteBufferBase class allows us to store vectors of WriteBuffers of different types
*
*  The WriteBuffer and DataBuffer inherit from this base class. The typeIdentifier specifies
*   the type of the derived classes.
*/
class WriteBufferBase
  : public virtual DataBufferBase
{};

/// The ReadBuffer interface limits the use of a DataBuffer for a writing client.
template <typename T>
class ReadBuffer
  : public virtual ReadBufferBase
{
public:
  virtual ~ReadBuffer(){};
  virtual void getReadData(DataSet<T>*& setPtr) = 0;
  virtual void releaseReadData(DataSet<T>*& setPtr) = 0;
};

/// The WriteBuffer interface limits the use of a DataBuffer for a writing client.
template <typename T>
class WriteBuffer
  : public virtual WriteBufferBase
{
public:
  virtual ~WriteBuffer(){};
  virtual void getWriteData(DataSet<T>*& setPtr, std::size_t size) = 0;
  virtual void releaseWriteData(DataSet<T>*& setPtr) = 0;
};

} // namespace iris

#endif // IRISAPI_DATABUFFERINTERFACES_H_
