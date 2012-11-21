/**
 * \file ReconfigurationDescriptions.h
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
 * Structs which describe reconfigurations to be carried out within
 * a running radio.
 */

#ifndef IRISAPI_RECONFIGURATIONDESCRIPTIONS_H_
#define IRISAPI_RECONFIGURATIONDESCRIPTIONS_H_

#include <string>
#include <vector>

#include "irisapi/LinkDescription.h"

namespace iris
{

//! A parametric reconfiguration 
struct ParametricReconfig
{
  std::string engineName;
  std::string componentName;
  std::string parameterName;
  std::string parameterValue;
};

//! An altered link
struct AlteredLink
{
  LinkDescription oldLink;
  LinkDescription newLink;
};

//! A structural reconfiguration
struct StructuralReconfig
{
  std::string engineName;
  std::vector< std::string > removedComponents;
  std::vector< LinkDescription > removedLinks;
  //std::vector< ComponentDescription > newComponents;
  std::vector< LinkDescription > newLinks;
  std::vector< AlteredLink > alteredLinks;
};

/** A set of radio reconfigurations
*
*   A ReconfigSet contains a number of reconfigurations which must be carried out 
*   atomically by an engine with respect to the data flow.
*/
struct ReconfigSet
{
  //! A set of parametric reconfigurations 
  std::vector< ParametricReconfig > paramReconfigs_;

  //! The set of structural reconfigurations
  std::vector< StructuralReconfig > structReconfigs_;
};

} /* namespace iris */

#endif /* IRISAPI_RECONFIGURATIONDESCRIPTIONS_H_ */
