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

import vrml.ConstMField;

public class ConstMFVec2f extends ConstMField
{
  public native int getSize();

  private native void CreateObject(float vec2s[][]);
  private native void CreateObject(int size, float vec2s[]);

  public ConstMFVec2f(float vec2s[][]) { CreateObject(vec2s); }
  public ConstMFVec2f(float vec2s[]) { CreateObject(vec2s.length, vec2s); }
  public ConstMFVec2f(int size, float vec2s[]) { CreateObject(size, vec2s); }

  public native void getValue(float vec2s[][]);
  public native void getValue(float vec2s[]);

  public native void get1Value(int index, float vec2s[]);
  public native void get1Value(int index, SFVec2f vec);

  public native String toString();
}
