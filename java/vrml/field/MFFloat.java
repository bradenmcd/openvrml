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

/**
 * Represents a VRML MFFloat field in Java.
 */
public class MFFloat extends MField {

    public native int getSize();

    public native void clear();

    public native void delete(int index);

    /**
     * Construct a new MFFloat field in OpenVRML using the given params.
     *
     * @param size Number of SFFloat elements passed in.
     * @param values List of SFFloat values.
     */
    private native void CreateObject(int size, float values[]);

    /**
     * Construct a new MFFloat field in OpenVRML using the default params
     */
    public MFFloat() {
        CreateObject(0, null);
    }

    /**
     * Construct an MFFloat field in OpenVRML using the given params.
     *
     * @param size Number of SFFloat elements passed in.
     * @param values Array of SFFloat values.
     */
    public MFFloat(int size, float values[]) {
        CreateObject(size, values);
    }

    /**
     * Construct an MFFloat field in OpenVRML using the given params.
     *
     * @param values An array of SFFloat values.
     */
    public MFFloat(float values[]) {
        CreateObject(values.length, values);
    }

    /**
     * Retrieves the value of an MFFloat field.
     *
     * @param values Array of float values to be returned.
     */
    public native void getValue(float values[]);

    /**
     * Retrieves a specific SFFloat element in an MFFloat and
     * returns it as a float.
     *
     * @param index Position of desired SFFloat
     * @return Value of SFFloat at index.
     */
    public native float get1Value(int index);

    /**
     * Set the value of the field.
     *
     * @param values New value for field.
     */
    public void setValue(float values[]) {
        setValue(values.length, values);
    }

    /**
     * Set the value of the field.
     *
     * @param size Size of new value for field.
     * @param values New value for field.
     */
    public native void setValue(int size, float values[]);

    /**
     * Set the value of the field.
     *
     * @param value New value for field.
     */
    public native void setValue(MFFloat value);

    /**
     * Set the value of the field.
     *
     * @param value New value for field.
     */
    public native void setValue(ConstMFFloat value);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param f New value for element.
     */
    public native void set1Value(int index, float f);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param f New value for element.
     */
    public native void set1Value(int index, ConstSFFloat f);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param f New value for element.
     */
    public native void set1Value(int index, SFFloat f);

    /**
     * Add a new element at the end of the list.
     *
     * @param f Element to add.
     */
    public native void addValue(float f);

    /**
     * Add a new element at the end of the list.
     *
     * @param f Element to add.
     */
    public native void addValue(ConstSFFloat f);

    /**
     * Add a new element at the end of the list.
     *
     * @param f Element to add.
     */
    public native void addValue(SFFloat f);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param f Value to insert.
     */
    public native void insertValue(int index, float f);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param f SFFloat to insert.
     */
    public native void insertValue(int index, ConstSFFloat f);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param f SFFloat to insert.
     */
    public native void insertValue(int index, SFFloat f);

    public native String toString();
}
