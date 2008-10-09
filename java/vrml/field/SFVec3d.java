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
 * Represents a VRML SFVec3d field in Java.
 */
public class SFVec3d extends Field {

    /**
     * Construct a new SFVec3d field in OpenVRML using the given values.
     *
     * @param x X-component
     * @param y Y-component
     * @param z Z-component
     */
    private native void CreateObject(double x, double y, double z);

    /**
     * Construct a default SFVec3d field.
     */
    public SFVec3d() {
        CreateObject(0f, 0f, 0f);
    }

    /**
     * Construct anSFVec3d field using the given values.
     *
     * @param x X-component
     * @param y Y-component
     * @param z Z-component
     */
    public SFVec3d(double x, double y, double z) {
        CreateObject(x, y, z);
    }

    /**
     * Get the X, Y and Z values representing the SFVec3d.
     *
     * @param vec3s X, Y and Z values representing the SFVec3d
     */
    public native void getValue(double vec3s[]);

    /**
     * Get the X-component of the vector.
     *
     * @return X-component of the vector
     */
    public native double getX();

    /**
     * Get the Y-component of the vector.
     *
     * @return Y-component of the vector
     */
    public native double getY();

    /**
     * Get the Z-component of the vector.
     *
     * @return Z-component of the vector
     */
    public native double getZ();

    /**
     * Set the value of an SFVec3d.
     *
     * @param vec3s X, Y, Z value
     */
    public native void setValue(double vec3s[]);

    /**
     * Set the value of SFVec3d with the given values.
     *
     * @param x X-component of vector
     * @param y Y-component of vector
     * @param z Z-component of vector
     */
    public native void setValue(double x, double y, double z);

    /**
     * Set the value of an SFVec3d using the given ConstSFVec3d.
     *
     * @param vec ConstSFVec3d to take X, Y, Z values from.
     */
    public native void setValue(ConstSFVec3d vec);

    /**
     * Set the value of an SFVec3d using the given SFVec3d.
     *
     * @param vec SFVec3d to take X, Y, Z values from.
     */
    public native void setValue(SFVec3d vec);

    public native String toString();
}
