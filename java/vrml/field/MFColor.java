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

public class MFColor extends MField
{
  public native int getSize();
  public native void clear();
  public native void delete(int index);

  private native void CreateObject(int size, float colors[]);
  private native void CreateObject(float colors[][]);

  public MFColor() { CreateObject(null); }
  public MFColor(float colors[][]) { CreateObject(colors); }
  public MFColor(float colors[]) { CreateObject(colors.length, colors); }
  public MFColor(int size, float colors[]) { CreateObject(size, colors); }

  public native void getValue(float colors[][]);
  public native void getValue(float colors[]);

  public native void get1Value(int index, float colors[]);
  public native void get1Value(int index, SFColor color);

  public native void setValue(float colors[][]);
  public native void setValue(float colors[]);
  public native void setValue(int size, float colors[]);
  public native void setValue(MFColor colors);
  public native void setValue(ConstMFColor colors);

  public native void set1Value(int index, ConstSFColor color);
  public native void set1Value(int index, SFColor color);
  public native void set1Value(int index, float red, float green, float blue);

  public native void addValue(ConstSFColor color);
  public native void addValue(SFColor color);
  public native void addValue(float red, float green, float blue);

  public native void insertValue(int index, ConstSFColor color);
  public native void insertValue(int index, SFColor color);
  public native void insertValue(int index, float red, float green, float blue);

  public native String toString();
}
