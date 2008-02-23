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
 * Represents a VRML event.
 */
public class Event implements Cloneable {
    private String name;
    private double timeStamp;
    private ConstField value;

    Event(final String name, final double timeStamp, final ConstField value) {
        this.name = name;
        this.timeStamp = timeStamp;
        this.value = value;
    }

    /**
     * Returns the name of the event.
     *
     * @return Name of the event.
     */
    public String getName() {
        return this.name;
    }

    /**
     * Returns the timestamp of the event.
     *
     * @return Timestamp of the event.
     */
    public double getTimeStamp() {
        return this.timeStamp;
    }

    /**
     * Returns the Field the event has been generated for.
     *
     * @return Field the event has been generated for.
     */
    public ConstField getValue() {
        return this.value;
    }

    /**
     * Clone.
     */
    public Object clone() throws CloneNotSupportedException {
        final Event e = (Event) super.clone();
        e.name = this.name;
        e.timeStamp = this.timeStamp;
        e.value = this.value;
        return e;
    }
}
