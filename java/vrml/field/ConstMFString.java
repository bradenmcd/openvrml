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

import vrml.*;

/**
 * Represents a read-only VRML MFString field in Java.
 */
public class ConstMFString extends ConstMField {

    public native int getSize();

    /**
     * Construct a new MFString field in OpenVRML using the given params.
     *
     * @param size Number of Strings passed in.
     * @param s Arrays of strings.
     */
    private native void CreateObject(int size, String s[]);

    /**
     * Construct a read-only MFString field.
     *
     * @param size Number of Strings passed in.
     * @param s Arrays of strings.
     */
    public ConstMFString(int size, String s[]) {
        CreateObject(size, s);
    }

    /**
     * Construct a read-only MFString field.
     *
     * @param s Arrays of strings.
     */
    public ConstMFString(String s[]) {
        CreateObject(s.length, s);
    }

    /**
     * Retrieves the value of an MFString field.
     *
     * @param values Array of strings to be returned.
     */
    public native void getValue(String values[]);

    /**
     * Retrieves a specific SFString element in an MFString and
     * returns it as a String.
     *
     * @param index Position of desired SFString
     * @return Value of SFString at index.
     */
    public native String get1Value(int index);

    public native String toString();
}
