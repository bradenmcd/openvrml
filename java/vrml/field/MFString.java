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

import vrml.MField;

/**
 * Represents a VRML MFString field in Java.
 */
public class MFString extends MField {

    public native int getSize();

    public native void clear();

    public native void delete(int index);

    /**
     * Construct a new MFString field in OpenVRML using the given params.
     *
     * @param size Number of Strings passed in.
     * @param s Arrays of strings.
     */
    private native void CreateObject(int size, String s[]);

    /**
     * Default constructor.
     */
    public MFString() {
        String dummy[] = new String[0];
        CreateObject(0, dummy);
    }

    /**
     * Construct an MFString field.
     *
     * @param size Number of Strings passed in.
     * @param s Arrays of strings.
     */
    public MFString(int size, String s[]) {
        CreateObject(size, s);
    }

    /**
     * Construct an MFString field.
     *
     * @param s Arrays of strings.
     */
    public MFString(String s[]) {
        CreateObject(s.length, s);
    }

    /**
     * Retrieves the value of an MFString field.
     *
     * @param s Array of strings to be returned.
     */
    public native void getValue(String s[]);

    /**
     * Retrieves a specific SFString element in an MFString and
     * returns it as a String.
     *
     * @param index Position of desired SFString
     * @return Value of SFString at index.
     */
    public native String get1Value(int index);

    /**
     * Set the value of the field.
     *
     * @param s New value for field.
     */
    public void setValue(String s[]) {
        setValue(s.length, s);
    }

    /**
     * Set the value of the field.
     *
     * @param size Size of new value for field.
     * @param s New value for field.
     */
    public native void setValue(int size, String s[]);

    /**
     * Set the value of the field.
     *
     * @param s New value for field.
     */
    public native void setValue(MFString s);

    /**
     * Set the value of the field.
     *
     * @param s New value for field.
     */
    public native void setValue(ConstMFString s);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param s New value for element.
     */
    public native void set1Value(int index, String s);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param s New value for element.
     */
    public native void set1Value(int index, ConstSFString s);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param s New value for element.
     */
    public native void set1Value(int index, SFString s);

    /**
     * Add a new element at the end of the list.
     *
     * @param s Element to add.
     */
    public native void addValue(String s);

    /**
     * Add a new element at the end of the list.
     *
     * @param s Element to add.
     */
    public native void addValue(ConstSFString s);

    /**
     * Add a new element at the end of the list.
     *
     * @param s Element to add.
     */
    public native void addValue(SFString s);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param s Value to insert.
     */
    public native void insertValue(int index, String s);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param s Value to insert.
     */
    public native void insertValue(int index, ConstSFString s);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param s Value to insert.
     */
    public native void insertValue(int index, SFString s);

    public native String toString();
}
