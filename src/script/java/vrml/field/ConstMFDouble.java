//
// OpenVRML
//
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
 * Represents a read-only VRML MFDouble field in Java.
 */
public class ConstMFDouble extends ConstMField {

    /**
     * Construct from a peer handle.
     */
    ConstMFDouble(long peer) {
        super(peer);
    }

    /**
     * Construct a read-only MFDouble field.
     *
     * @param size Number of SFDouble elements passed in.
     * @param values Array of SFDouble values.
     */
    public ConstMFDouble(int size, double values[]) {
        super(MFDouble.createPeer(size, values));
    }

    /**
     * Construct a read-only MFDouble field.
     *
     * @param values An array of SFDouble values.
     */
    public ConstMFDouble(double values[]) {
        this(values.length, values);
    }

    public native int getSize();

    /**
     * Retrieves the value of an MFDouble field.
     *
     * @param values Array of double values to be returned.
     */
    public native void getValue(double values[]);

    /**
     * Retrieves a specific SFDouble element in an MFDouble and
     * returns it as a double.
     *
     * @param index Position of desired SFDouble
     * @return Value of SFDouble at index.
     */
    public native double get1Value(int index);
}
