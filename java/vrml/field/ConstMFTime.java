package vrml.field;

import vrml.ConstMField;

public class ConstMFTime extends ConstMField
{
  public native int getSize();

  private native void CreateObject(int size, double times[]);

  public ConstMFTime(int size, double times[]) { CreateObject(size, times); }
  public ConstMFTime(double times[]) { CreateObject(times.length, times); }

  public native void getValue(double times[]);

  public native double get1Value(int index);

  public native String toString();
}
