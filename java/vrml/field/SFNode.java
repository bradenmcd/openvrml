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

package vrml.field;

import vrml.Field;
import vrml.BaseNode;

/**
 * Represents a VRML SFNode field in Java.
 */
public class SFNode extends Field {

    /**
     * Construct a new SFNode field in OpenVRML using the given node.
     *
     * @param node VRML Node to create SFNode with.
     */
    private native void CreateObject(BaseNode node);

    /**
     * Construct a default SFNode field.
     */
    public SFNode() {
        CreateObject(null);
    }

    /**
     * Construct an SFNode field.
     *
     * @param node VRML Node to create SFNode with
     */
    public SFNode(BaseNode node) {
        CreateObject(node);
    }

    /**
     * Get the VRML node stored in this SFNode.
     *
     * @return VRML node object
     */
    public native BaseNode getValue();

    /**
     * Set the VRML node stored in this SFNode.
     *
     * @param node VRML node object
     */
    public native void setValue(BaseNode node);

    /**
     * Set the VRML node stored in this SFNode.
     *
     * @param node Existing ConstSFNode
     */
    public native void setValue(ConstSFNode node);

    /**
     * Set the VRML node stored in this SFNode.
     *
     * @param node Existing SFNode
     */
    public native void setValue(SFNode node);

    public native String toString();
}
