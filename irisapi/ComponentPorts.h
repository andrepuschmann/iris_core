/**
 * \file ComponentPorts.h
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
 * Contains classes for the port interface of an Iris component.
 */

#ifndef IRISAPI_COMPONENTPORTS_H_
#define IRISAPI_COMPONENTPORTS_H_

#include <string>
#include <vector>


namespace iris
{

/** Represents a port of a component child class. Will be created by
 *  the registerPort function in the ComponentPorts class.
 */
struct Port
{
  std::string portName;             ///< The name of this port.
  std::vector<int> supportedTypes;  ///< The data types supported by this port.

  Port()
    : portName("")
  {}

  /** Constructs a new Port object.
   *
   * \param name The name assigned to this port
   * \param types The types supported for this port
   */
  Port(std::string name, const std::vector<int>& types)
    : portName(name), supportedTypes(types)
  {}

};

/** The ports interface for all components
 *
 * The ComponentPorts class allows components to register their input and output
 * ports along with supported data types.
 */
class ComponentPorts
{
public:
  ComponentPorts()
  {};

  std::vector<Port> getInputPorts() const
  {return inputPorts;};

  std::vector<Port> getOutputPorts() const
  {return outputPorts;};

protected:
  /** Registers an input port of a child class.
   *
   * \param name Name of the port
   * \param types Types supported by the port
   */
  void registerInputPort(std::string name, const std::vector<int>& types)
  {
    inputPorts.push_back(Port(name, types));
  };

  /** Registers an output port of a child class.
   *
   * \param name Name of the port
   * \param types Types supported by the port
   */
  void registerOutputPort(std::string name, std::vector<int> types)
  {
    outputPorts.push_back(Port(name, types));
  };

private:
  std::vector<Port> inputPorts;   ///< Input ports registered for this component.
  std::vector<Port> outputPorts;  ///< Output ports registered for this component.

};


} // namespace iris

#endif // IRISAPI_COMPONENTPORTS_H_
