//
// OpenVRML
//
// Copyright (C) 2001  Henri Manson 
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

public class MFVec3f extends MField
{
  public native int getSize();
  public native void clear();
  public native void delete(int index);

  private native void CreateObject(float vec3s[][]);
  private native void CreateObject(int size, float vec3s[]);

  public MFVec3f() { CreateObject(0, null); }
  public MFVec3f(float vec3s[][]) { CreateObject(vec3s); }
  public MFVec3f(float vec3s[]) { CreateObject(vec3s.length, vec3s); }
  public MFVec3f(int size, float vec3s[]) { CreateObject(size, vec3s); }

  public native void getValue(float vec3s[][]);
  public native void getValue(float vec3s[]);

  public native void get1Value(int index, float vec3s[]);
  public native void get1Value(int index, SFVec3f vec);

  public native void setValue(float vec3s[][]);
  public void setValue(float vec3s[]) { setValue(vec3s.length, vec3s); }
  public native void setValue(int size, float vec3s[]);
  public native void setValue(MFVec3f vecs);
  public native void setValue(ConstMFVec3f vecs);

  public native void set1Value(int index, float x, float y, float z);
  public native void set1Value(int index, ConstSFVec3f vec);
  public native void set1Value(int index, SFVec3f vec);

  public native void addValue(float x, float y, float z);
  public native void addValue(ConstSFVec3f vec);
  public native void addValue(SFVec3f vec);
	
  public native void insertValue(int index, float x, float y, float z);
  public native void insertValue(int index, ConstSFVec3f vec);
  public native void insertValue(int index, SFVec3f vec);

  public native String toString();
}

