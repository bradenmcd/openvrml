package vrml.field;

import vrml.Field;

public class SFFloat extends Field
{
  private native void CreateObject(float value);

  public SFFloat() { CreateObject(0f); }
  public SFFloat(float f) { CreateObject(f); }
  public native float getValue();

  public native void setValue(float f);
  public native void setValue(ConstSFFloat f);
  public native void setValue(SFFloat f);

  public native String toString();
}
