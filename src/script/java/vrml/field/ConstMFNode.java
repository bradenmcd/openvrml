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

import java.util.ArrayList;
import java.util.Arrays;
import vrml.ConstMField;
import vrml.BaseNode;

/**
 * Represents a read-only VRML MFNode field in Java.
 */
public class ConstMFNode extends ConstMField {

    /**
     * We maintain a parallel array of nodes on the Java side so that
     * getting a BaseNode reference from the MFNode yields the same
     * instance as was added to the MFNode.
     */
    final ArrayList<BaseNode> nodes;

    /**
     * Construct from a peer handle.
     */
    ConstMFNode(long peer) {
        super(peer);
        this.nodes =
            new ArrayList<BaseNode>(Arrays.asList(MFNode.initNodes(peer)));
    }

    /**
     * Construct a read-only MFNode field.
     *
     * @param size Number of elements passed in.
     * @param nodes Array of BaseNodes.
     */
    public ConstMFNode(int size, BaseNode nodes[]) {
        super(MFNode.createPeer(size, nodes));
        this.nodes = new ArrayList<BaseNode>(size);
        for (int i = 0; i < size; i++) { this.nodes.add(nodes[i]); }
    }

    /**
     * Construct a read-only MFNode field.
     *
     * @param nodes Array of BaseNodes.
     */
    public ConstMFNode(BaseNode nodes[]) {
        this(nodes.length, nodes);
    }

    public int getSize() {
        return this.nodes.size();
    }

    /**
     * Retrieves the value of an MFNode field.
     *
     * @param nodes Array of BaseNodes to be returned.
     */
    public void getValue(BaseNode nodes[]) {
        int i = 0;
        for (BaseNode n : this.nodes) { nodes[i++] = n; }
    }

    /**
     * Retrieves a specific BaseNode from an MFNode.
     *
     * @param index Position of desired BaseNode.
     * @return Value of BaseNode at index.
     */
    public BaseNode get1Value(int index) {
        return this.nodes.get(index);
    }
}
