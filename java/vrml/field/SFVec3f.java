package vrml.field;

import vrml.Field;

public class SFVec3f extends Field
{
  private native void CreateObject(float x, float y, float z);

  public SFVec3f() { CreateObject(0f, 0f, 0f); }
  public SFVec3f(float x, float y, float z) { CreateObject(x, y, z); }

  public native void getValue(float vec3s[]);
  public native float getX();
  public native float getY();
  public native float getZ();

  public native void setValue(float vec3s[]);
  public native void setValue(float x, float y, float z);
  public native void setValue(ConstSFVec3f vec);
  public native void setValue(SFVec3f vec);

  public native String toString();
}
