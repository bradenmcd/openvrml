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
import vrml.BaseNode;

public class MFNode extends MField
{
  public native int getSize();
  public native void clear();
  public native void delete(int index);

  private native void CreateObject(int size, BaseNode node[]);

  public MFNode() { CreateObject(0, null); }
  public MFNode(int size, BaseNode node[]) { CreateObject(size, node); }
  public MFNode(BaseNode node[]) { CreateObject(node.length, node); }

  public native void getValue(BaseNode node[]);

  public native BaseNode get1Value(int index);

  public void setValue(BaseNode node[]) { setValue(node.length, node); }
  public native void setValue(int size, BaseNode node[]);
  public native void setValue(MFNode node);
  public native void setValue(ConstMFNode node);

  public native void set1Value(int index, BaseNode node);
  public native void set1Value(int index, ConstSFNode node);
  public native void set1Value(int index, SFNode node);

  public native void addValue(BaseNode node);
  public native void addValue(ConstSFNode node);
  public native void addValue(SFNode node);

  public native void insertValue(int index, BaseNode node);
  public native void insertValue(int index, ConstSFNode node);
  public native void insertValue(int index, SFNode node);

  public native String toString();
}
