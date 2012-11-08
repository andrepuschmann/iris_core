/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file Launcher.cpp
 * The launcher application which runs the IRISV2 system
 *
 *  Created on: 12-Jan-2009
 *  Created by: suttonp, lotzej
 *  $Revision: 1316 $
 *  $LastChangedDate: 2011-09-13 12:41:16 +0100 (Tue, 13 Sep 2011) $
 *  $LastChangedBy: suttonp $
 */
#include "IrisStateMachine.h"

#include <iostream>
#include <fstream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>

//! \todo We need to write a basic config.h for Windows, with package information
#ifdef HAVE_CONFIG_H
#  include "config.h"
#else
#  define VERSION "0.1"
#endif

using namespace std;
namespace po = boost::program_options;

class Launcher
{
public:
    //! constructor
    Launcher();

    //! destructor
    ~Launcher();

	//! pause the console
	static void pauseConsole();
    //! parses the command line options
    void parseOptions(int argc, char* argv[]) throw (po::error, LauncherException);
    //! prints status info (repository dir, radio config, etc.)
    void printStatus();
    //! prints menu for radio control
    void printMenu();
    //! prints welcome banner for startup
    void printBanner();
    //! loop for controlling the radio
    void menuLoop() throw (LauncherException);

private:
    //! XML radio configuration
    string d_radioConfig;
	//! path to the Stack component repository
    string d_stackRepoPath;
    //! path to the PN component repository
    string d_pnRepoPath;
    //! path to the SDF component repository
    string d_sdfRepoPath;
	//! path to the controller repository
    string d_contRepoPath;
    //! log level
    string d_logLevel;
    //! whether to load the radio automatically at startup
    bool d_autoLoad;
    //! whether to start the radio automatically at startup
    bool d_autoStart;
    //! configuration file for passing options to launcher
    string d_configFile;
    //! the state machine managing the various calls into the iris interface
    IrisStateMachine d_stateMachine;
    //! whether the launcher is running - if false, functions will return immediately
    bool d_isRunning;
};


