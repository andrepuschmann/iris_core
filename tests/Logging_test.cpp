/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/*
 * file TestLogging.cpp
 * Test file for Logging Element
 *
 *  Created on: 05-Dec-2008
 *  Created by: hlahlou
 *  $Revision: 1346 $
 *  $LastChangedDate: 2011-09-19 18:24:26 +0100 (Mon, 19 Sep 2011) $
 *  $LastChangedBy: suttonp $
 */

#define BOOST_TEST_MODULE LoggingTest

#include <boost/test/unit_test.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/thread_time.hpp>

#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>

#include "irisapi/Logging.h"


BOOST_AUTO_TEST_SUITE (Logging)

BOOST_AUTO_TEST_CASE(Init)
{
    using namespace iris;

    // static settings

    LoggingPolicy::getPolicyInstance()->ReportingLevel() = LDEBUG;
    BOOST_CHECK_EQUAL(LoggingPolicy::getPolicyInstance()->ReportingLevel(), LDEBUG);

    LoggingPolicy::getPolicyInstance()->ReportingLevel() = LINFO;
    BOOST_CHECK_EQUAL(LoggingPolicy::getPolicyInstance()->ReportingLevel(), LINFO);

    LoggingPolicy::getPolicyInstance()->ReportingLevel() = LWARNING;
    BOOST_CHECK_EQUAL(LoggingPolicy::getPolicyInstance()->ReportingLevel(), LWARNING);

    LoggingPolicy::getPolicyInstance()->ReportingLevel() = LERROR;
    BOOST_CHECK_EQUAL(LoggingPolicy::getPolicyInstance()->ReportingLevel(), LERROR);

    LoggingPolicy::getPolicyInstance()->ReportingLevel() = LFATAL;
    BOOST_CHECK_EQUAL(LoggingPolicy::getPolicyInstance()->ReportingLevel(), LFATAL);

    BOOST_CHECK_EQUAL(Logger::ToString(LDEBUG), "DEBUG");
    BOOST_CHECK_EQUAL(Logger::ToString(LINFO), "INFO");
    BOOST_CHECK_EQUAL(Logger::ToString(LWARNING), "WARNING");
    BOOST_CHECK_EQUAL(Logger::ToString(LERROR), "ERROR");
    BOOST_CHECK_EQUAL(Logger::ToString(LFATAL), "FATAL");

    BOOST_CHECK_EQUAL(Logger::FromString("DEBUG"), LDEBUG);
    BOOST_CHECK_EQUAL(Logger::FromString("INFO"), LINFO);
    BOOST_CHECK_EQUAL(Logger::FromString("WARNING"), LWARNING);
    BOOST_CHECK_EQUAL(Logger::FromString("ERROR"), LERROR);
    BOOST_CHECK_EQUAL(Logger::FromString("FATAL"), LFATAL);
}

// NOTE: for checking the file
static unsigned counter = 0;
#define LINE "(line " << counter+1 << ") "

std::string getName()
{
    return "TestLogging";
}

