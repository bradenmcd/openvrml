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

import vrml.BaseNode;
import vrml.Field;
import vrml.ConstField;
import vrml.InvalidEventInException;
import vrml.InvalidEventOutException;
import vrml.InvalidExposedFieldException;

/**
 * The general node class.
 */
public class Node extends BaseNode {

    /**
     * Get an EventIn by name. Return value is write-only.
     *
     * @param eventInName Name of eventIn to retrieve.
     * @return Write-only eventIn field.
     * @throws InvalidEventInException Invalid eventIn name specified.
     */
    public native final Field getEventIn(String eventInName)
        throws InvalidEventInException;

    /**
     * Get an EventOut by name. Return value is read-only.
     *
     * @param eventOutName Name of eventOut to retrieve.
     * @return Read-only eventOut field.
     * @throws InvalidEventOutException Invalid eventOut name specified.
     */
    public native final ConstField getEventOut(String eventOutName)
        throws InvalidEventOutException;

    /**
     * Get an exposed field by name.
     *
     * @param exposedFieldName Name of exposedField to retrieve.
     * @return Exposed field specified by name.
     * @throws InvalidExposedFieldException if exposedField name is invalid.
     */
    public native final Field getExposedField(String exposedFieldName)
        throws InvalidExposedFieldException;

    public native String toString();
}
