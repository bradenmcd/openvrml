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
 * Represents a read-only VRML SFString field in Java.
 */
public class ConstSFString extends ConstField {

    /**
     * Construct a new SFString field in OpenVRML using the given string.
     *
     * @param value Initial value of SFString
     */
    private native void CreateObject(String value);

    /**
     * Construct a read-only SFString field.
     *
     * @param value Initial value of SFString
     */
    public ConstSFString(String value) {
        CreateObject(value);
    }

    /**
     * Get the value of the SFString field.
     *
     * @return Value of SFString
     */
    public native String getValue();

    public native String toString();
}
