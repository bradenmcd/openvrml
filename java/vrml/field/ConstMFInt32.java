package vrml.field;

import vrml.ConstMField;

public class ConstMFInt32 extends ConstMField
{
  public native int getSize();

  private native void CreateObject(int size, int values[]);

  public ConstMFInt32(int size, int values[]) { CreateObject(size, values); }
  public ConstMFInt32(int values[]) { CreateObject(values.length, values); }

  public native void getValue(int values[]);

  public native int get1Value(int index);

  public native String toString();
}
