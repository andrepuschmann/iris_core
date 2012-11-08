/*
 * This file is part of Iris 2.
 * 
 * Copyright (C) 2009 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 * All rights reserved.
 * 
 */

/**
 * \file ComponentPorts.h
 * Contains classes for the port interface of a component
 *
 *  Created on: 7-1-2009
 *  Created by: suttonp
 *  $Revision: 712 $
 *  $LastChangedDate: 2009-10-20 19:28:06 +0100 (Tue, 20 Oct 2009) $
 *  $LastChangedBy: lotzej $
 */

#ifndef COMPONENTPORTS_H_
#define COMPONENTPORTS_H_

#include <string>
#include <vector>


namespace iris
{

/** Represents a port of a component child class. Will be created by
 *  the registerPort function in the ComponentPorts class.
 */
struct Port
{
    //! name of port
    std::string portName;

    //! types supported for this port
    std::vector<int> supportedTypes;

    //! default constructor
    Port() :
        portName("")
    {
    }

    /** Constructs a new Port object.
     *
     * @param name The name assigned to this port
     * @param types The types supported for this port
     */
        Port(std::string name, const std::vector<int>& types) :
        portName(name), supportedTypes(types)
    {
    }

};

/*!
 * \brief The ports interface for all components
 *
 * The ComponentPorts class allows components to register their input and output ports along with supported
 * data types.
 */

class ComponentPorts
{
private:
    //! The input ports registered for this component
    std::vector<Port> inputPorts;

    //! The output ports registered for this component
    std::vector<Port> outputPorts;

protected:
    /** Registers an input port of a child class.
     *
     * @param name Name of the port
     * @param types Types supported by the port
     */
    void registerInputPort(std::string name, const std::vector<int>& types)
    {
        inputPorts.push_back(Port(name, types));
    };

    /** Registers an output port of a child class.
     *
     * @param name Name of the port
     * @param types Types supported by the port
     */
    void registerOutputPort(std::string name, std::vector<int> types)
    {
        outputPorts.push_back(Port(name, types));
    };


public:
    //! ctor
    ComponentPorts()
    {};

    /** Get the input ports of this component.
     *
     */
    std::vector<Port> getInputPorts() const
    {
        return inputPorts;
    };

    /** Get the output ports of this component.
     *
     */
    std::vector<Port> getOutputPorts() const
    {
        return outputPorts;
    };

};


} // end iris namespace

#endif /* COMPONENTPORTS_H_ */
