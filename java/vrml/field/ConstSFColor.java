package vrml.field;

import vrml.ConstField;

public class ConstSFColor extends ConstField
{
  private native void CreateObject(float red, float green, float blue);

  public ConstSFColor(float red, float green, float blue) { CreateObject(red, green, blue); }
	
  public native void getValue(float colors[]);
  public native float getRed();
  public native float getGreen();
  public native float getBlue();
	
  public native String toString();
}
