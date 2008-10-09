//
// OpenVRML
//
// Copyright 2001  Thomas Flynn
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
 * Thrown when an invalid field name is used to request a field.
 */
public class InvalidFieldException extends IllegalArgumentException {
    /**
     * Constructs an InvalidFieldException with no detail message.
     */
    public InvalidFieldException() {
        super();
    }

    /**
     * Constructs an InvalidFieldException with the specified detail message.
     * A detail message is a String that describes this particular exception.
     *
     * @param s the detail message
     */
    public InvalidFieldException(String s) {
        super(s);
    }
}
