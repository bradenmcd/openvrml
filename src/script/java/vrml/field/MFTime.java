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

import vrml.MField;

/**
 * Represents a VRML MFTime field in Java.
 */
public class MFTime extends MField {

    /**
     * Construct a new MFTime field in OpenVRML using the given params.
     *
     * @param size Number of values passed in.
     * @param times List of times to initialize object with.
     */
    static native long createPeer(int size, double times[]);

    /**
     * Default constructor.
     */
    public MFTime() {
        this(0, null);
    }

    /**
     * Construct from a peer handle.
     */
    MFTime(long peer) {
        super(peer);
    }

    /**
     * Construct an MFTime field.
     *
     * @param size Number of values passed in.
     * @param times List of times to initialize object with.
     */
    public MFTime(int size, double times[]) {
        super(createPeer(size, times));
    }

    /**
     * Construct an MFTime field.
     *
     * @param times List of times to initialize object with.
     */
    public MFTime(double times[]) {
        this(times.length, times);
    }

    public native int getSize();

    public native void clear();

    public native void delete(int index);

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

    /**
     * Set the value of the field.
     *
     * @param times New value for field.
     */
    public void setValue(double times[]) {
        this.setValue(times.length, times);
    }

    /**
     * Set the value of the field.
     *
     * @param size Size of new value for field.
     * @param times New value for field.
     */
    public native void setValue(int size, double times[]);

    /**
     * Set the value of the field.
     *
     * @param times New value for field.
     */
    public native void setValue(MFTime times);

    /**
     * Set the value of the field.
     *
     * @param times New value for field.
     */
    public native void setValue(ConstMFTime times);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param time New value for element.
     */
    public native void set1Value(int index, double time);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param time New value for element.
     */
    public void set1Value(int index, ConstSFTime time) {
        this.set1Value(index, time.getValue());
    }

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param time New value for element.
     */
    public void set1Value(int index, SFTime time) {
        this.set1Value(index, time.getValue());
    }

    /**
     * Add a new element at the end of the list.
     *
     * @param time Element to add.
     */
    public native void addValue(double time);

    /**
     * Add a new element at the end of the list.
     *
     * @param time Element to add.
     */
    public void addValue(ConstSFTime time) {
        this.addValue(time.getValue());
    }

    /**
     * Add a new element at the end of the list.
     *
     * @param time Element to add.
     */
    public void addValue(SFTime time) {
        this.addValue(time.getValue());
    }

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param time Value to insert.
     */
    public native void insertValue(int index, double time);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param time Value to insert.
     */
    public void insertValue(int index, ConstSFTime time) {
        this.insertValue(index, time.getValue());
    }

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param time Value to insert.
     */
    public void insertValue(int index, SFTime time) {
        this.insertValue(index, time.getValue());
    }
}
