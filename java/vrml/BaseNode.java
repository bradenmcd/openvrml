//
// OpenVRML
//
// Copyright 2001  Henri Manson
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

package vrml;

/**
 * Base class for all VRML nodes.
 */
public abstract class BaseNode {
    /**
     * Stores a pointer to a VrmlNode in OpenVRML.
     */
    private int NodePtr;

    /**
     * Is this a script node?
     */
    private boolean isScript = false;

    /**
     * Returns the type of the node.  If the node is a prototype
     * it returns the name of the prototype.
     *
     * @return Type of the node.
     */
    public native String getType();

    /**
     * Get the Browser that this node is contained in.
     *
     * @return Browser node is contained in.
     */
    public native Browser getBrowser();
}
