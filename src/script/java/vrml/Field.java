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

package vrml;

/**
 * Parent class of all VRML fields.
 */
public abstract class Field implements Cloneable {
    private long peer;

    public Field(long peer) {
        this.peer = peer;
    }

    protected long getPeer() {
        return this.peer;
    }

    /**
     * Creates and returns a copy of the Field.
     *
     * @return a clone of this instance.
     */
    public native Object clone() throws CloneNotSupportedException;

    private static native void dispose(long peer);

    protected void dispose() {
        if (this.peer != 0) {
            Field.dispose(this.peer);
            this.peer = 0;
        }
    }

    /**
     * Finalize.
     */
    protected void finalize() throws Throwable {
        try {
            //
            // Delegating to dispose() in this fashion saves us some
            // legwork on the JNI side.
            //
            this.dispose();
        } finally {
            super.finalize();
        }
    }

    /**
     * Returns a string representation of the Field.
     *
     * @return a string representation of the Field.
     */
    public native String toString();
}
