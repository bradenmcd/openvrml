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

import vrml.ConstMField;
import vrml.BaseNode;

/**
 * Represents a read-only VRML MFNode field in Java.
 */
public class ConstMFNode extends ConstMField {
    public native int getSize();

    /**
     * Construct a new MFNode field in OpenVRML using the given params.
     *
     * @param size Number of elements passed in.
     * @param nodes List of BaseNodes.
     */
    private native void CreateObject(int size, BaseNode nodes[]);

    /**
     * Construct a read-only MFNode field.
     *
     * @param size Number of elements passed in.
     * @param nodes Array of BaseNodes.
     */
    public ConstMFNode(int size, BaseNode nodes[]) {
        CreateObject(size, nodes);
    }

    /**
     * Construct a read-only MFNode field.
     *
     * @param nodes Array of BaseNodes.
     */
    public ConstMFNode(BaseNode nodes[]) {
        CreateObject(nodes.length, nodes);
    }

    /**
     * Retrieves the value of an MFNode field.
     *
     * @param nodes Array of BaseNodes to be returned.
     */
    public native void getValue(BaseNode nodes[]);

    /**
     * Retrieves a specific BaseNode from an MFNode.
     *
     * @param index Position of desired BaseNode.
     * @return Value of BaseNode at index.
     */
    public native BaseNode get1Value(int index);

    public native String toString();
}
