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

package vrml.node;

import java.util.HashMap;
import java.util.Map;
import vrml.Event;
import vrml.Field;
import vrml.BaseNode;
import vrml.InvalidFieldException;
import vrml.InvalidEventOutException;
import vrml.InvalidEventInException;

/**
 * This is the general Script class, to be subclassed by all scripts.
 * Note that the provided methods allow the script author to explicitly
 * throw tailored exceptions in case something goes wrong in the
 * script.
 */
public abstract class Script extends BaseNode {

    static {
        System.loadLibrary("openvrml");
    }

    private long scriptPeer;

    private final HashMap<String, Field> fields = new HashMap<String, Field>();
    private final HashMap<String, Field> eventIns =
        new HashMap<String, Field>();
    private final HashMap<String, NodeField> eventOuts =
        new HashMap<String, NodeField>();

    /**
     * Destroy the native peer instance.
     *
     * Normally this method is called from {@link Script#finalize()}.
     * If it is called before finalization, only {@link Script#finalize()}
     * can safely be called on the instance subsequently.
     */
    protected void dispose() {
        //
        // scriptPeer isn't an owning reference; we can just set it to 0.
        //
        if (this.scriptPeer != 0) {
            this.scriptPeer = 0;
        }
        super.dispose();
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

    private native void updateField(String id, Field value);

    private void updateFields() {
        for (Map.Entry<String, Field> field: this.fields.entrySet()) {
            this.updateField(field.getKey(), field.getValue());
        }
    }

    private native void emitEvent(String id, Field value);

    private void emitEvents() {
        for (Map.Entry<String, NodeField> field: this.eventOuts.entrySet()) {
            if (field.getValue().commitPendingChange()) {
                this.emitEvent(field.getKey(), (Field) field.getValue());
            }
        }
    }

    /**
     * This method is called before any event is generated.
     */
    public void initialize() {}

    /**
     * Get a Field by name.
     *
     * @param fieldName Name of field to retrieve.
     * @return Field with given name.
     * @throws InvalidFieldException Invalid field name specified.
     */
    protected final Field getField(final String fieldName)
            throws InvalidFieldException {
        final Field field = this.fields.get(fieldName);
        if (field == null) {
            throw new InvalidFieldException("no field \"" + fieldName
                                            + "\" declared for Script node");
        }
        return field;
    }

    /**
     * Get an EventOut by name.
     *
     * @param eventOutName Name of eventOut to retrieve.
     * @return Field representing eventOut with given name.
     * @throws InvalidEventOutException Invalid eventOut name specified.
     */
    protected final Field getEventOut(final String eventOutName)
            throws InvalidEventOutException {
        final NodeField eventOut = this.eventOuts.get(eventOutName);
        if (eventOut == null) {
            throw new InvalidEventOutException(
                "no eventOut \"" + eventOutName + "\" declared for Script "
                + "node");
        }
        return (Field) eventOut;
    }

    /**
     * Get an EventIn by name.
     *
     * @param eventInName Name of eventIn to retrieve.
     * @return Field representing eventIn with given name.
     * @throws InvalidEventInException Invalid eventIn name specified.
     */
    protected final Field getEventIn(final String eventInName)
            throws InvalidEventInException {
        final Field eventIn = this.eventIns.get(eventInName);
        if (eventIn == null) {
            throw new InvalidEventInException(
                "no eventIn \"" + eventInName + "\" declared for Script "
                + "node");
        }
        return eventIn;
    }

    /**
     * Called automatically when the script receives some set of events.
     * It should not be called directly except by its subclass.
     *
     * @param count Number of events delivered.
     * @param events Array of events to process.
     */
    public void processEvents(int count, Event events[]) {
        for (Event e: events) { this.processEvent(e); }
    }

    /**
     * Called automatically when the script receives an event.
     */
    public void processEvent(Event event) {}

    /**
     * Called after every invocation of processEvents().
     */
    public void eventsProcessed() {}

    /**
     * Called when the Script node is deleted.
     */
    public void shutdown() {}
}

class OutStream extends java.io.OutputStream {
    // private final ArrayList<byte> buf = new ArrayList<byte>();

    public void write(int b) {
    }
}
