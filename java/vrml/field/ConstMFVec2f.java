package vrml.field;

import vrml.ConstMField;

public class ConstMFVec2f extends ConstMField
{
  public native int getSize();

  private native void CreateObject(float vec2s[][]);
  private native void CreateObject(int size, float vec2s[]);

  public ConstMFVec2f(float vec2s[][]) { CreateObject(vec2s); }
  public ConstMFVec2f(float vec2s[]) { CreateObject(vec2s.length, vec2s); }
  public ConstMFVec2f(int size, float vec2s[]) { CreateObject(size, vec2s); }

  public native void getValue(float vec2s[][]);
  public native void getValue(float vec2s[]);

  public native void get1Value(int index, float vec2s[]);
  public native void get1Value(int index, SFVec2f vec);

  public native String toString();
}
