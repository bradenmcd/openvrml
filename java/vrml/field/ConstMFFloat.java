package vrml.field;

import vrml.ConstMField;

public class ConstMFFloat extends ConstMField
{
  public native int getSize();

  private native void CreateObject(int size, float values[]);

  public ConstMFFloat(int size, float values[]) { CreateObject(size, values); }
  public ConstMFFloat(float values[])  { CreateObject(values.length, values); }
	
  public native void getValue(float values[]);
	
  public native float get1Value(int index);
	
  public native String toString();
}
