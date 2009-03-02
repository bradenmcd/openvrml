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

/**
 * Represents a VRML SFRotation field in Java.
 */
public class SFRotation extends Field {

    /**
     * Construct a new SFRotation field in OpenVRML using the given values.
     *
     * @param axisX X-component of the rotation
     * @param axisY Y-component of the rotation
     * @param axisZ Z-component of the rotation
     * @param angle Angle of the rotation
     */
    private native void CreateObject(float axisX, float axisY,
                                     float axisZ, float angle);

    /**
     * Construct a default SFRotation field.
     */
    public SFRotation() {
        CreateObject(0f, 0f, 1f, 0f);
    }

    /**
     * Construct an SFRotation field.
     *
     * @param axisX X-component of the rotation
     * @param axisY Y-component of the rotation
     * @param axisZ Z-component of the rotation
     * @param angle Angle of the rotation
     */
    public SFRotation(float axisX, float axisY, float axisZ, float angle) {
        CreateObject(axisX, axisY, axisZ, angle);
    }

    /**
     * Get the X, Y, Z and angle values representing the SFRotation.
     *
     * @param rotations X, Y, Z and angle values representing the SFRotation
     */
    public native void getValue(float rotations[]);

    /**
     * Set the X, Y, Z and angle values of an SFRotation.
     *
     * @param rotations X, Y, Z and angle values to set SFRotation with
     */
    public native void setValue(float rotations[]);

    /**
     * Set the X, Y, Z and angle values of an SFRotation.
     *
     * @param axisX X-component of the rotation
     * @param axisY Y-component of the rotation
     * @param axisZ Z-component of the rotation
     * @param angle Angle of the rotation
     */
    public native void setValue(float axisX, float axisY, float axisZ,
                                float angle);

    /**
     * Set the value of an SFRotation.
     *
     * @param rotation ConstSFRotation to take values from
     */
    public native void setValue(ConstSFRotation rotation);

    /**
     * Set the value of an SFRotation.
     *
     * @param rotation SFRotation to take values from
     */
    public native void setValue(SFRotation rotation);

    public native String toString();
}
