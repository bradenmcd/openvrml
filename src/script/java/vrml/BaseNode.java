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

package vrml;

/**
 * Base class for all VRML nodes.
 */
public abstract class BaseNode {

    /**
     * A Java <code>long</code> representation of a pointer to a
     * native peer instance.
     */
    private long peer;

    /**
     * The Browser associated with the node instance.
     */
    private Browser browser;

    public BaseNode() {}

    public BaseNode(long peer) {
        this.peer = peer;
    }

    /**
     * Destroy a native peer instance.
     *
     * @param peer  a Java long representation of a pointer to a native
     *              peer instance.
     */
    private static native void dispose(long peer);

    /**
     * Destroy the native peer instance.
     *
     * Normally this method is called from {@link BaseNode#finalize()}.
     * If it is called before finalization, only {@link BaseNode#finalize()}
     * can safely be called on the instance subsequently.
     */
    protected void dispose() {
        if (this.peer != 0) {
            BaseNode.dispose(this.peer);
            this.peer = 0;
        }
    }

    /**
     * Finalize.
     */
    protected void finalize() throws Throwable {
        try {
            this.dispose();
        } finally {
            super.finalize();
        }
    }

    /**
     * Returns the type of the node.  If the node is a prototype
     * it returns the name of the prototype.
     *
     * @return Type of the node.
     */
    public native String getType();

    /**
     * Get the Browser associated with this node.
     *
     * @return the Browser associated with this node.
     */
    public Browser getBrowser() {
        return this.browser;
    }

    /**
     * Returns a string representation of the node.
     *
     * @return a string representation of the node.
     */
    public native String toString();
}
