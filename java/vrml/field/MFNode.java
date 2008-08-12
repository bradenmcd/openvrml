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

import vrml.MField;
import vrml.BaseNode;

/**
 * Represents a VRML MFNode field in Java.
 */
public class MFNode extends MField {

    public native int getSize();

    public native void clear();

    public native void delete(int index);

    /**
     * Construct a new MFNode field in OpenVRML using the given params.
     *
     * @param size Number of elements passed in.
     * @param nodes List of BaseNodes.
     */
    private native void CreateObject(int size, BaseNode nodes[]);

    /**
     * Default constructor.
     */
    public MFNode() {
        CreateObject(0, null);
    }

    /**
     * Construct an MFNode field.
     *
     * @param size Number of elements passed in.
     * @param nodes Array of BaseNodes.
     */
    public MFNode(int size, BaseNode nodes[]) {
        CreateObject(size, nodes);
    }

    /**
     * Construct an MFNode field.
     *
     * @param nodes Array of BaseNodes.
     */
    public MFNode(BaseNode nodes[]) {
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

    /**
     * Set the value of the field.
     *
     * @param node New value for field.
     */
    public void setValue(BaseNode node[]) {
        setValue(node.length, node);
    }

    /**
     * Set the value of the field.
     *
     * @param size Size of new value for field.
     * @param node New value for field.
     */
    public native void setValue(int size, BaseNode node[]);

    /**
     * Set the value of the field.
     *
     * @param node New value for field.
     */
    public native void setValue(MFNode node);

    /**
     * Set the value of the field.
     *
     * @param node New value for field.
     */
    public native void setValue(ConstMFNode node);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param node New value for element.
     */
    public native void set1Value(int index, BaseNode node);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param node New value for element.
     */
    public native void set1Value(int index, ConstSFNode node);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param node New value for element.
     */
    public native void set1Value(int index, SFNode node);

    /**
     * Add a new element at the end of the list.
     *
     * @param node Element to add.
     */
    public native void addValue(BaseNode node);

    /**
     * Add a new element at the end of the list.
     *
     * @param node Element to add.
     */
    public native void addValue(ConstSFNode node);

    /**
     * Add a new element at the end of the list.
     *
     * @param node Element to add.
     */
    public native void addValue(SFNode node);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param node Value to insert.
     */
    public native void insertValue(int index, BaseNode node);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param node Value to insert.
     */
    public native void insertValue(int index, ConstSFNode node);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param node Value to insert.
     */
    public native void insertValue(int index, SFNode node);

    public native String toString();
}
