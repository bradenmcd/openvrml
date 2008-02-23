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

package vrml.node;

import java.util.HashMap;
import vrml.BaseNode;
import vrml.Field;
import vrml.ConstField;
import vrml.InvalidEventInException;
import vrml.InvalidEventOutException;
import vrml.InvalidExposedFieldException;

/**
 * The general node class.
 */
public abstract class Node extends BaseNode {

    private final HashMap<String, Field> eventIns =
        new HashMap<String, Field>();
    private final HashMap<String, ConstField> eventOuts =
        new HashMap<String, ConstField>();
    private final HashMap<String, Field> exposedFields =
        new HashMap<String, Field>();

    public Node(long peer) {
        super(peer);
    }

    /**
     * Get an EventIn by name. Return value is write-only.
     *
     * @param eventInName Name of eventIn to retrieve.
     * @return Write-only eventIn field.
     * @throws InvalidEventInException Invalid eventIn name specified.
     */
    public final Field getEventIn(final String eventInName)
            throws InvalidEventInException {
        final Field eventIn = this.eventIns.get(eventInName);
        if (eventIn == null) {
            throw new InvalidEventInException(
                "no eventIn \"" + eventInName + "\" declared for "
                + this.getType() + " node");
        }
        return eventIn;
    }

    /**
     * Get an EventOut by name. Return value is read-only.
     *
     * @param eventOutName Name of eventOut to retrieve.
     * @return Read-only eventOut field.
     * @throws InvalidEventOutException Invalid eventOut name specified.
     */
    public final ConstField getEventOut(final String eventOutName)
            throws InvalidEventOutException {
        final ConstField eventOut = this.eventOuts.get(eventOutName);
        if (eventOut == null) {
            throw new InvalidEventOutException(
                "no eventOut \"" + eventOutName + "\" declared for "
                + this.getType() + " node");
        }
        return eventOut;
    }

    /**
     * Get an exposed field by name.
     *
     * @param exposedFieldName Name of exposedField to retrieve.
     * @return Exposed field specified by name.
     * @throws InvalidExposedFieldException if exposedField name is invalid.
     */
    public final Field getExposedField(final String exposedFieldName)
            throws InvalidExposedFieldException {
        final Field exposedField = this.exposedFields.get(exposedFieldName);
        if (exposedField == null) {
            throw new InvalidExposedFieldException(
                "no exposedField \"" + exposedFieldName + "\" declared for "
                + this.getType() + " node");
        }
        return exposedField;
    }

    public native String toString();
}

class NodeImpl extends Node {
    public NodeImpl(long peer) {
        super(peer);
        assert peer != 0;
    }
}

interface NodeField {
    boolean commitPendingChange();
}

class NodeSFBool extends vrml.field.SFBool implements NodeField {
    private vrml.field.SFBool pendingChange;

