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
 * Represents a VRML SFVec3f field in Java.
 */
public class SFVec3f extends Field {

    /**
     * Construct a new SFVec3f field in OpenVRML using the given values.
     *
     * @param x X-component
     * @param y Y-component
     * @param z Z-component
     */
    private native void CreateObject(float x, float y, float z);

    /**
     * Construct a default SFVec3f field.
     */
    public SFVec3f() {
        CreateObject(0f, 0f, 0f);
    }

    /**
     * Construct anSFVec3f field using the given values.
     *
     * @param x X-component
     * @param y Y-component
     * @param z Z-component
     */
    public SFVec3f(float x, float y, float z) {
        CreateObject(x, y, z);
    }

    /**
     * Get the X, Y and Z values representing the SFVec3f.
     *
     * @param vec3s X, Y and Z values representing the SFVec3f
     */
    public native void getValue(float vec3s[]);

    /**
     * Get the X-component of the vector.
     *
     * @return X-component of the vector
     */
    public native float getX();

    /**
     * Get the Y-component of the vector.
     *
     * @return Y-component of the vector
     */
    public native float getY();

    /**
     * Get the Z-component of the vector.
     *
     * @return Z-component of the vector
     */
    public native float getZ();

    /**
     * Set the value of an SFVec3f.
     *
     * @param vec3s X, Y, Z value
     */
    public native void setValue(float vec3s[]);

    /**
     * Set the value of SFVec3f with the given values.
     *
     * @param x X-component of vector
     * @param y Y-component of vector
     * @param z Z-component of vector
     */
    public native void setValue(float x, float y, float z);

    /**
     * Set the value of an SFVec3f using the given ConstSFVec3f.
     *
     * @param vec ConstSFVec3f to take X, Y, Z values from.
     */
    public native void setValue(ConstSFVec3f vec);

    /**
     * Set the value of an SFVec3f using the given SFVec3f.
     *
     * @param vec SFVec3f to take X, Y, Z values from.
     */
    public native void setValue(SFVec3f vec);

    public native String toString();
}
