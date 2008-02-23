//
// OpenVRML
//
// Copyright 2001  Henri Manson
// Copyright 2008  Braden McDaniel
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

import vrml.ConstMField;

/**
 * Represents a read-only VRML MFColor field in Java.
 */
public class ConstMFColor extends ConstMField {

    /**
     * Construct from a peer handle.
     */
    ConstMFColor(long peer) {
        super(peer);
    }

    /**
     * Construct a read-only MFColor field.
     *
     * @param colors An array of sets of three-float values (RGB)
     */
    public ConstMFColor(float colors[][]) {
        super(MFColor.createPeer(colors));
    }

    /**
     * Construct a read-only MFColor field.
     *
     * @param colors An array of RGB values.
     */
    public ConstMFColor(float colors[]) {
        this(colors.length / 3, colors);
    }

    /**
     * Construct a read-only MFColor field.
     *
     * @param size Number of RGB value passed in array.
     * @param colors Array of RGB values.
     */
    public ConstMFColor(int size, float colors[]) {
        super(MFColor.createPeer(size, colors));
    }

    public native int getSize();

    /**
     * Retrieves the value of an MFColor field.
     *
     * @param colors 2D array of sets of RGB values to be returned.
     */
    public native void getValue(float colors[][]);

    /**
     * Retrieves the value of an MFColor field.
     *
     * @param colors Array of sets of RGB values to be returned.
     */
    public native void getValue(float colors[]);

    /**
     * Retrieves a specific SFColor element in an MFColor and
     * returns it as a float array.
     *
     * @param index Position of desired SFColor
     * @param colors RGB value of specified SFColor.
     */
    public native void get1Value(int index, float colors[]);

    /**
     * Retrieves a specific SFColor element in an MFColor and
     * returns it as an SFColor object.
     *
     * @param index Position of desired SFColor
     * @param color SFColor that will be set to desired value.
     */
    public void get1Value(int index, SFColor color) {
        float[] c = { 0.0f, 0.0f, 0.0f };
        this.get1Value(index, c);
        color.setValue(c);
    }
}
