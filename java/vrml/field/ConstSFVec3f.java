package vrml.field;

import vrml.ConstField;

public class ConstSFVec3f extends ConstField
{
  private native void CreateObject(float x, float y, float z);

  public ConstSFVec3f(float x, float y, float z) { CreateObject(x, y, z); }

  public native void getValue(float vec3s[]);
  public native float getX();
  public native float getY();
  public native float getZ();

  public native String toString();
}
