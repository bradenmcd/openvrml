package vrml.field;

import vrml.Field;

public class SFBool extends Field
{
  private native void CreateObject(boolean value);

  public SFBool() { CreateObject(false); }
  public SFBool(boolean value) { CreateObject(value); }

  public native boolean getValue();

  public native void setValue(boolean b);
  public native void setValue(ConstSFBool b);
  public native void setValue(SFBool b);

  public native String toString();
}
