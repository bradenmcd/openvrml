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
 * Represents a VRML SFInt32 field in Java.
 */
public class SFInt32 extends Field {

    /**
     * Construct a new SFInt32 field in OpenVRML using the given value.
     *
     * @param value Initial value of field.
     */
    private native void CreateObject(int value);

    /**
     * Construct a default SFInt32 field.
     */
    public SFInt32() {
        CreateObject(0);
    }

    /**
     * Construct an SFInt32 field.
     *
     * @param value Initial value of field.
     */
    public SFInt32(int value) {
        CreateObject(value);
    }

    /**
     * Get the value of the SFInt32 field.
     *
     * @return Value of field.
     */
    public native int getValue();

    /**
     * Set the value of an SFInt32 field.
     *
     * @param i Value to set field to.
     */
    public native void setValue(int i);

    /**
     * Set the value of an SFInt32 field.
     *
     * @param i Value to set field to.
     */
    public native void setValue(ConstSFInt32 i);

    /**
     * Set the value of an SFInt32 field.
     *
     * @param i Value to set field to.
     */
    public native void setValue(SFInt32 i);

    public native String toString();
}
