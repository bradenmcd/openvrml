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

/**
 * Represents a VRML SFDouble field in Java.
 */
public class SFDouble extends vrml.Field {

    /**
     * Construct a new SFDouble field in OpenVRML using the given value.
     *
     * @param value Initial value of field.
     */
    static native long createPeer(double value);

    /**
     * Default constructor for SFDouble fields.
     */
    public SFDouble() {
        this(0.0);
    }

    /**
     * Construct from a peer handle.
     */
    SFDouble(long peer) {
        super(peer);
    }

    /**
     * Construct an SFDouble field.
     *
     * @param d Float value of field.
     */
    public SFDouble(double d) {
        super(createPeer(d));
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
     * @param d Desired value for SFDouble.
     */
    public native void setValue(double d);

    /**
     * Set the value of the SFDouble field.
     *
     * @param d Desired value for SFDouble.
     */
    public void setValue(ConstSFDouble d) {
        this.setValue(d.getValue());
    }

    /**
     * Set the value of the SFDouble field.
     *
     * @param d Desired value for SFDouble.
     */
    public void setValue(SFDouble d) {
        this.setValue(d.getValue());
    }
}
