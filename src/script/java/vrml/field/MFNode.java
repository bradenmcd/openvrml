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

import java.util.ArrayList;
import java.util.Arrays;
import vrml.MField;
import vrml.BaseNode;

/**
 * Represents a VRML MFNode field in Java.
 */
public class MFNode extends MField {

    /**
     * We maintain a parallel array of nodes on the Java side so that
     * getting a BaseNode reference from the MFNode yields the same
     * instance as was added to the MFNode.
     */
    ArrayList<BaseNode> nodes;

    /**
     * Construct a new MFNode field in OpenVRML using the given params.
     *
     * @param size Number of elements passed in.
     * @param nodes List of BaseNodes.
     */
    static native long createPeer(int size, BaseNode nodes[]);

    /**
     * Default constructor.
     */
    public MFNode() {
        this(0, null);
    }

    /**
     * Initialize MFNode.nodes to correspond to the nodes in the peer.
     */
    static native BaseNode[] initNodes(long peer);

    /**
     * Construct from a peer handle.
     */
    MFNode(long peer) {
        super(peer);
        this.nodes =
            new ArrayList<BaseNode>(Arrays.asList(MFNode.initNodes(peer)));
    }

    /**
     * Construct an MFNode field.
     *
     * @param size Number of elements passed in.
     * @param nodes Array of BaseNodes.
     */
    public MFNode(int size, BaseNode nodes[]) {
        super(createPeer(size, nodes));
        this.nodes = new ArrayList<BaseNode>(size);
        for (int i = 0; i < size; i++) { this.nodes.add(nodes[i]); }
    }

    /**
     * Construct an MFNode field.
     *
     * @param nodes Array of BaseNodes.
     */
    public MFNode(BaseNode nodes[]) {
        this(nodes.length, nodes);
    }

    /**
     * Create a shallow copy of the MFNode; the underlying BaseNodes are
     * not copied.
     *
     * @return a shallow copy of the MFNode.
     */
    public Object clone() throws CloneNotSupportedException {
        final MFNode mfn = (MFNode) super.clone();
        mfn.nodes = new ArrayList<BaseNode>(this.nodes);
        return mfn;
    }

    public int getSize() {
        return this.nodes.size();
    }

    private static native void peer_clear(long peer);

    public void clear() {
        MFNode.peer_clear(this.getPeer());
        this.nodes.clear();
    }

    private static native void peer_delete(long peer, int index);

    public void delete(int index) {
        MFNode.peer_delete(this.getPeer(), index);
        this.nodes.remove(index);
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

    /**
     * Set the value of the field.
     *
     * @param node New value for field.
     */
    public void setValue(BaseNode node[]) {
        this.setValue(node.length, node);
    }

    private static native void peer_setValue(long peer,
                                             int size, BaseNode nodes[]);

    /**
     * Set the value of the field.
     *
     * @param size Size of new value for field.
     * @param nodes New value for field.
     */
    public void setValue(int size, BaseNode nodes[]) {
        MFNode.peer_setValue(this.getPeer(), size, nodes);
        this.nodes.clear();
        this.nodes.ensureCapacity(size);
        for (int i = 0; i < size; i++) { this.nodes.add(nodes[i]); }
    }

    private static native void peer_setValue(long peer, MFNode nodes);

    /**
     * Set the value of the field.
     *
     * @param nodes New value for field.
     */
    public void setValue(MFNode nodes) {
        MFNode.peer_setValue(this.getPeer(), nodes);
        this.nodes.clear();
        this.nodes.addAll(nodes.nodes);
    }

    private static native void peer_setValue(long peer, ConstMFNode nodes);

    /**
     * Set the value of the field.
     *
     * @param node New value for field.
     */
    public void setValue(ConstMFNode nodes) {
        MFNode.peer_setValue(this.getPeer(), nodes);
        this.nodes.clear();
        this.nodes.addAll(nodes.nodes);
    }

    private static native void peer_set1Value(long peer,
                                              int index, BaseNode node);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param node New value for element.
     */
    public void set1Value(int index, BaseNode node) {
        MFNode.peer_set1Value(this.getPeer(), index, node);
        this.nodes.set(index, node);
    }

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param node New value for element.
     */
    public void set1Value(int index, ConstSFNode node) {
        this.set1Value(index, node.getValue());
    }

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param node New value for element.
     */
    public void set1Value(int index, SFNode node) {
        this.set1Value(index, node.getValue());
    }

    private static native void peer_addValue(long peer, BaseNode node);

    /**
     * Add a new element at the end of the list.
     *
     * @param node Element to add.
     */
    public void addValue(BaseNode node) {
        MFNode.peer_addValue(this.getPeer(), node);
        this.nodes.add(node);
    }

    /**
     * Add a new element at the end of the list.
     *
     * @param node Element to add.
     */
    public void addValue(ConstSFNode node) {
        this.addValue(node.getValue());
    }

    /**
     * Add a new element at the end of the list.
     *
     * @param node Element to add.
     */
    public void addValue(SFNode node) {
        this.addValue(node.getValue());
    }

    private static native void peer_insertValue(long peer,
                                                int index, BaseNode node);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param node Value to insert.
     */
    public void insertValue(int index, BaseNode node) {
        MFNode.peer_insertValue(this.getPeer(), index, node);
        this.nodes.add(index, node);
    }

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param node Value to insert.
     */
    public void insertValue(int index, ConstSFNode node) {
        this.insertValue(index, node.getValue());
    }

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param node Value to insert.
     */
    public void insertValue(int index, SFNode node) {
        this.insertValue(index, node.getValue());
    }
}
