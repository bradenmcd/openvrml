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
 * Represents a read-only VRML SFVec3f field in Java.
 */
public class ConstSFVec3f extends ConstField {

    /**
     * Construct a new SFVec3f field in OpenVRML using the given values.
     *
     * @param x X-component
     * @param y Y-component
     * @param z Z-component
     */
    private native void CreateObject(float x, float y, float z);

    /**
     * Construct a read-only SFVec3f field.
     *
     * @param x X-component
     * @param y Y-component
     * @param z Z-component
     */
    public ConstSFVec3f(float x, float y, float z) {
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

    public native String toString();
}
