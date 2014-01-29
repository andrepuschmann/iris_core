/**
 * \file Logging.h
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
 * Simple logging for Iris.
 */

#ifndef IRISAPI_LOGGING_H_
#define IRISAPI_LOGGING_H_

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
  //boost::date_time uses windows.h - need to define the following to avoid socket and macro issues
  #define WIN32_LEAN_AND_MEAN
  //#define NOMINMAX - already defined on command line (see top-level CMakeLists.txt)
#endif

#include <sstream>
#include <string>
#include <cstdio>

#include <boost/thread/mutex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/utility.hpp>

namespace iris
{

/// Get the current time
inline std::string NowTime();

/// The log levels available for logging
enum LogLevel {LDEBUG, LINFO, LWARNING, LERROR, LFATAL};

/// Convert a log level to a color
inline std::string ToColor(LogLevel level);

/** The logging policy. This class determines the output streams of the logging element.
*
*   Logger objects use a policy to determine the output streams.
*   The default output stream is stderr. A log file can be created by setting the file stream.
*/
class LoggingPolicy
{
private:
  boost::mutex mutex_;
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

  void output(const std::string& msg, const LogLevel level)
  {
    boost::mutex::scoped_lock lock(mutex_);

    // Output to console
    if(consoleStream)
    {
      std::string tmp = ToColor(level) + msg + "\033[0m";
      fprintf(consoleStream, "%s", tmp.c_str());
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
    boost::mutex::scoped_lock lock(mutex_);
    fileStream = pFile;
  }

  /// Get and/or set the reporting level - anything below this level is ignored
  LogLevel& ReportingLevel()
  {
    return reportingLevel;
  }

};

/** The logging element.
*
*  In your code, use the LOG macro as follows:
*  \code LOG(LDEBUG) << "This is an example log message with variable: " << someVariable; \endcode
*  The log levels are (in ascending order) LDEBUG, LINFO, LWARNING, LERROR, LFATAL.
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

  static std::string ToString(LogLevel level);
  static LogLevel FromString(const std::string& level);
  static LoggingPolicy*& getPolicy();

protected:
  std::ostringstream os;
  LogLevel level;
};

/// Constructor sets LoggingPolicy if required
inline Logger::Logger()
{
}

/** Get the output log stream
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
  
  this->level = level;

  return os;
}

/// Destructor flushes the log stream
inline Logger::~Logger()
{
  os << std::endl;
  getPolicy()->output(os.str(), level);
}

/// Get ref to pointer to the current LoggingPolicy
inline LoggingPolicy*& Logger::getPolicy()
{
  static LoggingPolicy* thePolicyPtr = LoggingPolicy::getPolicyInstance();
  return thePolicyPtr;
}

/// Get the string for a given level
inline std::string Logger::ToString(LogLevel level)
{
  static const char* const buffer[] = {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};
  return buffer[level];
}

/// Find the level associated with a given string
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

/// The lowest log level to be reported
#ifndef LOG_MIN_LEVEL
#define LOG_MIN_LEVEL ::iris::LDEBUG
#endif

/**  Macro which checks the loglevel and outputs to streams
 *
 *  This is the main interface for performing logging.
 *
 *   Usage:
 *   \code LOG(LDEBUG) << "this is a log message"; \endcode
 */
#define LOG(level) \
  if (::iris::level < LOG_MIN_LEVEL) ;\
  else if (::iris::level < ::iris::Logger::getPolicy()->ReportingLevel()) ; \
  else ::iris::Logger().Get(::iris::level) << getName() << ": "

///  Cross-platform time access using boost::date_time
inline std::string NowTime()
{
  using namespace boost::posix_time;
  return to_simple_string(microsec_clock::local_time());
}

/// Get the escape sequence to set console color for a given level
inline std::string ToColor(LogLevel level)
{
  static const char* const buffer[] = {"\033[36m" /* DEBUG is cyan */,
                                         "\033[0m" /* INFO is default */,
                                         "\033[33m" /* WARNING is yellow */,
                                         "\033[31m" /* ERROR is red */,
                                         "\033[31m\033[1m" /* FATAL is red, bold */};
  return buffer[level];
}

} // namespace iris

#endif // IRISAPI_LOGGING_H_
