package vrml.field;

import vrml.ConstMField;

public class ConstMFVec3f extends ConstMField
{
  public native int getSize();

  private native void CreateObject(float vec3s[][]);
  private native void CreateObject(int size, float vec3s[]);

  public ConstMFVec3f(float vec3s[][]) { CreateObject(vec3s); }
  public ConstMFVec3f(float vec3s[]) { CreateObject(vec3s.length, vec3s); }
  public ConstMFVec3f(int size, float vec3s[]) { CreateObject(size, vec3s); }

  public native void getValue(float vec3s[][]);
  public native void getValue(float vec3s[]);

  public native void get1Value(int index, float vec3s[]);
  public native void get1Value(int index, SFVec3f vec);

  public native String toString();
}
