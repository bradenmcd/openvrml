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

public class MFTime extends MField
{
  public native int getSize();
  public native void clear();
  public native void delete(int index);

  private native void CreateObject(int size, double times[]);

  public MFTime() { CreateObject(0, null); }
  public MFTime(int size, double times[]) { CreateObject(size, times); }
  public MFTime(double times[]) { CreateObject(times.length, times); }

  public native void getValue(double times[]);

  public native double get1Value(int index);

  public void setValue(double times[]) { setValue(times.length, times); }
  public native void setValue(int size, double times[]);
  public native void setValue(MFTime times);
  public native void setValue(ConstMFTime times);

  public native void set1Value(int index, double time);
  public native void set1Value(int index, ConstSFTime time);
  public native void set1Value(int index, SFTime time);

  public native void addValue(double time);
  public native void addValue(ConstSFTime time);
  public native void addValue(SFTime time);

  public native void insertValue(int index, double time);
  public native void insertValue(int index, ConstSFTime time);
  public native void insertValue(int index, SFTime time);

  public native String toString();
}

