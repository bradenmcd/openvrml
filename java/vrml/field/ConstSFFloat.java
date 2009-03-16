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
 * Represents a read-only VRML SFFloat field in Java.
 */
public class ConstSFFloat extends ConstField {

    /**
     * Construct a new SFFloat in OpenVRML using the given value.
     *
     * @param value Value of SFFloat.
     */
    private native void CreateObject(float value);

    /**
     * Construct a read-only SFFloat field.
     *
     * @param value Value of SFFloat.
     */
    public ConstSFFloat(float value) {
        CreateObject(value);
    }

    /**
     * Get the value of an SFFloat field.
     *
     * @return Value of SFFloat.
     */
    public native float getValue();

    public native String toString();
}
