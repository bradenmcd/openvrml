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

/**
 * Represents a read-only VRML MFRotation field in Java.
 */
public class ConstMFRotation extends ConstMField {

    public native int getSize();

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
     * Construct a read-only MFRotation field.
     *
     * @param rotations An array of four float tuples. (x, y, z, a)
     */
    public ConstMFRotation(float rotations[][]) {
        CreateObject(rotations);
    }

    /**
     * Construct a read-only MFRotation field.
     *
     * @param rotations List of x, y, z, a 4-tuples
     */
    public ConstMFRotation(float rotations[]) {
        CreateObject(rotations.length / 4, rotations);
    }

    /**
     * Construct a read-only MFRotation field.
     *
     * @param size Number of rotations passed in.
     * @param rotations List of x, y, z, a 4-tuples
     */
    public ConstMFRotation(int size, float rotations[]) {
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
    public native void get1Value(int index, float rotation[]);

    /**
     * Retrieves a specific rotation element in an MFRotation and
     * returns it as an SFRotation.
     *
     * @param index Position of desired rotation
     * @param rotation SFRotation to be set to desired value.
     */
    public native void get1Value(int index, SFRotation rotation);

    public native String toString();
}
