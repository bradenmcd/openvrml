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
 * Represents a read-only VRML MFFloat field in Java.
 */
public class ConstMFFloat extends ConstMField {

    /**
     * Construct from a peer handle.
     */
    ConstMFFloat(long peer) {
        super(peer);
    }

    /**
     * Construct a read-only MFFloat field.
     *
     * @param size Number of SFFloat elements passed in.
     * @param values Array of SFFloat values.
     */
    public ConstMFFloat(int size, float values[]) {
        super(MFFloat.createPeer(size, values));
    }

    /**
     * Construct a read-only MFFloat field.
     *
     * @param values An array of SFFloat values.
     */
    public ConstMFFloat(float values[]) {
        this(values.length, values);
    }

    public native int getSize();

    /**
     * Retrieves the value of an MFFloat field.
     *
     * @param values Array of float values to be returned.
     */
    public native void getValue(float values[]);

    /**
     * Retrieves a specific SFFloat element in an MFFloat and
     * returns it as a float.
     *
     * @param index Position of desired SFFloat
     * @return Value of SFFloat at index.
     */
    public native float get1Value(int index);
}
