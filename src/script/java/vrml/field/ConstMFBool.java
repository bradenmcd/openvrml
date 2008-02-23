//
// OpenVRML
//
// Copyright 2005, 2008  Braden McDaniel
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
 * Represents a read-only VRML MFBool field in Java.
 */
public class ConstMFBool extends ConstMField {

    /**
     * Construct from a peer handle.
     */
    ConstMFBool(long peer) {
        super(peer);
    }

    /**
     * Construct a read-only MFBool field.
     *
     * @param size Number of booleans to initialize object with.
     * @param values List of booleans to initialize object with.
     */
    public ConstMFBool(int size, boolean values[]) {
        super(MFBool.createPeer(size, values));
    }

    /**
     * Construct a read-only MFBool field.
     *
     * @param values List of booleans to initialize object with.
     */
    public ConstMFBool(boolean values[]) {
        this(values.length, values);
    }

    public native int getSize();

    /**
     * Retrieves the complete list of booleans making up an MFBool field.
     *
     * @param values Array to put resulting list of booleans in.
     */
    public native void getValue(boolean values[]);

    /**
     * Retrieve a particular element from an MFBool field.
     *
     * @param index Position of desired element.
     * @return Value at specified position.
     */
    public native boolean get1Value(int index);
}
