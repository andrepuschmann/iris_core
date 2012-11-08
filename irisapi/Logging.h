/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * @file Logging.h
 * Simple logging for IRIS 2.0.
 *
 *  Created on: 18-Mar-2009
 *  Created by: suttonp
 *  $Revision: 1002 $
 *  $LastChangedDate: 2010-08-11 15:56:49 +0100 (Wed, 11 Aug 2010) $
 *  $LastChangedBy: suttonp $
 */

#ifndef IRIS_LOGGING_H_
#define IRIS_LOGGING_H_

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    //boost::date_time uses windows.h - need to define the following to avoid socket and macro issues
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
#endif

#include <sstream>
#include <string>
#include <cstdio>

#include <boost/thread/mutex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/utility.hpp>

namespace iris
{

//! Get the current time
inline std::string NowTime();

//! The log levels available for logging
enum LogLevel {LDEBUG, LINFO, LWARNING, LERROR, LFATAL};

/*!
*   \brief  The logging policy. This class determines the output streams of the logging element.
*
*   Logger objects use a policy to determine the output streams.
*   The default output stream is stderr. A log file can be created by setting the file stream.
*/
class LoggingPolicy
{
private:
    boost::mutex d_mutex;
    FILE* consoleStream;
    FILE* fileStream;
    LogLevel reportingLevel;

public:
    LoggingPolicy()
        : consoleStream(stderr),
        fileStream(NULL),
        reportingLevel(LDEBUG)
    {}

    static LoggingPolicy* getPolicyInstance()
    {
        static LoggingPolicy thePolicy;
        return &thePolicy;
    }

    void output(const std::string& msg)
    {
        boost::mutex::scoped_lock lock(d_mutex);

        // Output to console
        if(consoleStream)
        {
            fprintf(consoleStream, "%s", msg.c_str());
            fflush(consoleStream);
        }

        //Output to file
        if(fileStream)
        {
            std::string tmp = NowTime() + " " + msg;
            fprintf(fileStream, "%s", tmp.c_str());
            fflush(fileStream);
        }
    }

    void setFileStream(FILE* pFile)
    {
        boost::mutex::scoped_lock lock(d_mutex);
        fileStream = pFile;
    }

    //! Get and/or set the reporting level - anything below this level is ignored
    LogLevel& ReportingLevel()
    {
        return reportingLevel;
    }

};

/*!
*   \brief  The logging element.
*
*	In your code, use the LOG macro as follows:
*	\code LOG(LDEBUG) << "This is an example log message with variable: " << someVariable; \endcode
*	The log levels are (in ascending order) LDEBUG, LINFO, LWARNING, LERROR, LFATAL.
*   Each time the LOG macro is called, a temporary Logger object is created. The
*   Logger objects output according to their static LoggingPolicy pointer.
*   The LoggingPolicy output streams are flushed when the temporary Logger object
*   is destroyed.
*   The operation of Logger objects can be altered by resetting their LoggingPolicy pointer.
*/
class Logger : boost::noncopyable
{
public:
    Logger();
    ~Logger();
    std::ostringstream& Get(LogLevel level = LINFO);
public:
    static std::string ToString(LogLevel level);
    static LogLevel FromString(const std::string& level);
    static LoggingPolicy*& getPolicy();
protected:
    std::ostringstream os;
};

//! Constructor sets LoggingPolicy if required
inline Logger::Logger()
{
}

/*!
*   \brief  Get the output log stream
*
*   \param level The log level for the message.
*/
inline std::ostringstream& Logger::Get(LogLevel level)
{
    // put in brackets and append spaces to make sure length is 8
    std::string tmp = "[";
    tmp += ToString(level);
    tmp += "]";
    tmp += std::string(sizeof("WARNING")+1 - tmp.size(), ' ');

    os << tmp << " ";

    return os;
}

//! Destructor flushes the log stream
inline Logger::~Logger()
{
    os << std::endl;
    getPolicy()->output(os.str());
}

inline LoggingPolicy*& Logger::getPolicy()
{
    static LoggingPolicy* thePolicyPtr = LoggingPolicy::getPolicyInstance();
    return thePolicyPtr;
}

//! Get the string for a given level
inline std::string Logger::ToString(LogLevel level)
{
	static const char* const buffer[] = {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};
    return buffer[level];
}

//! Find the level associated with a given string
inline LogLevel Logger::FromString(const std::string& level)
{
    if (level == "DEBUG")
        return LDEBUG;
    if (level == "INFO")
        return LINFO;
    if (level == "WARNING")
        return LWARNING;
    if (level == "ERROR")
        return LERROR;
    if (level == "FATAL")
        return LFATAL;
    Logger().Get(LWARNING) << "Unknown logging level '" << level << "'. Using INFO level as default.";
    return LINFO;
}

//! The lowest log level to be reported
#ifndef LOG_MIN_LEVEL
#define LOG_MIN_LEVEL ::iris::LDEBUG
#endif

/*  Macro which checks the loglevel and outputs to streams
 *
 *  This is the main interface for performing logging.

 *   Usage:
 *   \code LOG(LDEBUG) << "this is a log message"; \endcode
 */
#define LOG(level) \
    if (::iris::level < LOG_MIN_LEVEL) ;\
    else if (::iris::level < ::iris::Logger::getPolicy()->ReportingLevel()) ; \
    else ::iris::Logger().Get(::iris::level) << getName() << ": "

/*  Cross-platform time access using boost::date_time
 *
 */
inline std::string NowTime()
{
    using namespace boost::posix_time;
    return to_simple_string(microsec_clock::local_time());
}

} /* namespace iris */

#endif /* IRIS_LOGGING_H_ */
