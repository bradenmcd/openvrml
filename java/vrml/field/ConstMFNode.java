package vrml.field;

import vrml.ConstMField;
import vrml.BaseNode;

public class ConstMFNode extends ConstMField
{
  public native int getSize();

  private native void CreateObject(int size, BaseNode node[]);

  public ConstMFNode(int size, BaseNode node[]) { CreateObject(size, node); }
  public ConstMFNode(BaseNode node[]) { CreateObject(node.length, node); }
	
  public native void getValue(BaseNode node[]);

  public native BaseNode get1Value(int index);

  public native String toString();
}
