package vrml.field;

import vrml.Field;

public class SFColor extends Field
{
  private native void CreateObject(float red, float green, float blue);

  public SFColor() { CreateObject(0f, 0f, 0f); }
  public SFColor(float red, float green, float blue) { CreateObject(red, green, blue); }
	
  public native void getValue(float colors[]);
  public native float getRed();
  public native float getGreen();
  public native float getBlue();
	
  public native void setValue(float colors[]);
  public native void setValue(float red, float green, float blue);
  public native void setValue(ConstSFColor color);
  public native void setValue(SFColor color);

  public native String toString();
}
