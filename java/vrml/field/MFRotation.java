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

public class MFRotation extends MField
{
  public native int getSize();
  public native void clear();
  public native void delete(int index);

  private native void CreateObject(float rotations[][]);
  private native void CreateObject(int size, float rotations[]);

  public MFRotation() { CreateObject(0, null); }
  public MFRotation(float rotations[][]) { CreateObject(rotations); }
  public MFRotation(float rotations[]) { CreateObject(rotations.length, rotations); }
  public MFRotation(int size, float rotations[]) { CreateObject(size, rotations); }

  public native void getValue(float rotations[][]);
  public native void getValue(float rotations[]);

  public native void get1Value(int index, float rotations[]);
  public native void get1Value(int index, SFRotation rotation);

  public native void setValue(float rotations[][]);
  public native void setValue(float rotations[]);
  public native void setValue(int size, float rotations[]);
  public native void setValue(MFRotation rotations);
  public native void setValue(ConstMFRotation rotations);

  public native void set1Value(int index, ConstSFRotation rotation);
  public native void set1Value(int index, SFRotation rotation);
  public native void set1Value(int index, float axisX, float axisY, float axisZ, float angle);

  public native void addValue(ConstSFRotation rotation);
  public native void addValue(SFRotation rotation);
  public native void addValue(float axisX, float axisY, float axisZ, float angle);

  public native void insertValue(int index, ConstSFRotation rotation);
  public native void insertValue(int index, SFRotation rotation);
  public native void insertValue(int index, float axisX, float axisY, float axisZ, float angle);

  public native String toString();
}
