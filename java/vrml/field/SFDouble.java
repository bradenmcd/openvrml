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
 * Represents a VRML SFDouble field in Java.
 */
public class SFDouble extends Field {

    /**
     * Construct a new SFDouble field in OpenVRML using the given value.
     *
     * @param value Initial value of field.
     */
    private native void CreateObject(double value);

    /**
     * Default constructor for SFDouble fields.
     */
    public SFDouble() {
        CreateObject(0.0f);
    }

    /**
     * Construct an SFDouble field.
     *
     * @param f Float value of field.
     */
    public SFDouble(double f) {
        CreateObject(f);
    }

    /**
     * Get the value of the SFDouble field.
     *
     * @return Value of SFDouble.
     */
    public native double getValue();

    /**
     * Set the value of the SFDouble field.
     *
     * @param f Desired value for SFDouble.
     */
    public native void setValue(double f);

    /**
     * Set the value of the SFDouble field.
     *
     * @param f Desired value for SFDouble.
     */
    public native void setValue(ConstSFDouble f);

    /**
     * Set the value of the SFDouble field.
     *
     * @param f Desired value for SFDouble.
     */
    public native void setValue(SFDouble f);

    public native String toString();
}
