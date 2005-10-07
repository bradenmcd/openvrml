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
 * Represents a VRML MFDouble field in Java.
 */
public class MFDouble extends MField {

    public native int getSize();

    public native void clear();

    public native void delete(int index);

    /**
     * Construct a new MFDouble field in OpenVRML using the given params.
     *
     * @param size Number of SFDouble elements passed in.
     * @param values List of SFDouble values.
     */
    private native void CreateObject(int size, double values[]);

    /**
     * Construct a new MFDouble field in OpenVRML using the default params
     */
    public MFDouble() {
        CreateObject(0, null);
    }

    /**
     * Construct an MFDouble field in OpenVRML using the given params.
     *
     * @param size Number of SFDouble elements passed in.
     * @param values Array of SFDouble values.
     */
    public MFDouble(int size, double values[]) {
        CreateObject(size, values);
    }

    /**
     * Construct an MFDouble field in OpenVRML using the given params.
     *
     * @param values An array of SFDouble values.
     */
    public MFDouble(double values[]) {
        CreateObject(values.length, values);
    }

    /**
     * Retrieves the value of an MFDouble field.
     *
     * @param values Array of double values to be returned.
     */
    public native void getValue(double values[]);

    /**
     * Retrieves a specific SFDouble element in an MFDouble and
     * returns it as a double.
     *
     * @param index Position of desired SFDouble
     * @return Value of SFDouble at index.
     */
    public native double get1Value(int index);

    /**
     * Set the value of the field.
     *
     * @param values New value for field.
     */
    public void setValue(double values[]) {
        setValue(values.length, values);
    }

    /**
     * Set the value of the field.
     *
     * @param size Size of new value for field.
     * @param values New value for field.
     */
    public native void setValue(int size, double values[]);

    /**
     * Set the value of the field.
     *
     * @param value New value for field.
     */
    public native void setValue(MFDouble value);

    /**
     * Set the value of the field.
     *
     * @param value New value for field.
     */
    public native void setValue(ConstMFDouble value);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param f New value for element.
     */
    public native void set1Value(int index, double f);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param f New value for element.
     */
    public native void set1Value(int index, ConstSFDouble f);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param f New value for element.
     */
    public native void set1Value(int index, SFDouble f);

    /**
     * Add a new element at the end of the list.
     *
     * @param f Element to add.
     */
    public native void addValue(double f);

    /**
     * Add a new element at the end of the list.
     *
     * @param f Element to add.
     */
    public native void addValue(ConstSFDouble f);

    /**
     * Add a new element at the end of the list.
     *
     * @param f Element to add.
     */
    public native void addValue(SFDouble f);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param f Value to insert.
     */
    public native void insertValue(int index, double f);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param f SFDouble to insert.
     */
    public native void insertValue(int index, ConstSFDouble f);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param f SFDouble to insert.
     */
    public native void insertValue(int index, SFDouble f);

    public native String toString();
}
