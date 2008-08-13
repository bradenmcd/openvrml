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

import vrml.ConstField;

/**
 * Represents a read-only VRML SFRotation field in Java.
 */
public class ConstSFRotation extends ConstField {

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
     * Construct a read-only SFRotation field.
     *
     * @param axisX X-component of the rotation
     * @param axisY Y-component of the rotation
     * @param axisZ Z-component of the rotation
     * @param angle Angle of the rotation
     */
    public ConstSFRotation(float axisX, float axisY, float axisZ,
                           float angle) {
        CreateObject(axisX, axisY, axisZ, angle);
    }

    /**
     * Get the X, Y, Z and angle values representing the SFRotation.
     *
     * @param rotations X, Y, Z and angle values representing the SFRotation
     */
    public native void getValue(float rotations[]);

    public native String toString();
}
