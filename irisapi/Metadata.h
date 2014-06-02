/**
 * \file Metadata.h
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
 * A metadata class that holds different datatypes in a map
 * accessible through keywords.
 */

#ifndef METADATA_H
#define METADATA_H

#include <boost/utility.hpp>
#include <boost/thread/mutex.hpp>

//! Exception which can be thrown by this class
class MetadataException : public std::exception
{
private:
    std::string message_;
public:
    MetadataException(const std::string &message) throw()
        :exception(), message_(message)
    {};
    virtual const char* what() const throw()
    {
        return message_.c_str();
    };
    virtual ~MetadataException() throw()
    {};
};

class MetadataBase : boost::noncopyable
{
public:
    MetadataBase()
        : description(""),
          source("")
    {}
    virtual ~MetadataBase(){}
    std::string description; ///< Optional, a description of the metadata
    std::string source; ///< Optional, component that created the metadata
};


template<class T>
class Metadata : public MetadataBase
{
public:
    T data; ///< The actual data passed.
};


class MetadataMap
{
    typedef std::map<std::string, boost::shared_ptr<MetadataBase> > MetadataContainer;
public:

    /**
     * Provide a constructor with samplerate and timestamp
     */
    MetadataMap(double s = 0, double t = 0)
    {
        setMetadata("sampleRate", s);
        setMetadata("timeStamp", t);
    }

    /**
     * Destructor
     */
    ~MetadataMap() {}

    /**
     * Copy constructor to initialize mutex properly
     */
    MetadataMap(const MetadataMap& other) : mutex_() {}

    /**
     * Assignment operator
     */
    MetadataMap& operator=(const MetadataMap& orig)
    {
        map_ = orig.map_; // Actual map is copied using default operator
        return *this;
    }

    /**
     * Set metadata.
     *
     * @param key The key
     * @param e The data to store
     */
    template<class T>
    void setMetadata(const std::string key, T e)
    {
        boost::mutex::scoped_lock lock(mutex_);
        if (map_.find(key) != map_.end()) {
            // try to update existing entry
            boost::shared_ptr< Metadata<T> > tmp(boost::dynamic_pointer_cast< Metadata<T> >(map_[key]));
            if (tmp == NULL)
                throw MetadataException("Key already exists with different data type!");

            tmp->data = e;
        } else {
            // add new entry
            boost::shared_ptr<MetadataBase> b(new Metadata<T>);
            boost::dynamic_pointer_cast< Metadata<T> >(b)->data = e;
            map_[key] = b;
        }
    }


    /**
     * Tries to retrieve a specific key from the metadata.
     *
     * @param key The key to look for
     * @param e A reference to the variable that should hold the metadata.
     */
    template<class T>
    void getMetadata(const std::string key, T& e)
    {
        if (not hasMetadata(key))
            throw MetadataException("Requested metadata not present.");

        boost::mutex::scoped_lock lock(mutex_);
        boost::shared_ptr< Metadata<T> > tmp(boost::dynamic_pointer_cast< Metadata<T> >(map_[key]));
        if (tmp == NULL)
            throw MetadataException("Failed to cast metadata to desired type.");
        e = tmp->data;
    }


    /**
     * Tries to delete a specific key from the metadata.
     *
     * @param The key to delete
     * @return True if key could be deleted, false otherwise
     */
    bool removeMetadata(const std::string key)
    {
        boost::mutex::scoped_lock lock(mutex_);
        if (hasMetadata(key)) {
            map_.erase(key);
            return true;
        }
        return false;
    }


    /**
     * Checks whether there is metadata at all.
     *
     * @return True if metadata is available, false otherwise
     */
    bool hasMetadata() const {
        boost::mutex::scoped_lock lock(mutex_);
        return (map_.empty() ? false : true);
    }


    /**
     * Checks for a specific key inside metadata.
     *
     * @param The key to look for
     * @return True if metadata is available, false otherwise
     */
    bool hasMetadata(const std::string key) const {
        boost::mutex::scoped_lock lock(mutex_);
        return (map_.find(key) == map_.end() ? false : true);
    }


    /**
     * Return the number of metadata items.
     *
     * @return The number of items stored
     */
    size_t getMetadataCount() const {
        boost::mutex::scoped_lock lock(mutex_);
        return map_.size();
    }

private:
    MetadataContainer map_;       ///< The container for storing the data.
    mutable boost::mutex mutex_;  ///< Mutex used for thread safety.
};

#endif // METADATA_H