BOOST_AUTO_TEST_CASE(SingleThread)
{
  
    using namespace std;
    using namespace iris;

    {
        // clear log.txt
        std::ofstream logfile("iris2.log");
        logfile.close();
    }

    FILE* pFile = fopen("iris2.log", "a");
    BOOST_REQUIRE(pFile!=NULL);
    LoggingPolicy::getPolicyInstance()->setFileStream(pFile);

    LoggingPolicy::getPolicyInstance()->ReportingLevel() = LDEBUG;
    LOG(LDEBUG) << LINE << "This should be logged"; counter++;
    LOG(LINFO) << LINE << "This should be logged"; counter++;
    LOG(LWARNING) << LINE << "This should be logged"; counter++;
    LOG(LERROR) << LINE << "This should be logged"; counter++;
    LOG(LFATAL) << LINE << "This should be logged"; counter++;

    LoggingPolicy::getPolicyInstance()->ReportingLevel() = LINFO;
    LOG(LDEBUG) << LINE << "This should not be logged"; 
    LOG(LINFO) << LINE << "This should be logged"; counter++;
    LOG(LWARNING) << LINE << "This should be logged"; counter++;
    LOG(LERROR) << LINE << "This should be logged"; counter++;
    LOG(LFATAL) << LINE << "This should be logged"; counter++;

    LoggingPolicy::getPolicyInstance()->ReportingLevel() = LWARNING;
    LOG(LDEBUG) << LINE << "This should not be logged";
    LOG(LINFO) << LINE << "This should not be logged";
    LOG(LWARNING) << LINE << "This should be logged"; counter++;
    LOG(LERROR) << LINE << "This should be logged"; counter++;
    LOG(LFATAL) << LINE << "This should be logged"; counter++;

    LoggingPolicy::getPolicyInstance()->ReportingLevel() = LERROR;
    LOG(LDEBUG) << LINE << "This should not be logged";
    LOG(LINFO) << LINE << "This should not be logged";
    LOG(LWARNING) << LINE << "This should not be logged";
    LOG(LERROR) << LINE << "This should be logged"; counter++;
    LOG(LFATAL) << LINE << "This should be logged"; counter++;

    LoggingPolicy::getPolicyInstance()->ReportingLevel() = LFATAL;
    LOG(LDEBUG) << LINE << "This should not be logged";
    LOG(LINFO) << LINE << "This should not be logged";
    LOG(LWARNING)<< LINE  << "This should not be logged";
    LOG(LERROR) << LINE << "This should not be logged";
    LOG(LFATAL) << LINE << "This should be logged"; counter++;

    LoggingPolicy::getPolicyInstance()->setFileStream(NULL);  // switch off logging to file

    LOG(LFATAL) << LINE << "This should be logged to console but not to file";

    LoggingPolicy::getPolicyInstance()->setFileStream(pFile);  // switch file on again

    LOG(LFATAL) << LINE << "This should be logged"; counter++;

    // now let's check if everything was written properly
    
    LoggingPolicy::getPolicyInstance()->setFileStream(NULL);  // switch off logging to file
    fclose(pFile);
    

    ifstream file("iris2.log"); //open file
    const std::streamsize MAX_LEN = 150;
    char line[MAX_LEN];
    std::string ending = "should be logged";
    size_t endlen = ending.size();

    unsigned lineCount = 0;
    while (!file.eof())
    {
        file.getline(line, MAX_LEN);
        std::string str(line);
        size_t strl = str.size();
        if (strl > 0) // ignore empty lines
        {
            BOOST_CHECK_EQUAL(str.substr(strl-endlen), ending);
            lineCount++;
            // check that line number matches the one mentioned in the line in the file
            std::stringstream sstr;
            sstr << "line " << lineCount;
            BOOST_CHECK_NE(str.find(sstr.str()), std::string::npos);
        }
    }
    // make sure the correct number of lines are in the file
    BOOST_CHECK_EQUAL(lineCount, counter);

    file.close();

}

struct threads
{
    threads(int number) : d_number(number) {}

    void operator()()
    {
        using namespace iris;

        for (int i = 1; i < 123; i++)
        {
            LOG(LWARNING) << "Thread " << d_number << " " << i << " testing logging";
            LOG(LDEBUG) << "no log";
            if (i % 10 == 0) // sleep a while every 10 iterations
                boost::this_thread::sleep(boost::posix_time::milliseconds(1));
        }     
    }
private:
    int d_number;
};

BOOST_AUTO_TEST_CASE(MultipleThreads)
{
    using namespace std;
    using namespace iris;

    FILE* pFile = fopen("iris2.log", "a");
    BOOST_REQUIRE(pFile!=NULL);

    LoggingPolicy::getPolicyInstance()->setFileStream(pFile);
    LoggingPolicy::getPolicyInstance()->ReportingLevel() = LWARNING;

    // do this a couple of time to ensure different schedulings
    for (int i = 0; i < 30; i++)
    {
        boost::thread t1(threads(1));
        boost::thread t2(threads(2));
        boost::thread t3(threads(3));
        boost::thread t4(threads(4));

        // wait a bit...
        boost::this_thread::sleep(boost::posix_time::milliseconds(4));   

        // wait for threads to finish
        t1.join();
        t2.join();
        t3.join();
        t4.join();
    }

    fclose(pFile);
}



BOOST_AUTO_TEST_SUITE_END()
