//
// OpenVRML
//
// Copyright 2005  Braden McDaniel
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

public class MFBool extends MField {

    /**
     * Construct a new MFBool field in OpenVRML using the given params.
     *
     * @param size Number of SFBool values passed in.
     * @param values List of booleans to initialize object with.
     */
    static native long createPeer(int size, boolean values[]);

    /**
     * Default constructor
     */
    public MFBool() {
        this(0, null);
    }

    /**
     * Construct from a peer handle.
     */
    MFBool(long peer) {
        super(peer);
    }

    /**
     * Construct an MFBool field.
     *
     * @param size Number of booleans to initialize object with.
     * @param values List of booleans to initialize object with.
     */
    public MFBool(int size, boolean values[]) {
        super(createPeer(size, values));
    }

    /**
     * Construct an MFBool field.
     *
     * @param values List of booleans to initialize object with.
     */
    public MFBool(boolean values[]) {
        this(values.length, values);
    }

    public native int getSize();

    public native void clear();

    public native void delete(int index);

    /**
     * Retrieves the complete list of booleans making up an MFBool field.
     *
     * @param values Array to put resulting list of booleans in.
     */
    public native void getValue(boolean values[]);

    /**
     * Retrieve a particular element from an MFBool field.
     *
     * @param index Position of desired element.
     * @return Value at specified position.
     */
    public native boolean get1Value(int index);

    /**
     * Set the value of the field.
     *
     * @param values New value for field.
     */
    public void setValue(boolean values[]) {
        setValue(values.length, values);
    }

    /**
     * Set the value of the field.
     *
     * @param size Size of new value for field.
     * @param values New value for field.
     */
    public native void setValue(int size, boolean values[]);

    /**
     * Set the value of the field.
     *
     * @param value New value for field.
     */
    public native void setValue(MFBool value);

    /**
     * Set the value of the field.
     *
     * @param value New value for field.
     */
    public native void setValue(ConstMFBool value);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param b New value for element.
     */
    public native void set1Value(int index, boolean b);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param b New value for element.
     */
    public void set1Value(int index, ConstSFBool b) {
        this.set1Value(index, b.getValue());
    }

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param b New value for element.
     */
    public void set1Value(int index, SFBool b) {
        this.set1Value(index, b.getValue());
    }

    /**
     * Add a new element at the end of the list.
     *
     * @param b Element to add.
     */
    public native void addValue(boolean b);

    /**
     * Add a new element at the end of the list.
     *
     * @param b Element to add.
     */
    public void addValue(ConstSFBool b) {
        this.addValue(b.getValue());
    }

    /**
     * Add a new element at the end of the list.
     *
     * @param b Element to add.
     */
    public void addValue(SFBool b) {
        this.addValue(b.getValue());
    }

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param b Value to insert.
     */
    public native void insertValue(int index, boolean b);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param b Value to insert.
     */
    public void insertValue(int index, ConstSFBool b) {
        this.insertValue(index, b.getValue());
    }

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param b Value to insert.
     */
    public void insertValue(int index, SFBool b) {
        this.insertValue(index, b.getValue());
    }
}
