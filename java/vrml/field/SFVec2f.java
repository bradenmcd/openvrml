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
 * Represents a VRML SFVec2f field in Java.
 */
public class SFVec2f extends Field {

    /**
     * Construct a new SFVec2f field in OpenVRML using the given values.
     *
     * @param x X-component
     * @param y Y-component
     */
    private native void CreateObject(float x, float y);

    /**
     * Construct a default SFVec2f field.
     */
    public SFVec2f() {
        CreateObject(0f, 0f);
    }

    /**
     * Construct an SFVec2f field.
     *
     * @param x X-component
     * @param y Y-component
     */
    public SFVec2f(float x, float y) {
        CreateObject(x, y);
    }

    /**
     * Get the X and Y values representing the SFVec2f.
     *
     * @param vec2s X and Y values representing the SFVec2f
     */
    public native void getValue(float vec2s[]);

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
     * Set the value of an SFVec2f.
     *
     * @param vec2s X, Y value
     */
    public native void setValue(float vec2s[]);

    /**
     * Set the value of SFVec2f with the given values.
     *
     * @param x X-component of vector
     * @param y Y-component of vector
     */
    public native void setValue(float x, float y);

    /**
     * Set the value of an SFVec2f using the given ConstSFVec2f.
     *
     * @param vec ConstSFVec3f to take X, Y values from.
     */
    public native void setValue(ConstSFVec2f vec);

    /**
     * Set the value of an SFVec2f using the given SFVec2f.
     *
     * @param vec ConstSFVec3f to take X, Y values from.
     */
    public native void setValue(SFVec2f vec);

    public native String toString();
}
