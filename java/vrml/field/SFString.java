package vrml.field;

import vrml.Field;

public class SFString extends Field
{
  private native void CreateObject(String value);

  public SFString() { CreateObject(null); }
  public SFString(String s) { CreateObject(s); }

  public native String getValue();

  public native void setValue(String s);
  public native void setValue(ConstSFString s);
  public native void setValue(SFString s);

  public native String toString();
}
