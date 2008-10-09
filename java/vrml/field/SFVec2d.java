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
 * Represents a VRML SFVec2d field in Java.
 */
public class SFVec2d extends Field {

    /**
     * Construct a new SFVec2d field in OpenVRML using the given values.
     *
     * @param x X-component
     * @param y Y-component
     */
    private native void CreateObject(double x, double y);

    /**
     * Construct a default SFVec2d field.
     */
    public SFVec2d() {
        CreateObject(0f, 0f);
    }

    /**
     * Construct an SFVec2d field.
     *
     * @param x X-component
     * @param y Y-component
     */
    public SFVec2d(double x, double y) {
        CreateObject(x, y);
    }

    /**
     * Get the X and Y values representing the SFVec2d.
     *
     * @param vec2s X and Y values representing the SFVec2d
     */
    public native void getValue(double vec2s[]);

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
     * Set the value of an SFVec2d.
     *
     * @param vec2s X, Y value
     */
    public native void setValue(double vec2s[]);

    /**
     * Set the value of SFVec2d with the given values.
     *
     * @param x X-component of vector
     * @param y Y-component of vector
     */
    public native void setValue(double x, double y);

    /**
     * Set the value of an SFVec2d using the given ConstSFVec2d.
     *
     * @param vec ConstSFVec3f to take X, Y values from.
     */
    public native void setValue(ConstSFVec2d vec);

    /**
     * Set the value of an SFVec2d using the given SFVec2d.
     *
     * @param vec ConstSFVec3f to take X, Y values from.
     */
    public native void setValue(SFVec2d vec);

    public native String toString();
}
