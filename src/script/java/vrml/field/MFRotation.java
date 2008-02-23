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

/**
 * Represents a VRML MFRotation field in Java.
 */
public class MFRotation extends MField {

    /**
     * Construct a new MFRotation field in OpenVRML using the given params.
     *
     * @param rotations An array of four float tuples. (x, y, z, a)
     */
    static native long createPeer(float rotations[][]);

    /**
     * Construct a new MFRotation field in OpenVRML using the given params.
     *
     * @param size Number of rotations passed in.
     * @param rotations List of x, y, z, a 4-tuples
     */
    static native long createPeer(int size, float rotations[]);

    /**
     * Default constructor.
     */
    public MFRotation() {
        this(0, null);
    }

    /**
     * Construct from a peer handle.
     */
    MFRotation(long peer) {
        super(peer);
    }

    /**
     * Construct an MFRotation field.
     *
     * @param rotations An array of four float tuples. (x, y, z, a)
     */
    public MFRotation(float rotations[][]) {
        super(createPeer(rotations));
    }

    /**
     * Construct an MFRotation field.
     *
     * @param rotations List of x, y, z, a 4-tuples
     */
    public MFRotation(float rotations[]) {
        this(rotations.length / 4, rotations);
    }

    /**
     * Construct an MFRotation field.
     *
     * @param size Number of rotations passed in.
     * @param rotations List of x, y, z, a 4-tuples
     */
    public MFRotation(int size, float rotations[]) {
        super(createPeer(size, rotations));
    }

    public native int getSize();

    public native void clear();

    public native void delete(int index);

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
    public void get1Value(int index, SFRotation rotation) {
        float[] r = { 0.0f, 0.0f, 1.0f, 0.0f };
        this.get1Value(index, r);
        rotation.setValue(r);
    }

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
        this.setValue(rotations.length / 4, rotations);
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
    public void set1Value(int index, ConstSFRotation rotation) {
        float[] r = { 0.0f, 0.0f, 1.0f, 0.0f };
        rotation.getValue(r);
        this.set1Value(index, r[0], r[1], r[2], r[3]);
    }

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param rotation New value for element.
     */
    public void set1Value(int index, SFRotation rotation) {
        float[] r = { 0.0f, 0.0f, 1.0f, 0.0f };
        rotation.getValue(r);
        this.set1Value(index, r[0], r[1], r[2], r[3]);
    }

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
    public void addValue(ConstSFRotation rotation) {
        float[] r = { 0.0f, 0.0f, 1.0f, 0.0f };
        rotation.getValue(r);
        this.addValue(r[0], r[1], r[2], r[3]);
    }

    /**
     * Add a new element at the end of the list.
     *
     * @param rotation Element to add.
     */
    public void addValue(SFRotation rotation) {
        float[] r = { 0.0f, 0.0f, 1.0f, 0.0f };
        rotation.getValue(r);
        this.addValue(r[0], r[1], r[2], r[3]);
    }

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
    public void insertValue(int index, ConstSFRotation rotation) {
        float[] r = { 0.0f, 0.0f, 1.0f, 0.0f };
        rotation.getValue(r);
        this.insertValue(index, r[0], r[1], r[2], r[3]);
    }

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param rotation Value to insert.
     */
    public void insertValue(int index, SFRotation rotation) {
        float[] r = { 0.0f, 0.0f, 1.0f, 0.0f };
        rotation.getValue(r);
        this.insertValue(index, r[0], r[1], r[2], r[3]);
    }

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
}
