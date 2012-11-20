/**
 * \file IrisStateMachine.h
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
 * State Machine for Iris Launcher using the boost statechart library.
 */

// Machine graph:
//
//  ---------------------------------------
// |                     |
// |    o     Active         |
// |    |                |
// |    v                |
// |  -------------------------------    |
// | |     Unloaded        |   |
// |  -------------------------------    |
// |  |         ^        |
// |  |  EvLoadUnload   | EvLoadUnload   |
// |  v         |        |
// |  -----------------------------------  |
// | |                   | |
// | |    o     Loaded       | |
// | |    |              | |
// | |    v              | |
// | |   -------------------------     | |
// | |  |  Stopped        |    | |
// | |   -------------------------     | |
// | |  |        ^       | |
// | |  | EvStartStop  | EvStartStop | |
// | |  v        |       | |
// | |   -------------------------     | |
// | |  |  Running        |    | |
// | |   -------------------------     | |
// |  -----------------------------------  |
//  ---------------------------------------
//


#ifndef IRIS_STATE_MACHINE_H
#define IRIS_STATE_MACHINE_H 1

#include <boost/statechart/event.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/custom_reaction.hpp>

//! Exception which can be thrown by this class
class LauncherException : public std::exception
{
private:
  std::string message_;
public:
  LauncherException(const std::string &message) throw()
    :exception(), message_(message)
  {};
  virtual const char* what() const throw()
  {
    return message_.c_str();
  };
  virtual ~LauncherException() throw()
  {};
};

//! Event to start/stop the radio
struct EvStartStop : boost::statechart::event< EvStartStop > {};
//! Event to load/unload the radio
struct EvLoadUnload : boost::statechart::event< EvLoadUnload > {};

// forward declarations of the states
struct Active;  //!< active state, Iris engine is initialized
struct Unloaded;  //!< no radio loaded, initial state within Active
struct Loaded;  //!< radio loaded, within Active
struct Stopped;   //!< radio is stopped, initial state within Loaded
struct Running;   //!< radio is running, state within Loaded


//! The state machine itself, start state is Active
struct IrisStateMachine : boost::statechart::state_machine< IrisStateMachine, Active >
{
  //! set XML radio configuration
  void setRadioConfig(std::string radioConfig) { radioConfig_ = radioConfig; }
  //! return XML radio configuration
  std::string getRadioConfig() const { return radioConfig_; }
  //! set radio Stack component repository
  void setStackRadioRepository(std::string radioRepository) { stackRadioRepository_ = radioRepository; }
  //! return radio Stack component repository
  std::string getStackRadioRepository() const { return stackRadioRepository_; }
  //! set radio PN component repository
  void setPnRadioRepository(std::string radioRepository) { pnRadioRepository_ = radioRepository; }
  //! return radio PN component repository
  std::string getPnRadioRepository() const { return pnRadioRepository_; }
  //! set radio SDF component repository
  void setSdfRadioRepository(std::string radioRepository) { sdfRadioRepository_ = radioRepository; }
  //! return radio SDF component repository
  std::string getSdfRadioRepository() const { return sdfRadioRepository_; }
  //! set radio Controller repository
  void setContRadioRepository(std::string radioRepository) { contRadioRepository_ = radioRepository; }
  //! return radio Controller repository
  std::string getContRadioRepository() const { return contRadioRepository_; }
  //! set log level
  void setLogLevel(std::string level) { logLevel_ = level; }
  //! return log level
  std::string getLogLevel() const { return logLevel_; }
  //! Reconfigure the radio
  void reconfigureRadio();
private:
  //! stores the XML radio configuration
  std::string radioConfig_;
  //! stores the radio Stack component repository
  std::string stackRadioRepository_;
  //! stores the radio PN component repository
  std::string pnRadioRepository_;
  //! stores the radio SDF component repository
  std::string sdfRadioRepository_;
  //! stores the radio Controller repository
  std::string contRadioRepository_;
  //! stores the log level
  std::string logLevel_;
};

//! Active is the parent of all other states, destruction means termination
//! When entering this state, the Iris System is initialised and the FSM goes to
//! the internal state Unloaded.
//
// Note: boost::statechart::simple_state takes 2 or 3 template parameters:
// 1. The struct itself (for template magic handle by statechart lib)
// 2. The parent (either state machine or other state in hierarchical FSMs)
// 3. (optional) The initial internal state in hierarchical FSMs
struct Active : boost::statechart::state< Active, IrisStateMachine, Unloaded >
{
  Active(my_context ctx);  //!< Constructor. Initialises the Iris System
};

//! In the Loaded state, a transition is made to Unloaded on EvLoadUnload
//! When entering this state, a radio is loaded into memory and made ready
//! for execution.
struct Loaded : boost::statechart::state< Loaded, Active, Stopped >
{
  //! On event EvLoadUnload, change to  Unloaded
  typedef boost::statechart::transition< EvLoadUnload, Unloaded > reactions;

  Loaded(my_context ctx) throw (LauncherException);  //!< Constructor. Loads the radio.

  //! unloads the radio
  void exit() throw (LauncherException);
};

//! In the Unloaded state, a transition is made to Loaded on EvLoadUnload
//! Nothing is done when entering or exiting this state
struct Unloaded :  boost::statechart::simple_state< Unloaded, Active >
{
  //! On event EvLoadUnload, transit to Loaded
  typedef boost::statechart::transition< EvLoadUnload, Loaded > reactions;

};

//! In the Stopped state, a transition is made to Running on EvStartStop
//! When entering/exiting this state, no actions are performed.
struct Stopped : boost::statechart::simple_state< Stopped, Loaded >
{
  //! On event EvStartStop, transit to Running
  typedef boost::statechart::transition< EvStartStop, Running > reactions;
};

//! In the Running state, a transition is made to Stopped on EvStartStop
//! When entering this state, the radio execution is started. On exit, the
//! radio execution is stopped.
struct Running : boost::statechart::simple_state< Running, Loaded >
{
  //! On event EvStartStop, transit to Stopped
  typedef boost::statechart::transition< EvStartStop, Stopped > reactions;

  //! starts the radio
  Running() throw (LauncherException);

  //! stops the radio
  void exit() throw (LauncherException);
};

#endif

