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

public class MFVec2f extends MField
{
  public native int getSize();
  public native void clear();
  public native void delete(int index);
	
  private native void CreateObject(float vec2s[][]);
  private native void CreateObject(int size, float vec2s[]);

  public MFVec2f() { CreateObject(0, null); }
  public MFVec2f(float vec2s[][]) { CreateObject(vec2s); }
  public MFVec2f(float vec2s[]) { CreateObject(vec2s.length, vec2s); }
  public MFVec2f(int size, float vec2s[]) { CreateObject(size, vec2s); }

  public native void getValue(float vec2s[][]);
  public native void getValue(float vec2s[]);

  public native void get1Value(int index, float vec2s[]);
  public native void get1Value(int index, SFVec2f vec);

  public native void setValue(float vec2s[][]);
  public void setValue(float vec2s[]) { setValue(vec2s.length, vec2s); }
  public native void setValue(int size, float vec2s[]);
  public native void setValue(MFVec2f vecs);
  public native void setValue(ConstMFVec2f vecs);

  public native void set1Value(int index, float x, float y);
  public native void set1Value(int index, ConstSFVec2f vec);
  public native void set1Value(int index, SFVec2f vec);

  public native void addValue(float x, float y);
  public native void addValue(ConstSFVec2f vec);
  public native void addValue(SFVec2f vec);

  public native void insertValue(int index, float x, float y);
  public native void insertValue(int index, ConstSFVec2f vec);
  public native void insertValue(int index, SFVec2f vec);

  public native String toString();
}
