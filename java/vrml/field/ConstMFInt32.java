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

import vrml.ConstMField;

/**
 * Represents a read-only VRML MFInt32 field in Java.
 */
public class ConstMFInt32 extends ConstMField {

    public native int getSize();

    /**
     * Construct a new MFInt32 field in OpenVRML using the given params.
     *
     * @param size Number of SFInt32 values passed in.
     * @param values List of integers to initialize object with.
     */
    private native void CreateObject(int size, int values[]);

    /**
     * Construct a read-only MFInt32 field.
     *
     * @param size Number of integers to initialize object with.
     * @param values List of integers to initialize object with.
     */
    public ConstMFInt32(int size, int values[]) {
        CreateObject(size, values);
    }

    /**
     * Construct a read-only MFInt32 field.
     *
     * @param values List of integers to initialize object with.
     */
    public ConstMFInt32(int values[]) {
        CreateObject(values.length, values);
    }

    /**
     * Retrieves the complete list of integers making up an MFInt32 field.
     *
     * @param values Array to put resulting list of integers in.
     */
    public native void getValue(int values[]);

    /**
     * Retrieve a particular element from an MFInt32 field.
     *
     * @param index Position of desired element.
     * @return Value at specified position.
     */
    public native int get1Value(int index);

    public native String toString();
}
