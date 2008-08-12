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

    /**
     * Is this a script node?
     */
    private boolean isScript = true;

    /**
     * This method is called before any event is generated.
     */
    public void initialize() { }

    /**
     * Get a Field by name.
     *
     * @param fieldName Name of field to retrieve.
     * @return Field with given name.
     * @throws InvalidFieldException Invalid field name specified.
     */
    protected native final Field getField(String fieldName)
        throws InvalidFieldException;

    /**
     * Get an EventOut by name.
     *
     * @param eventOutName Name of eventOut to retrieve.
     * @return Field representing eventOut with given name.
     * @throws InvalidEventOutException Invalid eventOut name specified.
     */
    protected native final Field getEventOut(String eventOutName)
        throws InvalidEventOutException;

    /**
     * Get an EventIn by name.
     *
     * @param eventInName Name of eventIn to retrieve.
     * @return Field representing eventIn with given name.
     * @throws InvalidEventInException Invalid eventIn name specified.
     */
    protected native final Field getEventIn(String eventInName)
        throws InvalidEventInException;

    /**
     * Called automatically when the script receives some set of events.
     * It should not be called directly except by its subclass.
     *
     * @param count Number of events delivered.
     * @param events Array of events to process.
     */
    public void processEvents(int count, Event events[]) { }

    /**
     * Called automatically when the script receives an event.
     */
    public void processEvent(Event event) { }

    /**
     * Called after every invocation of processEvents().
     */
    public void eventsProcessed() { }

    /**
     * Called when the Script node is deleted.
     */
    public void shutdown() { }

    public native String toString();
}
