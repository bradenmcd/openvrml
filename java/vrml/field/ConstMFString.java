package vrml.field;

import vrml.*;

public class ConstMFString extends ConstMField
{
  public native int getSize();

  private native void CreateObject(int size, String s[]);

  public ConstMFString(int size, String s[]) { CreateObject(size, s); }
  public ConstMFString(String s[]) { CreateObject(s.length, s); }

  public native void getValue(String values[]);

  public native String get1Value(int index);

  public native String toString();
}

