package vrml.field;

import vrml.ConstField;

public class ConstSFInt32 extends ConstField
{
  private native void CreateObject(int value);

  public ConstSFInt32(int value) { CreateObject(value); }
  public native int getValue();
  public native String toString();
}
