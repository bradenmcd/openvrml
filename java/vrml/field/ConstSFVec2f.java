package vrml.field;

import vrml.ConstField;

public class ConstSFVec2f extends ConstField
{
  private native void CreateObject(float x, float y);

  public ConstSFVec2f(float x, float y) { CreateObject(x, y); }

  public native void getValue(float vec2s[]);
  public native float getX();
  public native float getY();

  public native String toString();
}
