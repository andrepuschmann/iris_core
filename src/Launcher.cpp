/**
 * \file Launcher.cpp
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
 * The Launcher command line application which can be used to run
 * Iris. Uses the C interface defined in Iris.h.
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
    string radioConfig_;
    //! path to the Stack component repository
    string stackRepoPath_;
    //! path to the PN component repository
    string pnRepoPath_;
    //! path to the SDF component repository
    string sdfRepoPath_;
    //! path to the controller repository
    string contRepoPath_;
    //! log level
    string logLevel_;
    //! whether to load the radio automatically at startup
    bool autoLoad_;
    //! whether to start the radio automatically at startup
    bool autoStart_;
    //! configuration file for passing options to launcher
    string configFile_;
    //! the state machine managing the various calls into the iris interface
    IrisStateMachine stateMachine_;
    //! whether the launcher is running - if false, functions will return immediately
    bool isRunning_;
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
    catch (LauncherException& ex)
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
    :radioConfig_(""), pnRepoPath_(""), sdfRepoPath_(""), contRepoPath_(""), 
    logLevel_("debug"), autoLoad_(true), autoStart_(true), stateMachine_(),
    isRunning_(true)
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
    if (!isRunning_)
        return;

    po::options_description general("General options");
    general.add_options()
        ("help,h", "Produce help message")
        ("version,V", "Print version information and exit")
        ("config-file,f", po::value<string>(&configFile_), "Load parameters from config file")
        ;

    po::options_description desc("Configuration options");
    desc.add_options()
        ("stackrepository,t", po::value<string>(&stackRepoPath_), "Repository of IRISv2 Stack components")
        ("pnrepository,p", po::value<string>(&pnRepoPath_), "Repository of IRISv2 PN components")
        ("sdfrepository,s", po::value<string>(&sdfRepoPath_), "Repository of IRISv2 SDF components")
        ("controllerrepository,c", po::value<string>(&contRepoPath_), "Repository of IRISv2 controllers")
        ("loglevel,l", po::value<string>(&logLevel_), "Log level (options are debug, info, warning, error & fatal)")
        ("no-load",  "Do not automatically load radio (implies --no-start)")
        ("no-start", "Do not automatically start radio")
    ;

    // Hidden options, will not be shown in the help string (positional options)
    po::options_description hidden("Hidden options");
    hidden.add_options()
    ("xmlconfig,x", po::value< string >(&radioConfig_), "XML configuration file for the radio")
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
        cout << "Iris software radio" << endl << endl;
        cout << "Usage: " << argv[0] << " [OPTIONS] xmlconfigfile" << endl << endl;
        cout << desc << endl << general << endl;
        isRunning_ = false;
        return;
    }

    // print version number and exit
    if (vm.count("version")) {
        cout << "Iris, version " << VERSION << endl;
        isRunning_ = false;
        return;
    }

    // config file was given
    if (vm.count("config-file")) {
        cout << "parsing config file " << configFile_ << "..." << endl;
        ifstream hConfFile(configFile_.c_str(), ios::in);
        if (hConfFile.fail())
            throw LauncherException("Could not open " + configFile_ + " for reading.");
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
        autoLoad_ = false;
        autoStart_ = false;
    }

    if (vm.count("no-start"))
        autoStart_ = false;
}

void Launcher::menuLoop() throw (LauncherException)
{
    if (!isRunning_)
        return;

    stateMachine_.setRadioConfig(radioConfig_);
    stateMachine_.setStackRadioRepository(stackRepoPath_);
    stateMachine_.setPnRadioRepository(pnRepoPath_);
    stateMachine_.setSdfRadioRepository(sdfRepoPath_);
    stateMachine_.setContRadioRepository(contRepoPath_);
    stateMachine_.setLogLevel(logLevel_);
    stateMachine_.initiate();

    if (autoLoad_)
    {
        stateMachine_.process_event(EvLoadUnload() );
        if (autoStart_)
            stateMachine_.process_event(EvStartStop() );
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
                stateMachine_.process_event( EvLoadUnload() );
                break;
            case 'S':  // start/stop
                stateMachine_.process_event( EvStartStop() );
                break;
            case 'Q':   // do nothing, will exit while
                break;
            case 'R':   // reconfigure the radio
                stateMachine_.reconfigureRadio();
                break;
            default:
                cout << "Unknown command: '" << key << "'";
                break;
        }

    }

    stateMachine_.terminate();
}

void Launcher::printStatus()
{
    cout << endl;
    cout << "Stack Repository  :  " << stackRepoPath_ << endl;
    cout << "PN Repository  :  " << pnRepoPath_ << endl;
    cout << "SDF Repository  : " << sdfRepoPath_ << endl;
    cout << "Controller Repository  : " << contRepoPath_ << endl;
    cout << "Log level : " << logLevel_ << endl;
    cout << "Radio Config: " << radioConfig_ << endl;
}

void Launcher::printMenu()
{
    string line1 = "";
    string state = "";

    // iterate over leaf state to find menu output
    for (IrisStateMachine::state_iterator pLeafState = stateMachine_.state_begin();
         pLeafState != stateMachine_.state_end(); ++pLeafState )
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
         << "\t    Iris Software Radio" << endl
         << "\t    ~~~~~~~~~~~~~~~~~~~" << endl << endl
         << line1 << endl
         << "\tR  Reconfigure\t\tQ  Quit" << endl << endl
         << "(Radio " << state << "), Selection: ";
}

void Launcher::printBanner()
{
    cout << endl
         << "\t    Iris Software Radio" << endl
         << "\t    ~~~~~~~~~~~~~~~~~~~" << endl << endl;
}