    public void setValue(final boolean b) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.SFBool(b);
        } else {
            this.pendingChange.setValue(b);
        }
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            super.setValue(this.pendingChange.getValue());
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeSFColor extends vrml.field.SFColor implements NodeField {
    private vrml.field.SFColor pendingChange;

    public void setValue(final float r, final float g, final float b) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.SFColor(r, g, b);
        } else {
            this.pendingChange.setValue(r, g, b);
        }
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            final float[] c = { 0.0f, 0.0f, 0.0f };
            this.pendingChange.getValue(c);
            super.setValue(c[0], c[1], c[2]);
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeSFDouble extends vrml.field.SFDouble implements NodeField {
    private vrml.field.SFDouble pendingChange;

    public void setValue(final double d) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.SFDouble(d);
        } else {
            this.pendingChange.setValue(d);
        }
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            super.setValue(this.pendingChange.getValue());
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeSFFloat extends vrml.field.SFFloat implements NodeField {
    private vrml.field.SFFloat pendingChange;

    public void setValue(final float f) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.SFFloat(f);
        } else {
            this.pendingChange.setValue(f);
        }
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            super.setValue(this.pendingChange.getValue());
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeSFImage extends vrml.field.SFImage implements NodeField {
    private vrml.field.SFImage pendingChange;

    public void setValue(final int w, final int h, final int c,
                         final byte[] pixels) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.SFImage(w, h, c, pixels);
        } else {
            this.pendingChange.setValue(w, h, c, pixels);
        }
    }

    //
    // This is annoying.  We only have to override these two setValue
    // methods because the superclass implementations of them don't
    // simply delegate to setValue(int, int, int, byte[]).  That
    // should be corrected; at which point the following overrides can
    // go away.
    //

    public void setValue(final vrml.field.ConstSFImage image) {
        final int w = image.getWidth();
        final int h = image.getHeight();
        final int c = image.getComponents();
        final byte[] pixels = new byte[w * h * c];
        image.getPixels(pixels);
        this.setValue(w, h, c, pixels);
    }

    public void setValue(final vrml.field.SFImage image) {
        final int w = image.getWidth();
        final int h = image.getHeight();
        final int c = image.getComponents();
        final byte[] pixels = new byte[w * h * c];
        image.getPixels(pixels);
        this.setValue(w, h, c, pixels);
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            final int w = this.pendingChange.getWidth();
            final int h = this.pendingChange.getHeight();
            final int c = this.pendingChange.getComponents();
            final byte[] pixels = new byte[w * h * c];
            this.pendingChange.getPixels(pixels);
            super.setValue(w, h, c, pixels);
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeSFInt32 extends vrml.field.SFInt32 implements NodeField {
    private vrml.field.SFInt32 pendingChange;

    public void setValue(final int i) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.SFInt32(i);
        } else {
            this.pendingChange.setValue(i);
        }
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            super.setValue(this.pendingChange.getValue());
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeSFNode extends vrml.field.SFNode implements NodeField {
    private vrml.field.SFNode pendingChange;

    public void setValue(final vrml.BaseNode node) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.SFNode(node);
        } else {
            this.pendingChange.setValue(node);
        }
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            super.setValue(this.pendingChange.getValue());
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeSFRotation extends vrml.field.SFRotation implements NodeField {
    private vrml.field.SFRotation pendingChange;

    public void setValue(final float x, final float y, final float z,
                         final float angle) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.SFRotation(x, y, z, angle);
        } else {
            this.pendingChange.setValue(x, y, z, angle);
        }
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            final float[] r = { 0.0f, 0.0f, 0.0f, 0.0f };
            this.pendingChange.getValue(r);
            super.setValue(r[0], r[1], r[2], r[3]);
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeSFString extends vrml.field.SFString implements NodeField {
    private vrml.field.SFString pendingChange;

    public void setValue(final String s) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.SFString(s);
        } else {
            this.pendingChange.setValue(s);
        }
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            super.setValue(this.pendingChange.getValue());
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeSFTime extends vrml.field.SFTime implements NodeField {
    private vrml.field.SFTime pendingChange;

    public void setValue(final double time) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.SFTime(time);
        } else {
            this.pendingChange.setValue(time);
        }
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            super.setValue(this.pendingChange.getValue());
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeSFVec2d extends vrml.field.SFVec2d implements NodeField {
    private vrml.field.SFVec2d pendingChange;

    public void setValue(final double x, final double y) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.SFVec2d(x, y);
        } else {
            this.pendingChange.setValue(x, y);
        }
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            final double[] v = { 0.0, 0.0 };
            this.pendingChange.getValue(v);
            super.setValue(v);
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeSFVec2f extends vrml.field.SFVec2f implements NodeField {
    private vrml.field.SFVec2f pendingChange;

    public void setValue(final float x, final float y) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.SFVec2f(x, y);
        } else {
            this.pendingChange.setValue(x, y);
        }
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            final float[] v = { 0.0f, 0.0f };
            this.pendingChange.getValue(v);
            super.setValue(v);
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeSFVec3d extends vrml.field.SFVec3d implements NodeField {
    private vrml.field.SFVec3d pendingChange;

    public void setValue(final double x, final double y, final double z) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.SFVec3d(x, y, z);
        } else {
            this.pendingChange.setValue(x, y, z);
        }
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            final double[] v = { 0.0, 0.0, 0.0 };
            this.pendingChange.getValue(v);
            super.setValue(v);
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeSFVec3f extends vrml.field.SFVec3f implements NodeField {
    private vrml.field.SFVec3f pendingChange;

    public void setValue(final float x, final float y, final float z) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.SFVec3f(x, y, z);
        } else {
            this.pendingChange.setValue(x, y, z);
        }
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            final float[] v = { 0.0f, 0.0f, 0.0f };
            this.pendingChange.getValue(v);
            super.setValue(v);
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeMFBool extends vrml.field.MFBool implements NodeField {
    private vrml.field.MFBool pendingChange;

    public void clear() {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFBool();
        } else {
            this.pendingChange.clear();
        }
    }

    public void delete(final int index) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFBool) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.delete(index);
    }

    public void setValue(final int size, final boolean[] values) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFBool(size, values);
        } else {
            this.pendingChange.setValue(size, values);
        }
    }

    public void setValue(final vrml.field.MFBool value) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFBool) value.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void setValue(final vrml.field.ConstMFBool value) {
        if (this.pendingChange == null) {
            final int size = value.getSize();
            final boolean[] values = new boolean[size];
            value.getValue(values);
            this.pendingChange = new vrml.field.MFBool(values);
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void set1Value(final int index, final boolean b) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFBool) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.set1Value(index, b);
    }

    public void addValue(final boolean b) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFBool) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.addValue(b);
    }

    public void insertValue(final int index, final boolean b) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFBool) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.insertValue(index, b);
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            super.setValue(this.pendingChange);
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeMFColor extends vrml.field.MFColor implements NodeField {
    private vrml.field.MFColor pendingChange;

    public void clear() {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFColor();
        } else {
            this.pendingChange.clear();
        }
    }

    public void delete(final int index) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFColor) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.delete(index);
    }

    public void setValue(final int size, final float[] values) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFColor(size, values);
        } else {
            this.pendingChange.setValue(size, values);
        }
    }

    public void setValue(final vrml.field.MFColor value) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFColor) value.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void setValue(final vrml.field.ConstMFColor value) {
        if (this.pendingChange == null) {
            final int size = value.getSize();
            final float[] values = new float[size * 3];
            value.getValue(values);
            this.pendingChange = new vrml.field.MFColor(values);
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void set1Value(final int index,
                          final float r, final float g, final float b) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFColor) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.set1Value(index, r, g, b);
    }

    public void addValue(final float r, final float g, final float b) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFColor) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.addValue(r, g, b);
    }

    public void insertValue(final int index,
                            final float r, final float g, final float b) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFColor) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.insertValue(index, r, g, b);
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            super.setValue(this.pendingChange);
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeMFDouble extends vrml.field.MFDouble implements NodeField {
    private vrml.field.MFDouble pendingChange;

    public void clear() {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFDouble();
        } else {
            this.pendingChange.clear();
        }
    }

    public void delete(final int index) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFDouble) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.delete(index);
    }

    public void setValue(final int size, final double[] values) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFDouble(size, values);
        } else {
            this.pendingChange.setValue(size, values);
        }
    }

    public void setValue(final vrml.field.MFDouble value) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFDouble) value.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void setValue(final vrml.field.ConstMFDouble value) {
        if (this.pendingChange == null) {
            final int size = value.getSize();
            final double[] values = new double[size];
            value.getValue(values);
            this.pendingChange = new vrml.field.MFDouble(values);
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void set1Value(final int index, final double d) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFDouble) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.set1Value(index, d);
    }

    public void addValue(final double d) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFDouble) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.addValue(d);
    }

    public void insertValue(final int index, final double d) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFDouble) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.insertValue(index, d);
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            super.setValue(this.pendingChange);
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeMFFloat extends vrml.field.MFFloat implements NodeField {
    private vrml.field.MFFloat pendingChange;

    public void clear() {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFFloat();
        } else {
            this.pendingChange.clear();
        }
    }

    public void delete(final int index) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFFloat) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.delete(index);
    }

    public void setValue(final int size, final float[] values) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFFloat(size, values);
        } else {
            this.pendingChange.setValue(size, values);
        }
    }

    public void setValue(final vrml.field.MFFloat value) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFFloat) value.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void setValue(final vrml.field.ConstMFFloat value) {
        if (this.pendingChange == null) {
            final int size = value.getSize();
            final float[] values = new float[size];
            value.getValue(values);
            this.pendingChange = new vrml.field.MFFloat(values);
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void set1Value(final int index, final float f) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFFloat) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.set1Value(index, f);
    }

    public void addValue(final float f) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFFloat) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.addValue(f);
    }

    public void insertValue(final int index, final float f) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFFloat) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.insertValue(index, f);
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            super.setValue(this.pendingChange);
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeMFInt32 extends vrml.field.MFInt32 implements NodeField {
    private vrml.field.MFInt32 pendingChange;

    public void clear() {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFInt32();
        } else {
            this.pendingChange.clear();
        }
    }

    public void delete(final int index) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFInt32) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.delete(index);
    }

    public void setValue(final int size, final int[] values) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFInt32(size, values);
        } else {
            this.pendingChange.setValue(size, values);
        }
    }

    public void setValue(final vrml.field.MFInt32 value) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFInt32) value.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void setValue(final vrml.field.ConstMFInt32 value) {
        if (this.pendingChange == null) {
            final int size = value.getSize();
            final int[] values = new int[size];
            value.getValue(values);
            this.pendingChange = new vrml.field.MFInt32(values);
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void set1Value(final int index, final int i) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFInt32) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.set1Value(index, i);
    }

    public void addValue(final int i) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFInt32) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.addValue(i);
    }

    public void insertValue(final int index, final int i) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFInt32) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.insertValue(index, i);
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            super.setValue(this.pendingChange);
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeMFNode extends vrml.field.MFNode implements NodeField {
    private vrml.field.MFNode pendingChange;

    public void clear() {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFNode();
        } else {
            this.pendingChange.clear();
        }
    }

    public void delete(final int index) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFNode) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.delete(index);
    }

    public void setValue(final int size, final BaseNode[] values) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFNode(size, values);
        } else {
            this.pendingChange.setValue(size, values);
        }
    }

    public void setValue(final vrml.field.MFNode value) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFNode) value.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void setValue(final vrml.field.ConstMFNode value) {
        if (this.pendingChange == null) {
            final int size = value.getSize();
            final BaseNode[] values = new BaseNode[size];
            value.getValue(values);
            this.pendingChange = new vrml.field.MFNode(values);
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void set1Value(final int index, final BaseNode node) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFNode) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.set1Value(index, node);
    }

    public void addValue(final BaseNode node) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFNode) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.addValue(node);
    }

    public void insertValue(final int index, final BaseNode node) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFNode) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.insertValue(index, node);
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            super.setValue(this.pendingChange);
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeMFRotation extends vrml.field.MFRotation implements NodeField {
    private vrml.field.MFRotation pendingChange;

    public void clear() {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFRotation();
        } else {
            this.pendingChange.clear();
        }
    }

    public void delete(final int index) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFRotation) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.delete(index);
    }

    public void setValue(final int size, final float[] values) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFRotation(size, values);
        } else {
            this.pendingChange.setValue(size, values);
        }
    }

    public void setValue(final vrml.field.MFRotation value) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFRotation) value.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void setValue(final vrml.field.ConstMFRotation value) {
        if (this.pendingChange == null) {
            final int size = value.getSize();
            final float[] values = new float[size * 4];
            value.getValue(values);
            this.pendingChange = new vrml.field.MFRotation(values);
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void set1Value(final int index,
                          final float x, final float y, final float z,
                          final float angle) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFRotation) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.set1Value(index, x, y, z, angle);
    }

    public void addValue(final float x, final float y, final float z,
                         final float angle) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFRotation) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.addValue(x, y, z, angle);
    }

    public void insertValue(final int index,
                            final float x, final float y, final float z,
                            final float angle) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFRotation) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.insertValue(index, x, y, z, angle);
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            super.setValue(this.pendingChange);
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeMFString extends vrml.field.MFString implements NodeField {
    private vrml.field.MFString pendingChange;

    public void clear() {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFString();
        } else {
            this.pendingChange.clear();
        }
    }

    public void delete(final int index) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFString) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.delete(index);
    }

    public void setValue(final int size, final String[] values) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFString(size, values);
        } else {
            this.pendingChange.setValue(size, values);
        }
    }

    public void setValue(final vrml.field.MFString value) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFString) value.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void setValue(final vrml.field.ConstMFString value) {
        if (this.pendingChange == null) {
            final int size = value.getSize();
            final String[] values = new String[size];
            value.getValue(values);
            this.pendingChange = new vrml.field.MFString(values);
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void set1Value(final int index, final String s) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFString) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.set1Value(index, s);
    }

    public void addValue(final String s) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFString) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.addValue(s);
    }

    public void insertValue(final int index, final String s) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFString) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.insertValue(index, s);
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            super.setValue(this.pendingChange);
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeMFTime extends vrml.field.MFTime implements NodeField {
    private vrml.field.MFTime pendingChange;

    public void clear() {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFTime();
        } else {
            this.pendingChange.clear();
        }
    }

    public void delete(final int index) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFTime) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.delete(index);
    }

    public void setValue(final int size, final double[] values) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFTime(size, values);
        } else {
            this.pendingChange.setValue(size, values);
        }
    }

    public void setValue(final vrml.field.MFTime value) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFTime) value.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void setValue(final vrml.field.ConstMFTime value) {
        if (this.pendingChange == null) {
            final int size = value.getSize();
            final double[] values = new double[size];
            value.getValue(values);
            this.pendingChange = new vrml.field.MFTime(values);
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void set1Value(final int index, final double d) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFTime) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.set1Value(index, d);
    }

    public void addValue(final double d) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFTime) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.addValue(d);
    }

    public void insertValue(final int index, final double d) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFTime) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.insertValue(index, d);
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            super.setValue(this.pendingChange);
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeMFVec2d extends vrml.field.MFVec2d implements NodeField {
    private vrml.field.MFVec2d pendingChange;

    public void clear() {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFVec2d();
        } else {
            this.pendingChange.clear();
        }
    }

    public void delete(final int index) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFVec2d) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.delete(index);
    }

    public void setValue(final int size, final double[] values) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFVec2d(size, values);
        } else {
            this.pendingChange.setValue(size, values);
        }
    }

    public void setValue(final vrml.field.MFVec2d value) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFVec2d) value.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void setValue(final vrml.field.ConstMFVec2d value) {
        if (this.pendingChange == null) {
            final int size = value.getSize();
            final double[] values = new double[size * 2];
            value.getValue(values);
            this.pendingChange = new vrml.field.MFVec2d(values);
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void set1Value(final int index, final double x, final double y) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFVec2d) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.set1Value(index, x, y);
    }

    public void addValue(final double x, final double y) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFVec2d) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.addValue(x, y);
    }

    public void insertValue(final int index, final double x, final double y) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFVec2d) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.insertValue(index, x, y);
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            super.setValue(this.pendingChange);
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeMFVec2f extends vrml.field.MFVec2f implements NodeField {
    private vrml.field.MFVec2f pendingChange;

    public void clear() {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFVec2f();
        } else {
            this.pendingChange.clear();
        }
    }

    public void delete(final int index) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFVec2f) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.delete(index);
    }

    public void setValue(final int size, final float[] values) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFVec2f(size, values);
        } else {
            this.pendingChange.setValue(size, values);
        }
    }

    public void setValue(final vrml.field.MFVec2f value) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFVec2f) value.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void setValue(final vrml.field.ConstMFVec2f value) {
        if (this.pendingChange == null) {
            final int size = value.getSize();
            final float[] values = new float[size * 2];
            value.getValue(values);
            this.pendingChange = new vrml.field.MFVec2f(values);
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void set1Value(final int index, final float x, final float y) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFVec2f) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.set1Value(index, x, y);
    }

    public void addValue(final float x, final float y) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFVec2f) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.addValue(x, y);
    }

    public void insertValue(final int index, final float x, final float y) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFVec2f) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.insertValue(index, x, y);
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            super.setValue(this.pendingChange);
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeMFVec3d extends vrml.field.MFVec3d implements NodeField {
    private vrml.field.MFVec3d pendingChange;

    public void clear() {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFVec3d();
        } else {
            this.pendingChange.clear();
        }
    }

    public void delete(final int index) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFVec3d) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.delete(index);
    }

    public void setValue(final int size, final double[] values) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFVec3d(size, values);
        } else {
            this.pendingChange.setValue(size, values);
        }
    }

    public void setValue(final vrml.field.MFVec3d value) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFVec3d) value.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void setValue(final vrml.field.ConstMFVec3d value) {
        if (this.pendingChange == null) {
            final int size = value.getSize();
            final double[] values = new double[size * 3];
            value.getValue(values);
            this.pendingChange = new vrml.field.MFVec3d(values);
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void set1Value(final int index,
                          final double x, final double y, final double z) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFVec3d) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.set1Value(index, x, y, z);
    }

    public void addValue(final double x, final double y, final double z) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFVec3d) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.addValue(x, y, z);
    }

    public void insertValue(final int index,
                            final double x, final double y, final double z) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFVec3d) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.insertValue(index, x, y, z);
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            super.setValue(this.pendingChange);
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}