//! binary entry point
int main(int argc, char* argv[])
{
    try
    {
        Launcher l;
        l.parseOptions(argc, argv);
        l.menuLoop();
    }
    catch (po::error& err)
    {
		cerr << "Launcher: Program options error: " << err.what() << endl;
		Launcher::pauseConsole();
        return EXIT_FAILURE;
    }
    catch (LauncherException ex)
    {
		cerr << "Launcher: Error: " << ex.what() << endl;
		Launcher::pauseConsole();
        return EXIT_FAILURE;
    }
    catch (...)
    {
		cerr << "Launcher: Unexpected error - exiting" << endl;
		Launcher::pauseConsole();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


Launcher::Launcher()
    :d_radioConfig(""), d_pnRepoPath(""), d_sdfRepoPath(""), d_contRepoPath(""), 
	d_logLevel("debug"), d_autoLoad(true), d_autoStart(true), d_stateMachine(),
	d_isRunning(true)
{
    printBanner();
}

Launcher::~Launcher()
{
}

void Launcher::pauseConsole()
{
   cout << "Press any key to continue..." << endl;
   cin.get();
}

void Launcher::parseOptions(int argc, char* argv[]) throw (po::error, LauncherException)
{
    if (!d_isRunning)
        return;

    po::options_description general("General options");
    general.add_options()
        ("help,h", "Produce help message")
        ("version,V", "Print version information and exit")
        ("config-file,f", po::value<string>(&d_configFile), "Load parameters from config file")
        ;

    po::options_description desc("Configuration options");
    desc.add_options()
		("stackrepository,t", po::value<string>(&d_stackRepoPath), "Repository of IRISv2 Stack components")
        ("pnrepository,p", po::value<string>(&d_pnRepoPath), "Repository of IRISv2 PN components")
        ("sdfrepository,s", po::value<string>(&d_sdfRepoPath), "Repository of IRISv2 SDF components")
		("controllerrepository,c", po::value<string>(&d_contRepoPath), "Repository of IRISv2 controllers")
		("loglevel,l", po::value<string>(&d_logLevel), "Log level (options are debug, info, warning, error & fatal)")
        ("no-load",  "Do not automatically load radio (implies --no-start)")
        ("no-start", "Do not automatically start radio")
    ;

    // Hidden options, will not be shown in the help string (positional options)
    po::options_description hidden("Hidden options");
    hidden.add_options()
    ("xmlconfig,x", po::value< string >(&d_radioConfig), "XML configuration file for the radio")
    ;

    // positional options - the parameters without --option will be treated as xml files
    po::positional_options_description p;
    p.add("xmlconfig", -1);

    // these options are allowed on the command line
    po::options_description cmdline_options;
    cmdline_options.add(desc).add(hidden).add(general);

    // parse the command line and store result in vm
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
    //po::store(po::config_file_parser("config.ini", desc), vm);
    po::notify(vm);

    // help option was given - print usage and set running to false
    if (vm.count("help")) {
        cout << "IRIS2 software radio" << endl << endl;
        cout << "Usage: " << argv[0] << " [OPTIONS] xmlconfigfile" << endl << endl;
        cout << desc << endl << general << endl;
        d_isRunning = false;
        return;
    }

    // print version number and exit
    if (vm.count("version")) {
        cout << "IRIS2, version " << VERSION << endl;
        d_isRunning = false;
        return;
    }

    // config file was given
    if (vm.count("config-file")) {
        cout << "parsing config file " << d_configFile << "..." << endl;
        ifstream hConfFile(d_configFile.c_str(), ios::in);
        if (hConfFile.fail())
            throw LauncherException("Could not open " + d_configFile + " for reading.");
        po::store(po::parse_config_file(hConfFile, desc), vm);
        po::notify(vm);
    }

    // if radio config was not set, exit with error
    if (!vm.count("xmlconfig")) {
        throw LauncherException("Radio configuration was not set.");
    }

    // if a repository was not given, exit with error
    if (!vm.count("pnrepository") && !vm.count("sdfrepository") && !vm.count("stackrepository")) {
        throw LauncherException("No component repositories were set.");
    }

    if (vm.count("no-load"))
    {
        d_autoLoad = false;
        d_autoStart = false;
    }

    if (vm.count("no-start"))
        d_autoStart = false;
}

void Launcher::menuLoop() throw (LauncherException)
{
    if (!d_isRunning)
        return;

    d_stateMachine.setRadioConfig(d_radioConfig);
	d_stateMachine.setStackRadioRepository(d_stackRepoPath);
    d_stateMachine.setPnRadioRepository(d_pnRepoPath);
    d_stateMachine.setSdfRadioRepository(d_sdfRepoPath);
	d_stateMachine.setContRadioRepository(d_contRepoPath);
	d_stateMachine.setLogLevel(d_logLevel);
    d_stateMachine.initiate();

    if (d_autoLoad)
    {
        d_stateMachine.process_event(EvLoadUnload() );
        if (d_autoStart)
            d_stateMachine.process_event(EvStartStop() );
    }

    char key = 0;

    while (toupper(key) != 'Q')
    {
		printStatus();
		printMenu();

        cin >> key;

        switch (toupper(key))
        {
            case 'L':  // load
            case 'U':  // unload
                d_stateMachine.process_event( EvLoadUnload() );
                break;
            case 'S':  // start/stop
                d_stateMachine.process_event( EvStartStop() );
                break;
            case 'Q':   // do nothing, will exit while
                break;
            case 'R':   // reconfigure the radio
                d_stateMachine.reconfigureRadio();
                break;
            default:
                cout << "Unknown command: '" << key << "'";
                break;
        }

    }

    d_stateMachine.terminate();
}

void Launcher::printStatus()
{
    cout << endl;
	cout << "Stack Repository  :  " << d_stackRepoPath << endl;
    cout << "PN Repository  :  " << d_pnRepoPath << endl;
    cout << "SDF Repository  : " << d_sdfRepoPath << endl;
	cout << "Controller Repository  : " << d_contRepoPath << endl;
	cout << "Log level : " << d_logLevel << endl;
    cout << "Radio Config: " << d_radioConfig << endl;
}

void Launcher::printMenu()
{
    string line1 = "";
    string state = "";

    // iterate over leaf state to find menu output
    for (IrisStateMachine::state_iterator pLeafState = d_stateMachine.state_begin();
         pLeafState != d_stateMachine.state_end(); ++pLeafState )
    {
        if (pLeafState->dynamic_type() == Running::static_type())
        {
            line1 = "\tU  Unload Radio\t\tS  Stop Radio";
            state = "running";
        }
        else if (pLeafState->dynamic_type() == Stopped::static_type())
        {
            line1 = "\tU  Unload Radio\t\tS  Start Radio";
            state = "stopped";
        }
        else if (pLeafState->dynamic_type() == Unloaded::static_type())
        {
            line1 = "\tL  Load Radio";
            state = "unloaded";
        }
        else
            line1 = "Error";
    }

    cout << endl
         << "\t    IRIS version 2 Software Radio" << endl
         << "\t    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl << endl
         << line1 << endl
         << "\tR  Reconfigure\t\tQ  Quit" << endl << endl
         << "(Radio " << state << "), Selection: ";
}

void Launcher::printBanner()
{
    cout << endl
         << "\t    IRIS version 2 Software Radio" << endl
         << "\t    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl << endl;
}
