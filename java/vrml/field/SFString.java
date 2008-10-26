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
 * Represents a VRML SFString field in Java.
 */
public class SFString extends Field {

    /**
     * Construct a new SFString field in OpenVRML using the given string.
     *
     * @param value Initial value of SFString
     */
    private native void CreateObject(String value);

    /**
     * Construct a default SFString field.
     */
    public SFString() {
        CreateObject(null);
    }

    /**
     * Construct an SFString field using the given value.
     *
     * @param s Initial value for SFString
     */
    public SFString(String s) {
        CreateObject(s);
    }

    /**
     * Get the value of the SFString field.
     *
     * @return Value of SFString.
     */
    public native String getValue();

    /**
     * Set the value of an SFString field.
     *
     * @param s Value to set field to.
     */
    public native void setValue(String s);

    /**
     * Set the value of an SFString field using an existing ConstSFString.
     *
     * @param s Value to set field to.
     */
    public native void setValue(ConstSFString s);

    /**
     * Set the value of an SFString field using an existing SFString.
     *
     * @param s Value to set field to.
     */
    public native void setValue(SFString s);

    public native String toString();
}
