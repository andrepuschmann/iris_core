/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/*
 * file CommandPrison_Test.cpp
 * Main test file for CommandPrison class
 *
 *  Created on: 18-May-2011
 *  Created by: suttonp
 *  $Revision: 1345 $
 *  $LastChangedDate: 2011-09-19 17:36:08 +0100 (Mon, 19 Sep 2011) $
 *  $LastChangedBy: suttonp $
 */


#define BOOST_TEST_MODULE CommandPrison_Test

#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "irisapi/Logging.h"
#include "irisapi/CommandPrison.h"

using namespace std;
using namespace iris;

BOOST_AUTO_TEST_SUITE (CommandPrison_Test)

//! getName function is used for logging
std::string getName()
{
	return "CommandPrison_Test";
}

//! Enter the prison and wait for a command
static void trap(std::string command, boost::shared_ptr<CommandPrison> prison, int id)
{
	LOG(LDEBUG) << "Thread " << id << " entering prison.";
    Command c = prison->trap(command);
    LOG(LDEBUG) << "Thread " << id <<
    		" released by command " << c.commandName;
}

//! Release threads waiting for the given command
static void release(Command command, boost::shared_ptr<CommandPrison> prison)
{
	prison->release(command);
}

BOOST_AUTO_TEST_CASE(CommandPrison_Basic_Test)
{
	boost::shared_ptr<CommandPrison> prison(new CommandPrison);

	BOOST_CHECK(prison->size() == 0);

	// Create a number of threads to wait in prison
	boost::thread t0( boost::bind( &trap, "go1", prison ,0) );
	boost::thread t1( boost::bind( &trap, "go1", prison ,1) );
	boost::thread t2( boost::bind( &trap, "go1", prison ,2) );
	boost::thread t3( boost::bind( &trap, "go1", prison ,3) );
	boost::thread t4( boost::bind( &trap, "go1", prison ,4) );
	boost::thread t5( boost::bind( &trap, "go2", prison ,5) );
	boost::thread t6( boost::bind( &trap, "go2", prison ,6) );
	boost::thread t7( boost::bind( &trap, "go2", prison ,7) );
	boost::thread t8( boost::bind( &trap, "go2", prison ,8) );
	boost::thread t9( boost::bind( &trap, "go2", prison ,9) );

	//Wait for threads to start
	boost::this_thread::sleep(boost::posix_time::seconds(1));

	BOOST_CHECK(prison->size() == 10);

	//Release 5 of our threads
    Command c;
    c.commandName = "go1";
    prison->release(c);

    t0.join();
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    BOOST_CHECK(prison->size() == 5);

    //Release the remaining 5 of our threads
	c.commandName = "go2";
	prison->release(c);

    t5.join();
    t6.join();
    t7.join();
    t8.join();
    t9.join();

    BOOST_CHECK(prison->size() == 0);
}

BOOST_AUTO_TEST_CASE(CommandPrison_MultiRelease_Test)
{
	boost::shared_ptr<CommandPrison> prison(new CommandPrison);

	BOOST_CHECK(prison->size() == 0);

	// Create a number of threads to wait in prison
	boost::thread t0( boost::bind( &trap, "go1", prison ,0) );
	boost::thread t1( boost::bind( &trap, "go1", prison ,1) );
	boost::thread t2( boost::bind( &trap, "go2", prison ,2) );
	boost::thread t3( boost::bind( &trap, "go2", prison ,3) );
	boost::thread t4( boost::bind( &trap, "go3", prison ,4) );
	boost::thread t5( boost::bind( &trap, "go3", prison ,5) );
	boost::thread t6( boost::bind( &trap, "go4", prison ,6) );
	boost::thread t7( boost::bind( &trap, "go4", prison ,7) );
	boost::thread t8( boost::bind( &trap, "go5", prison ,8) );
	boost::thread t9( boost::bind( &trap, "go5", prison ,9) );

	//Wait for threads to start
	boost::this_thread::sleep(boost::posix_time::seconds(1));

	BOOST_CHECK(prison->size() == 10);

	//Release our threads
	Command c;
	c.commandName = "go2";
	boost::thread r0( boost::bind( &release, c, prison ) );
	c.commandName = "go4";
	boost::thread r1( boost::bind( &release, c, prison ) );
	c.commandName = "go1";
	boost::thread r2( boost::bind( &release, c, prison ) );
	c.commandName = "go5";
	boost::thread r3( boost::bind( &release, c, prison ) );
	c.commandName = "go3";
	boost::thread r4( boost::bind( &release, c, prison ) );


    t0.join();
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
    t9.join();

    BOOST_CHECK(prison->size() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
