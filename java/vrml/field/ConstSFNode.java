package vrml.field;

import vrml.ConstField;
import vrml.BaseNode;

public class ConstSFNode extends ConstField
{
  private native void CreateObject(BaseNode node);

  public ConstSFNode(BaseNode node) { CreateObject(node); }

  public native BaseNode getValue();

  public native String toString();
}
