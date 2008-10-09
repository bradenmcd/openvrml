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

import vrml.ConstMField;

/**
 * Represents a read-only VRML MFTime field in Java.
 */
public class ConstMFTime extends ConstMField {

    public native int getSize();

    /**
     * Construct a new MFTime field in OpenVRML using the given params.
     *
     * @param size Number of values passed in.
     * @param times List of times to initialize object with.
     */
    private native void CreateObject(int size, double times[]);

    /**
     * Construct a read-only MFTime field.
     *
     * @param size Number of values passed in.
     * @param times List of times to initialize object with.
     */
    public ConstMFTime(int size, double times[]) {
        CreateObject(size, times);
    }

    /**
     * Construct a read-only MFTime field.
     *
     * @param times List of times to initialize object with.
     */
    public ConstMFTime(double times[]) {
        CreateObject(times.length, times);
    }

    /**
     * Retrieves the complete list of times making up an MFTime field.
     *
     * @param times List of times contained in field.
     */
    public native void getValue(double times[]);

    /**
     * Retrieve a particular element from an MFTime field.
     *
     * @param index Position of desired element.
     * @return Value at specified position.
     */
    public native double get1Value(int index);

    public native String toString();
}
