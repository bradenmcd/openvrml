package vrml.field;

import vrml.Field;
import vrml.BaseNode;

public class SFNode extends Field
{
  private native void CreateObject(BaseNode node);

  public SFNode() { CreateObject(null); }
  public SFNode(BaseNode node) { CreateObject(node); }

  public native BaseNode getValue();

  public native void setValue(BaseNode node);
  public native void setValue(ConstSFNode node);
  public native void setValue(SFNode node);

  public native String toString();
}

