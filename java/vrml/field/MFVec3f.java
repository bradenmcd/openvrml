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
 * Represents a VRML MFVec3f field in Java.
 */
public class MFVec3f extends MField {

    public native int getSize();

    public native void clear();

    public native void delete(int index);

    /**
     * Construct a new MFVec3f field in OpenVRML using the given params.
     *
     * @param vec3s An array of sets of x,y,z values
     */
    private native void CreateObject(float vec3s[][]);

    /**
     * Construct a new MFVec3f field in OpenVRML using the given params.
     *
     * @param size Number of SFVec3f elements passed in.
     * @param vec3s List of x,y,z tuples
     */
    private native void CreateObject(int size, float vec3s[]);

    /**
     * Default constructor.
     */
    public MFVec3f() {
        float[][] dummy = new float[0][0];
        CreateObject(dummy);
    }

    /**
     * Construct an MFVec3f field.
     *
     * @param vec3s An array of sets of x,y,z values
     */
    public MFVec3f(float vec3s[][]) {
        CreateObject(vec3s);
    }

    /**
     * Construct an MFVec3f field.
     *
     * @param vec3s List of x,y,z tuples
     */
    public MFVec3f(float vec3s[]) {
        CreateObject(vec3s.length / 3, vec3s);
    }

    /**
     * Construct an MFVec3f field.
     *
     * @param size Number of SFVec3f elements passed in.
     * @param vec3s List of x,y,z tuples
     */
    public MFVec3f(int size, float vec3s[]) {
        CreateObject(size, vec3s);
    }

    /**
     * Retrieves the value of an MFVec3f field.
     *
     * @param vec3s 2D array of x,y,z tuples to be returned.
     */
    public native void getValue(float vec3s[][]);

    /**
     * Retrieves the value of an MFVec3f field.
     *
     * @param vec3s Array of x,y,z tuples to be returned.
     */
    public native void getValue(float vec3s[]);

    /**
     * Retrieves a specific element in an MFVec3f and
     * returns it as a float array.
     *
     * @param index Position of desired element
     * @param vec3s Element at specified position
     */
    public native void get1Value(int index, float vec3s[]);

    /**
     * Retrieves a specific element in an MFVec3f and
     * returns it as an SFVec3f.
     *
     * @param index Position of desired element
     * @param vec Element at specified position
     */
    public native void get1Value(int index, SFVec3f vec);

    /**
     * Set the value of the field.
     *
     * @param vec3s New value for field.
     */
    public native void setValue(float vec3s[][]);

    /**
     * Set the value of the field.
     *
     * @param vec3s New value for field.
     */
    public void setValue(float vec3s[]) {
        setValue(vec3s.length / 3, vec3s);
    }

    /**
     * Set the value of the field.
     *
     * @param size Size of new value for field.
     * @param vec3s New value for field.
     */
    public native void setValue(int size, float vec3s[]);

    /**
     * Set the value of the field.
     *
     * @param vecs New value for field.
     */
    public native void setValue(MFVec3f vecs);

    /**
     * Set the value of the field.
     *
     * @param vecs New value for field.
     */
    public native void setValue(ConstMFVec3f vecs);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param x X-component of new value.
     * @param y Y-component of new value.
     * @param z Z-component of new value.
     */
    public native void set1Value(int index, float x, float y, float z);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param vec New value for element.
     */
    public native void set1Value(int index, ConstSFVec3f vec);

    /**
     * Set a specified element in the field.
     *
     * @param index Position of element to update.
     * @param vec New value for element.
     */
    public native void set1Value(int index, SFVec3f vec);

    /**
     * Add a new element at the end of the list.
     *
     * @param x X-component of the vector to add.
     * @param y Y-component of the vector to add.
     * @param z Z-component of the vector to add.
     */
    public native void addValue(float x, float y, float z);

    /**
     * Add a new element at the end of the list.
     *
     * @param vec Element to add.
     */
    public native void addValue(ConstSFVec3f vec);

    /**
     * Add a new element at the end of the list.
     *
     * @param vec Element to add.
     */
    public native void addValue(SFVec3f vec);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param x X-component of value to insert.
     * @param y Y-component of value to insert.
     * @param z Z-component of value to insert.
     */
    public native void insertValue(int index, float x, float y, float z);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param vec Value to insert.
     */
    public native void insertValue(int index, ConstSFVec3f vec);

    /**
     * Insert a new element at the specified position.
     *
     * @param index Position to insert new element at.
     * @param vec Value to insert.
     */
    public native void insertValue(int index, SFVec3f vec);

    public native String toString();
}
