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
 * Represents a read-only VRML SFColor field in Java.
 */
public class ConstSFColor extends ConstField {

    /**
     * Construct a new SFColor field in OpenVRML using the given RGB value.
     *
     * @param red R-component of color
     * @param green G-component of color
     * @param blue B-component of color
     */
    private native void CreateObject(float red, float green, float blue);

    /**
     * Construct a read-only SFColor field.
     *
     * @param red R-component of color
     * @param green G-component of color
     * @param blue B-component of color
     */
    public ConstSFColor(float red, float green, float blue) {
        CreateObject(red, green, blue);
    }

    /**
     * Get the R,G,B values representing the SFColor.
     *
     * @param colors R,G,B value representing the SFColor
     */
    public native void getValue(float colors[]);

    /**
     * Get the R-component of the SFColor.
     *
     * @return R-component of the SFColor
     */
    public native float getRed();

    /**
     * Get the G-component of the SFColor.
     *
     * @return G-component of the SFColor
     */
    public native float getGreen();

    /**
     * Get the B-component of the SFColor.
     *
     * @return B-component of the SFColor
     */
    public native float getBlue();

    public native String toString();
}
