package vrml.field;

import vrml.ConstField;

public class ConstSFFloat extends ConstField
{
  private native void CreateObject(float value);

  public ConstSFFloat(float value) { CreateObject(value); }
  public native float getValue();
  public native String toString();
}
