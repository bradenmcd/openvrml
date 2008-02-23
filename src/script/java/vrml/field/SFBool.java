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

/**
 * Represents a VRML SFBool field in Java.
 */
public class SFBool extends vrml.Field {

    /**
     * Construct a new SFBool field in OpenVRML using the given param.
     *
     * @param value Initial value of field.
     */
    static native long createPeer(boolean value);

    /**
     * Default constructor for SFBool fields.
     */
    public SFBool() {
        this(false);
    }

    /**
     * Construct from a peer handle.
     */
    SFBool(long peer) {
        super(peer);
    }

    /**
     * Construct an SFBool field.
     *
     * @param value Boolean value of field.
     */
    public SFBool(boolean value) {
        super(createPeer(value));
    }

    /**
     * Get the value of the SFBool field.
     *
     * @return Value of SFBool.
     */
    public native boolean getValue();

    /**
     * Set the value of the SFBool field.
     *
     * @param b Desired value for SFBool.
     */
    public native void setValue(boolean b);

    /**
     * Set the value of the SFBool field.
     *
     * @param b Desired value for SFBool.
     */
    public void setValue(ConstSFBool b) {
        this.setValue(b.getValue());
    }

    /**
     * Set the value of the SFBool field.
     *
     * @param b Desired value for SFBool.
     */
    public void setValue(SFBool b) {
        this.setValue(b.getValue());
    }
}
