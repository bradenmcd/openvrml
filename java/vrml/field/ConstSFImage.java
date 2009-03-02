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
 * Represents a read-only VRML SFImage field in Java.
 */
public class ConstSFImage extends ConstField {

    /**
     * Construct a new SFImage field in OpenVRML using the given values.
     *
     * @param width Initial width of SFImage.
     * @param height Initial height of SFImage.
     * @param components Number of components in SFImage.
     * @param pixels 2-dimensional pixel image.
     */
    private native void CreateObject(int width, int height, int components,
                                     byte pixels[]);

    /**
     * Construct a new SFImage field using the given values.
     *
     * @param width Initial width of SFImage.
     * @param height Initial height of SFImage.
     * @param components Number of components in SFImage.
     * @param pixels 2-dimensional pixel image.
     */
    public ConstSFImage(int width, int height, int components,
                        byte pixels[]) {
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
     * Get a 2-dimensional array of pixels representing the SFImage.
     *
     * @param pixels 2-dimensional pixel array.
     */
    public native void getPixels(byte pixels[]);

    public native String toString();
}
