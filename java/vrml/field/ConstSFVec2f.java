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
 * Represents a read-only VRML SFVec2f field in Java.
 */
public class ConstSFVec2f extends ConstField {

    /**
     * Construct a new SFVec2f field in OpenVRML using the given values.
     *
     * @param x X-component
     * @param y Y-component
     */
    private native void CreateObject(float x, float y);

    /**
     * Construct a read-only SFVec2f field.
     *
     * @param x X-component
     * @param y Y-component
     */
    public ConstSFVec2f(float x, float y) {
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

    public native String toString();
}
