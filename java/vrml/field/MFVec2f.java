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
 * Represents a VRML MFVec2f field in Java.
 */
public class MFVec2f extends MField {

    public native int getSize();

    public native void clear();

    public native void delete(int index);

    /**
     * Construct a new MFVec2f field in OpenVRML using the given params.
     *
     * @param vec2s An array of sets of x,y values
     */
    private native void CreateObject(float vec2s[][]);

    /**
     * Construct a new MFVec2f field in OpenVRML using the given params.
     *
     * @param size Number of SFVec2f elements passed in.
     * @param vec2s List of x,y pairs
     */
    private native void CreateObject(int size, float vec2s[]);

    /**
     * Default constructor.
     */
    public MFVec2f() {
        float[][] dummy = new float[0][0];
        CreateObject(dummy);
    }

    /**
     * Construct an MFVec2f field.
     *
     * @param vec2s An array of sets of x,y values
     */
    public MFVec2f(float vec2s[][]) {
        CreateObject(vec2s);
    }

    /**
     * Construct an MFVec2f field.
     *
     * @param vec2s List of x,y pairs
     */
    public MFVec2f(float vec2s[]) {
        CreateObject(vec2s.length / 2, vec2s);
    }

    /**
     * Construct an MFVec2f field.
     *
     * @param size Number of SFVec2f elements passed in.
     * @param vec2s List of x,y pairs
     */
    public MFVec2f(int size, float vec2s[]) {
        CreateObject(size, vec2s);
    }

    /**
     * Retrieves the value of an MFVec2f field.
     *
     * @param vec2s 2D array of x,y pairs to be returned.
     */
    public native void getValue(float vec2s[][]);

    /**
     * Retrieves the value of an MFVec2f field.
     *
     * @param vec2s Array of x,y pairs to be returned.
     */
    public native void getValue(float vec2s[]);

    /**
     * Retrieves a specific element in an MFVec2f and
     * returns it as a float array.
     *
     * @param index Position of desired element
     * @param vec2s Element at specified position
     */
    public native void get1Value(int index, float vec2s[]);

    /**
     * Retrieves a specific element in an MFVec2f and
     * returns it as an SFVec2f.
     *
     * @param index Position of desired element
     * @param vec Element at specified position
     */
    public native void get1Value(int index, SFVec2f vec);

    /**
     * Set the value of the field.
     *
     * @param vec2s New value for field.
     */
    public native void setValue(float vec2s[][]);

    /**
     * Set the value of the field.
     *
     * @param vec2s New value for field.
     */
    public void setValue(float vec2s[]) {
        setValue(vec2s.length / 2, vec2s);
    }

    /**
     * Set the value of the field.
     *
     * @param size Size of new value for field.
     * @param vec2s New value for field.
     */
    public native void setValue(int size, float vec2s[]);

    /**
     * Set the value of the field.
     *
     * @param vecs New value for field.
     */
    public native void setValue(MFVec2f vecs);

    /**
     * Set the value of the field.
     *
     * @param vecs New value for field.
     */
    public native void setValue(ConstMFVec2f vecs);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param x X-component of the new value.
     * @param y Y-component of the new value.
     */
    public native void set1Value(int index, float x, float y);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param vec New value for element.
     */
    public native void set1Value(int index, ConstSFVec2f vec);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param vec New value for element.
     */
    public native void set1Value(int index, SFVec2f vec);

    /**
     * Add a new element at the end of the list.
     *
     * @param x X-component of the vector to add.
     * @param y Y-component of the vector to add.
     */
    public native void addValue(float x, float y);

    /**
     * Add a new element at the end of the list.
     *
     * @param vec Element to add.
     */
    public native void addValue(ConstSFVec2f vec);

    /**
     * Add a new element at the end of the list.
     *
     * @param vec Element to add.
     */
    public native void addValue(SFVec2f vec);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param x X-component of value to insert.
     * @param y Y-component of value to insert.
     */
    public native void insertValue(int index, float x, float y);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param vec Value to insert.
     */
    public native void insertValue(int index, ConstSFVec2f vec);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param vec Value to insert.
     */
    public native void insertValue(int index, SFVec2f vec);

    public native String toString();
}
