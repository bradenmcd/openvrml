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
 * Represents a VRML MFRotation field in Java.
 */
public class MFRotation extends MField {

    public native int getSize();

    public native void clear();

    public native void delete(int index);

    /**
     * Construct a new MFRotation field in OpenVRML using the given params.
     *
     * @param rotations An array of four float tuples. (x, y, z, a)
     */
    private native void CreateObject(float rotations[][]);

    /**
     * Construct a new MFRotation field in OpenVRML using the given params.
     *
     * @param size Number of rotations passed in.
     * @param rotations List of x, y, z, a 4-tuples
     */
    private native void CreateObject(int size, float rotations[]);

    /**
     * Default constructor.
     */
    public MFRotation() {
        float[] dummy = new float[0];
        CreateObject(0, dummy);
    }

    /**
     * Construct an MFRotation field.
     *
     * @param rotations An array of four float tuples. (x, y, z, a)
     */
    public MFRotation(float rotations[][]) {
        CreateObject(rotations);
    }

    /**
     * Construct an MFRotation field.
     *
     * @param rotations List of x, y, z, a 4-tuples
     */
    public MFRotation(float rotations[]) {
        CreateObject(rotations.length / 4, rotations);
    }

    /**
     * Construct an MFRotation field.
     *
     * @param size Number of rotations passed in.
     * @param rotations List of x, y, z, a 4-tuples
     */
    public MFRotation(int size, float rotations[]) {
        CreateObject(size, rotations);
    }

    /**
     * Retrieves the value of an MFRotation field.
     *
     * @param rotations 2D array of sets of rotation values to be returned.
     */
    public native void getValue(float rotations[][]);

    /**
     * Retrieves the value of an MFRotation field.
     *
     * @param rotations Array of sets of rotations values to be returned.
     */
    public native void getValue(float rotations[]);

    /**
     * Retrieves a specific rotation element in an MFRotation and
     * returns it as a float array.
     *
     * @param index Position of desired rotation
     * @param rotation Value of specified rotation.
     */
    public native void get1Value(int index, float rotations[]);

    /**
     * Retrieves a specific rotation element in an MFRotation and
     * returns it as an SFRotation.
     *
     * @param index Position of desired rotation
     * @param rotation SFRotation to be set to desired value.
     */
    public native void get1Value(int index, SFRotation rotation);

    /**
     * Set the value of the field.
     *
     * @param rotations New value for field.
     */
    public native void setValue(float rotations[][]);

    /**
     * Set the value of the field.
     *
     * @param rotations New value for field.
     */
    public void setValue(float rotations[]) {
        setValue(rotations.length / 4, rotations);
    }

    /**
     * Set the value of the field.
     *
     * @param size Size of new value for field.
     * @param rotations New value for field.
     */
    public native void setValue(int size, float rotations[]);

    /**
     * Set the value of the field.
     *
     * @param rotations New value for field.
     */
    public native void setValue(MFRotation rotations);

    /**
     * Set the value of the field.
     *
     * @param rotations New value for field.
     */
    public native void setValue(ConstMFRotation rotations);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param rotation New value for element.
     */
    public native void set1Value(int index, ConstSFRotation rotation);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param rotation New value for element.
     */
    public native void set1Value(int index, SFRotation rotation);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param axisX Rotation along X-axis of new value.
     * @param axisY Rotation along Y-axis of new value.
     * @param axisZ Rotation along Z-axis of new value.
     * @param angle Angle of new element.
     */
    public native void set1Value(int index, float axisX, float axisY,
                                 float axisZ, float angle);

    /**
     * Add a new element at the end of the list.
     *
     * @param rotation Element to add.
     */
    public native void addValue(ConstSFRotation rotation);

    /**
     * Add a new element at the end of the list.
     *
     * @param rotation Element to add.
     */
    public native void addValue(SFRotation rotation);

    /**
     * Add a new element at the end of the list.
     *
     * @param axisX Rotation along X-axis of new element.
     * @param axisY Rotation along Y-axis of new element.
     * @param axisZ Rotation along Z-axis of new element.
     * @param angle Angle of new element.
     */
    public native void addValue(float axisX, float axisY, float axisZ,
                                float angle);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param rotation Value to insert.
     */
    public native void insertValue(int index, ConstSFRotation rotation);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param rotation Value to insert.
     */
    public native void insertValue(int index, SFRotation rotation);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param axisX Rotation along X-axis of rotation.
     * @param axisY Rotation along Y-axis of rotation.
     * @param axisZ Rotation along Z-axis of rotation.
     * @param angle Angle of rotation.
     */
    public native void insertValue(int index, float axisX, float axisY,
                                   float axisZ, float angle);

    public native String toString();
}
