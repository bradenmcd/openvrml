//
// OpenVRML
//
// Copyright (C) 2001  Braden McDaniel
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

# include "script.h"

using namespace OpenVRML;

/**
 * @class OpenVRML::Script
 *
 * @brief Abstract class implemented by scripting language bindings.
 *
 * The runtime instantiates subclasses of Script for each VRML97 Script node;
 * and calls its methods appropriately to execute script code.
 */

/**
 * @var ScriptNode & Script::scriptNode
 *
 * @brief a reference to the ScriptNode that uses this Script object.
 */

/**
 * @brief Constructor.
 *
 * @param scriptNode    a reference to the ScriptNode that uses this Script
 *      object.
 */
Script::Script(ScriptNode & scriptNode): scriptNode(scriptNode) {}

/**
 * @brief Destructor.
 */
Script::~Script() {}

/**
 * @fn void Script::initialize(double timestamp)
 *
 * @brief Initialize the Script node.
 */

/**
 * @fn void Script::processEvent(const std::string & id,
 *                               const FieldValue & value, double timestamp)
 *
 * @brief Process an event.
 */

/**
 * @fn void Script::eventsProcessed(double timestamp)
 *
 * @brief Execute script code after processing events.
 */

/**
 * @fn void Script::shutdown(double timestamp)
 *
 * @brief Shut down the Script node.
 */
