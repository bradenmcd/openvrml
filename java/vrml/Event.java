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
 * Represents a VRML event.
 */
public class Event implements Cloneable {
    /**
     * Stores a pointer to a VrmlEvent object.
     */
    private int EventPtr;

    /**
     * Returns the name of the event.
     *
     * @return Name of the event.
     */
    public native String getName();

    /**
     * Returns the timestamp of the event.
     *
     * @return Timestamp of the event.
     */
    public native double getTimeStamp();

    /**
     * Returns the Field the event has been generated for.
     *
     * @return Field the event has been generated for.
     */
    public native ConstField getValue();

    public native Object clone();

    public native String toString();
}
