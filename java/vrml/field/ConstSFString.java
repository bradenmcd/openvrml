package vrml.field;

import vrml.ConstField;

public class ConstSFString extends ConstField
{
  private native void CreateObject(String value);

  public ConstSFString(String value) { CreateObject(value); }

  public native String getValue();

  public native String toString();
}
