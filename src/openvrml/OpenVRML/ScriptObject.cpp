//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
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

# include "ScriptObject.h"

/**
 * @class ScriptObject
 *
 * @brief An abstract base class for Script node language bindings.
 */

/**
 * @brief Constructor.
 *
 * @param scriptNode a reference to the "owning" Script node for this script.
 */
ScriptObject::ScriptObject(VrmlNodeScript & scriptNode): scriptNode(scriptNode)
{}

/**
 * @brief Destructor.
 */
ScriptObject::~ScriptObject() {}

/**
 * @var ScriptObject::scriptNode
 *
 * @brief A reference to the Script node associated with this script.
 */
