/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file MemoryManager.cpp
 * Implementation of MemoryManager class.
 *
 *  Created on: 22-Sep-2008
 *  Created by: jlotze
 *  $Revision: 1316 $
 *  $LastChangedDate: 2011-09-13 12:41:16 +0100 (Tue, 13 Sep 2011) $
 *  $LastChangedBy: suttonp $
 *  Adapted on: 1-Dec-2008
 *  Created by: suttonp
 *  $Revision: 1316 $
 *  $LastChangedDate: 2011-09-13 12:41:16 +0100 (Tue, 13 Sep 2011) $
 *  $LastChangedBy: suttonp $
 */

#include "iris/MemoryManager.h"


namespace iris
{
    //! Ctor
    MemoryManager::MemoryManager()
        : totalMemoryRequested(0) 
    {}

    //! Dtor
    MemoryManager::~MemoryManager() 
    { 
        if (!allocated.empty() || !available.empty()) 
            reset(); 
    };

    //! Locks size bytes of memory and returns a pointer to it
    void* MemoryManager::allocateBlock(std::size_t size) throw(OutOfMemoryException)
    {
        avail_it lwr, last;

        boost::mutex::scoped_lock lock(memoryMutex);

        totalMemoryRequested += size;

        // search for block big enough in available list
        last = available.end();
        for (lwr = available.begin(); lwr != last; ++lwr)
        {
            if (lwr->size >= size)
            {
                // all allocated ones are in the allocated list anyway,
                // even if available
                //allocated.push_back(*lwr);
                void* tmp = lwr->memory;
                available.erase(lwr);

                return tmp;
            }
        }

        // none available
        memnode * node = new memnode(size);
        if(0 == node->size)
        {
            throw OutOfMemoryException("Failed to allocate new memory");
        }

        // add it to the right position (order by memory address)
        alloc_it it = allocated.begin();
        while ((it != allocated.end()) && (it->memory < node->memory))
        {
            ++it;
        }
        allocated.insert(it, *node);
        
        return node->memory;

    }

    //! Releases the specified memory
    void MemoryManager::releaseBlock(void* mem)
    {
        alloc_it it;

        boost::mutex::scoped_lock lock(memoryMutex);

        alloc_it last = allocated.end();
        for (it = allocated.begin(); it != last; ++it)
        {
            if (it->memory == mem)
            {
                // find good position
                avail_it avit = available.begin();
                while ((avit != available.end()) && (avit->size < it->size))
                    ++avit;
                available.insert(avit, *it);
                return;
            }
        }
    }

    //! Returns the total amount of memory being used by the manager in bytes
    std::size_t MemoryManager::getTotalMemoryUsed()
    {
        std::size_t usedMem = 0;

        for (alloc_it it = allocated.begin(); it != allocated.end(); ++it)
        {
            usedMem += it->size;
        }

        return usedMem;
    }

    //! Returns the total amount of locked memory
    std::size_t MemoryManager::getTotalMemoryLocked()
    {
        std::size_t availableMem = 0;

        for (avail_it it = available.begin(); it != available.end(); ++it)
        {
            availableMem += it->size;
        }

        return getTotalMemoryUsed() - availableMem;
    }

    //! Returns the total amount of memory that has been requested
    std::size_t MemoryManager::getTotalMemoryRequested()
    {
        return totalMemoryRequested;
    }

    //! Returns the size of the specified memory location
    std::size_t MemoryManager::getBlockSize(void* mem)
    {
        alloc_it it;

        boost::mutex::scoped_lock lock(memoryMutex);

        alloc_it last = allocated.end();
        for (it = allocated.begin(); it != last; ++it)
        {
            if (it->memory == mem)
            {
                return it->size;
            }
        }

        // should never get here
        return 0;
    }

    //! Resets the memory manager by unlocking and freeing all memory
    void MemoryManager::reset()
    {
        available.clear();
        while ( !allocated.empty() )
        {
            memnode& node = allocated.front();
            allocated.pop_front();
            delete &node;
        }

        totalMemoryRequested = 0;
    }

    //! Utility function for debugging
    void MemoryManager::printBlockInfo()
    {
        typedef std::map<std::size_t, unsigned> maptype;
        maptype table;
        for (alloc_it it = allocated.begin(); it != allocated.end(); ++it)
        {
            maptype::iterator mit = table.find(it->size);
            if (mit == table.end())
            {
                table[it->size] = 1;
            }
            else
            {
                table[it->size] += 1;
            }
        }
        for (avail_it it = available.begin(); it != available.end(); ++it)
        {
            maptype::iterator mit = table.find(it->size);
            if (mit == table.end())
            {
                table[it->size] = 1;
            }
            else
            {
                table[it->size] += 1;
            }
        }
        for (maptype::iterator mit = table.begin(); mit != table.end(); ++mit)
        {
            std::cout << "size : " << mit->first << ", number: " << mit->second << std::endl;
        }

    }

}
