//
// OpenVRML
//
// Copyright 2001  Henri Manson
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

import vrml.ConstMField;

/**
 * Represents a read-only VRML MFVec3f field in Java.
 */
public class ConstMFVec3f extends ConstMField {

    /**
     * Construct from a peer handle.
     */
    ConstMFVec3f(long peer) {
        super(peer);
    }

    /**
     * Construct a read-only MFVec3f field.
     *
     * @param vec3s An array of sets of x,y,z values
     */
    public ConstMFVec3f(float vec3s[][]) {
        super(MFVec3f.createPeer(vec3s));
    }

    /**
     * Construct a read-only MFVec3f field.
     *
     * @param vec3s List of x,y,z tuples
     */
    public ConstMFVec3f(float vec3s[]) {
        this(vec3s.length / 3, vec3s);
    }

    /**
     * Construct a read-only MFVec3f field.
     *
     * @param size Number of SFVec3f elements passed in.
     * @param vec3s List of x,y,z tuples
     */
    public ConstMFVec3f(int size, float vec3s[]) {
        super(MFVec3f.createPeer(size, vec3s));
    }

    public native int getSize();

    /**
     * Retrieves the value of an MFVec3f field.
     *
     * @param vec3s 2D array of x,y,z tuples to be returned.
     */
    public native void getValue(float vec3s[][]);

    /**
     * Retrieves the value of an MFVec3f field.
     *
     * @param vec3s Array of x,y,z tuples to be returned.
     */
    public native void getValue(float vec3s[]);

    /**
     * Retrieves a specific element in an MFVec3f and
     * returns it as a float array.
     *
     * @param index Position of desired element
     * @param vec3s Element at specified position
     */
    public native void get1Value(int index, float vec3s[]);

    /**
     * Retrieves a specific element in an MFVec3f and
     * returns it as an SFVec3f.
     *
     * @param index Position of desired element
     * @param vec Element at specified position
     */
    public void get1Value(int index, SFVec3f vec) {
        float[] v = { 0.0f, 0.0f, 0.0f };
        this.get1Value(index, v);
        vec.setValue(v);
    }
}
