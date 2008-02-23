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
 * Represents a VRML SFFloat field in Java.
 */
public class SFFloat extends Field {

    /**
     * Construct a new SFFloat field in OpenVRML using the given value.
     *
     * @param value Initial value of field.
     */
    static native long createPeer(float value);

    /**
     * Default constructor for SFFloat fields.
     */
    public SFFloat() {
        this(0.0f);
    }

    /**
     * Construct from a peer handle.
     */
    SFFloat(long peer) {
        super(peer);
    }

    /**
     * Construct an SFFloat field.
     *
     * @param f Float value of field.
     */
    public SFFloat(float f) {
        super(createPeer(f));
    }

    /**
     * Get the value of the SFFloat field.
     *
     * @return Value of SFFloat.
     */
    public native float getValue();

    /**
     * Set the value of the SFFloat field.
     *
     * @param f Desired value for SFFloat.
     */
    public native void setValue(float f);

    /**
     * Set the value of the SFFloat field.
     *
     * @param f Desired value for SFFloat.
     */
    public void setValue(ConstSFFloat f) {
        this.setValue(f.getValue());
    }

    /**
     * Set the value of the SFFloat field.
     *
     * @param f Desired value for SFFloat.
     */
    public void setValue(SFFloat f) {
        this.setValue(f.getValue());
    }
}
