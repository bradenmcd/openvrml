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
 * Represents a VRML SFColor field in Java.
 */
public class SFColor extends Field {

    /**
     * Construct a new SFColor field in OpenVRML using the given RGB value.
     *
     * @param red R-component of color
     * @param green G-component of color
     * @param blue B-component of color
     */
    private native void CreateObject(float red, float green, float blue);

    /**
     * Construct an SFColor field with the default values.
     */
    public SFColor() {
        CreateObject(0f, 0f, 0f);
    }

    /**
     * Construct an SFColor field.
     *
     * @param red R-component of color
     * @param green G-component of color
     * @param blue B-component of color
     */
    public SFColor(float red, float green, float blue) {
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

    /**
     * Set the value of an SFColor.
     *
     * @param colors R, G, B value
     */
    public native void setValue(float colors[]);

    /**
     * Set an SFColor with the given RGB values.
     *
     * @param red R-component of the color
     * @param green G-component of the color
     * @param blue B-component of the color
     */
    public native void setValue(float red, float green, float blue);

    /**
     * Set the value of an SFColor using the given SFColor.
     *
     * @param color ConstSFColor to take RGB value from
     */
    public native void setValue(ConstSFColor color);

    /**
     * Set the value of an SFColor using the given SFColor.
     *
     * @param color SFColor to take RGB value from
     */
    public native void setValue(SFColor color);

    public native String toString();
}
