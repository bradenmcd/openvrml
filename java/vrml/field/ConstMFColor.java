package vrml.field;

import vrml.ConstMField;

public class ConstMFColor extends ConstMField
{
  public native int getSize();

  private native void CreateObject(int size, float colors[]);
  private native void CreateObject(float colors[][]);

  public ConstMFColor(float colors[][]) { CreateObject(colors); }
  public ConstMFColor(float colors[]) { CreateObject(colors.length, colors); }
  public ConstMFColor(int size, float colors[]) { CreateObject(size, colors); }
       
  public native void getValue(float colors[][]);
  public native void getValue(float colors[]);
	
  public native void get1Value(int index, float colors[]);
  public native void get1Value(int index, SFColor color);
	
  public native String toString();
}
