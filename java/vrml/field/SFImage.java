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
 * Represents a VRML SFImage field in Java.
 */
public class SFImage extends Field {

    /**
     * Construct a new SFBool field in OpenVRML using the given param.
     *
     * @param width Initial width of SFImage.
     * @param height Initial height of SFImage.
     * @param components Number of components in SFImage.
     * @param pixels 2-dimensional pixel image.
     */
    private native void CreateObject(int width, int height,
                                     int components, byte pixels[]);

    /**
     * Construct a default SFImage field.
     */
    public SFImage() {
        CreateObject(0, 0, 0, null);
    }

    /**
     * Construct a new SFImage field using the given values.
     *
     * @param width Initial width of SFImage.
     * @param height Initial height of SFImage.
     * @param components Number of components in SFImage.
     * @param pixels 2-dimensional pixel image.
     */
    public SFImage(int width, int height, int components, byte pixels[]) {
        CreateObject(width, height, components, pixels);
    }

    /**
     * Get the width of the SFImage.
     *
     * @return Width of SFImage.
     */
    public native int getWidth();

    /**
     * Get the height of the SFImage.
     *
     * @return Height of SFImage.
     */
    public native int getHeight();

    /**
     * Get the number of components in an SFImage.
     *
     * @return Number of components in SFImage.
     */
    public native int getComponents();

    /**
     * Get a 2-dimensional array of pixels representing the
     * SFImage.
     *
     * @param pixels 2-dimensional pixel array.
     */
    public native void getPixels(byte pixels[]);

    /**
     * Set the value of an SFImage field.
     *
     * @param width New width of SFImage.
     * @param height New height of SFImage.
     * @param components Number of components in SFImage.
     * @param pixels 2-dimensional pixel image.
     */
    public native void setValue(int width, int height, int components,
                                byte pixels[]);

    /**
     * Set the value of an SFImage field.
     *
     * @param image Desired value for SFImage.
     */
    public native void setValue(ConstSFImage image);

    /**
     * Set the value of an SFImage field.
     *
     * @param image Desired value for SFImage.
     */
    public native void setValue(SFImage image);

    public native String toString();
}
