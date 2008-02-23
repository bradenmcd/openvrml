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
    static native long createPeer(float axisX, float axisY, float axisZ,
                                  float angle);

    /**
     * Construct a default SFRotation field.
     */
    public SFRotation() {
        this(0f, 0f, 1f, 0f);
    }

    /**
     * Construct from a peer handle.
     */
    SFRotation(long peer) {
        super(peer);
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
        super(createPeer(axisX, axisY, axisZ, angle));
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
    public void setValue(float rotations[]) {
        this.setValue(rotations[0], rotations[1], rotations[2], rotations[3]);
    }

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
    public void setValue(ConstSFRotation rotation) {
        float[] r = { 0, 0, 1, 0 };
        rotation.getValue(r);
        this.setValue(r);
    }

    /**
     * Set the value of an SFRotation.
     *
     * @param rotation SFRotation to take values from
     */
    public void setValue(SFRotation rotation) {
        float[] r = { 0, 0, 1, 0 };
        rotation.getValue(r);
        this.setValue(r);
    }
}
