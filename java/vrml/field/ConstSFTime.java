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
 * Represents a read-only VRML SFTime in Java.
 */
public class ConstSFTime extends ConstField {

    /**
     * Construct a new SFTime field in OpenVRML using the given value.
     *
     * @param time Initial value of field.
     */
    private native void CreateObject(double time);

    /**
     * Construct an SFTime field using the given value.
     *
     * @param time Initial value of field.
     */
    public ConstSFTime(double time) {
        CreateObject(time);
    }

    /**
     * Get the value of the SFTime field.
     *
     * @return Value of SFTime.
     */
    public native double getValue();

    public native String toString();
}
