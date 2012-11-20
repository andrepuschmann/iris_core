/**
 * \file MemoryManager.h
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
 * The MemoryManager provides a single point for allocation and
 * deallocation of memory for the DataBuffers within the Iris system.
 */

#ifndef IRIS_MEMORYMANAGER_H_
#define IRIS_MEMORYMANAGER_H_

#include <boost/intrusive/list.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <iostream>

#include "irisapi/Exceptions.h"

namespace iris
{

namespace intr = boost::intrusive;

// tags use to distinguish the hooks for both lists
struct tag1;
struct tag2;

typedef intr::list_base_hook< intr::tag<tag1>, intr::link_mode<intr::normal_link>, intr::constant_time_size<false> > basehook1;
typedef intr::list_base_hook< intr::tag<tag2>, intr::link_mode<intr::normal_link>, intr::constant_time_size<false> > basehook2;

//! The MemoryNode struct wraps an allocated block of memory within the MemoryManager.
template <unsigned Alignment = 128>
struct MemoryNode : public basehook1, public basehook2
{
    void *memory;
    std::size_t size;

    //! Constructor
    MemoryNode(std::size_t size) : memory(NULL), size(size)
    {
#ifdef WIN32
        memory = _aligned_malloc(size, Alignment);
        if (memory == NULL)
#else
        if (posix_memalign(&memory, Alignment, size) != 0)
#endif
        {
            size = 0;
        }
    }

    //! Copy constructor
    MemoryNode(const MemoryNode<Alignment>& rhs) : memory(rhs.memory), size(rhs.size) {}

    //! Destructor
    ~MemoryNode() {
#ifdef WIN32
        if (size != 0) _aligned_free(memory);
#else
        if (size != 0) free(memory);
#endif
    }

private:
    // No default constructor
    MemoryNode() {};

};

/** \class MemoryManager
 *  \brief The MemoryManager provides a single point for allocation and
 *  deallocation of memory for the DataBuffers within the IRIS system.
 *  In this way, we can ensure correct memory alignment for optimized
 *  instructions and can reduce the danger of memory leaks. MemoryManager
 *  keeps two linked lists of memory nodes (both referencing the same chunks
 *  of memory), one list ordered by chunk size, the other ordered by pointer
 *  address. Uses boost::intrusive lists, which basically add the list hooks
 *  into the nodes, instead of copying the nodes into a container.
 */
class MemoryManager
{
private:
    //! The size used for memory alignment
    const static int ALIGNMENT_SIZE = 128;

    //! A memory node
    typedef MemoryNode<ALIGNMENT_SIZE> memnode;

    //! Type for list of allocated nodes
    typedef intr::list< memnode, intr::base_hook< basehook1 > > allocnodelist;
    //! Type for list of free nodes
    typedef intr::list< memnode, intr::base_hook< basehook2 > > availnodelist;

    //! List of allocated nodes
    allocnodelist allocated;
    //! List of free nodes
    availnodelist available;

    //! Iterator types
    typedef allocnodelist::iterator alloc_it;
    typedef availnodelist::iterator avail_it;

    //! To protect shared access to the MemoryManager
    boost::mutex memoryMutex;

    //! Counts total memory used
    std::size_t totalMemoryRequested;

public:
    //! Ctor
    MemoryManager();
    //! Dtor
    ~MemoryManager();

    //! Locks size bytes of memory and returns a pointer to it
    void* allocateBlock(std::size_t size) throw(OutOfMemoryException);

    //! Releases the specified memory
    void releaseBlock(void* mem);

    //! Returns the total amount of memory being used by the manager in bytes
    std::size_t getTotalMemoryUsed();

    //! Returns the total amount of locked memory
    std::size_t getTotalMemoryLocked();

    //! Returns the total amount of memory that has been requested
    std::size_t getTotalMemoryRequested();

    //! Returns the size of the specified memory location
    std::size_t getBlockSize(void* mem);

    //! Resets the memory manager by unlocking and freeing all memory
    void reset();

    //! Utility function for debugging
    void printBlockInfo();
};

} // namespace iris

#endif // IRIS_MEMORYMANAGER_H_
