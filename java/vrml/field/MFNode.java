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
