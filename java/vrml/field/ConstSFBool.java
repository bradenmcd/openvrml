package vrml.field;

import vrml.ConstField;

public class ConstSFBool extends ConstField
{
  private native void CreateObject(boolean value);

  public ConstSFBool(boolean value) { CreateObject(value); }

  public native boolean getValue();

  public native String toString();
}
