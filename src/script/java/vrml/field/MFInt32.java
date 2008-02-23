//
// OpenVRML
//
// Copyright 2001  Henri Manson
// Copyright 2008  Braden McDaniel
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

public class MFInt32 extends MField {

    /**
     * Construct a new MFInt32 field in OpenVRML using the given params.
     *
     * @param size Number of SFInt32 values passed in.
     * @param values List of integers to initialize object with.
     */
    static native long createPeer(int size, int values[]);

    /**
     * Default constructor
     */
    public MFInt32() {
        this(0, null);
    }

    /**
     * Construct from a peer handle.
     */
    MFInt32(long peer) {
        super(peer);
    }

    /**
     * Construct an MFInt32 field.
     *
     * @param size Number of integers to initialize object with.
     * @param values List of integers to initialize object with.
     */
    public MFInt32(int size, int values[]) {
        super(createPeer(size, values));
    }

    /**
     * Construct an MFInt32 field.
     *
     * @param values List of integers to initialize object with.
     */
    public MFInt32(int values[]) {
        this(values.length, values);
    }

    public native int getSize();

    public native void clear();

    public native void delete(int index);

    /**
     * Retrieves the complete list of integers making up an MFInt32 field.
     *
     * @param values Array to put resulting list of integers in.
     */
    public native void getValue(int values[]);

    /**
     * Retrieve a particular element from an MFInt32 field.
     *
     * @param index Position of desired element.
     * @return Value at specified position.
     */
    public native int get1Value(int index);

    /**
     * Set the value of the field.
     *
     * @param values New value for field.
     */
    public void setValue(int values[]) {
        this.setValue(values.length, values);
    }

    /**
     * Set the value of the field.
     *
     * @param size Size of new value for field.
     * @param values New value for field.
     */
    public native void setValue(int size, int values[]);

    /**
     * Set the value of the field.
     *
     * @param value New value for field.
     */
    public native void setValue(MFInt32 value);

    /**
     * Set the value of the field.
     *
     * @param value New value for field.
     */
    public native void setValue(ConstMFInt32 value);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param i New value for element.
     */
    public native void set1Value(int index, int i);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param i New value for element.
     */
    public void set1Value(int index, ConstSFInt32 i) {
        this.set1Value(index, i.getValue());
    }

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param i New value for element.
     */
    public void set1Value(int index, SFInt32 i) {
        this.set1Value(index, i.getValue());
    }

    /**
     * Add a new element at the end of the list.
     *
     * @param i Element to add.
     */
    public native void addValue(int i);

    /**
     * Add a new element at the end of the list.
     *
     * @param i Element to add.
     */
    public void addValue(ConstSFInt32 i) {
        this.addValue(i.getValue());
    }

    /**
     * Add a new element at the end of the list.
     *
     * @param i Element to add.
     */
    public void addValue(SFInt32 i) {
        this.addValue(i.getValue());
    }

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param i Value to insert.
     */
    public native void insertValue(int index, int i);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param i Value to insert.
     */
    public void insertValue(int index, ConstSFInt32 i) {
        this.insertValue(index, i.getValue());
    }

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param i Value to insert.
     */
    public void insertValue(int index, SFInt32 i) {
        this.insertValue(index, i.getValue());
    }
}
