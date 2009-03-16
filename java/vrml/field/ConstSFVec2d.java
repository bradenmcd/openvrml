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
 * Represents a read-only VRML SFVec2d field in Java.
 */
public class ConstSFVec2d extends ConstField {

    /**
     * Construct a new SFVec2d field in OpenVRML using the given values.
     *
     * @param x X-component
     * @param y Y-component
     */
    private native void CreateObject(double x, double y);

    /**
     * Construct a read-only SFVec2d field.
     *
     * @param x X-component
     * @param y Y-component
     */
    public ConstSFVec2d(double x, double y) {
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

    public native String toString();
}