class NodeMFVec3f extends vrml.field.MFVec3f implements NodeField {
    private vrml.field.MFVec3f pendingChange;

    public void clear() {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFVec3f();
        } else {
            this.pendingChange.clear();
        }
    }

    public void delete(final int index) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFVec3f) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.delete(index);
    }

    public void setValue(final int size, final float[] values) {
        if (this.pendingChange == null) {
            this.pendingChange = new vrml.field.MFVec3f(size, values);
        } else {
            this.pendingChange.setValue(size, values);
        }
    }

    public void setValue(final vrml.field.MFVec3f value) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFVec3f) value.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void setValue(final vrml.field.ConstMFVec3f value) {
        if (this.pendingChange == null) {
            final int size = value.getSize();
            final float[] values = new float[size * 3];
            value.getValue(values);
            this.pendingChange = new vrml.field.MFVec3f(values);
        } else {
            this.pendingChange.setValue(value);
        }
    }

    public void set1Value(final int index,
                          final float x, final float y, final float z) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFVec3f) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.set1Value(index, x, y, z);
    }

    public void addValue(final float x, final float y, final float z) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFVec3f) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.addValue(x, y, z);
    }

    public void insertValue(final int index,
                            final float x, final float y, final float z) {
        if (this.pendingChange == null) {
            try {
                this.pendingChange = (vrml.field.MFVec3f) super.clone();
            } catch (CloneNotSupportedException ex) {
                assert false;
            }
        }
        this.pendingChange.insertValue(index, x, y, z);
    }

    public boolean commitPendingChange() {
        if (this.pendingChange != null) {
            super.setValue(this.pendingChange);
            this.pendingChange = null;
            return true;
        }
        return false;
    }
}
