package vrml.field;

import vrml.Field;

public class SFVec2f extends Field
{
  private native void CreateObject(float x, float y);

  public SFVec2f() { CreateObject(0f, 0f); }
  public SFVec2f(float x, float y) { CreateObject(x, y); }

  public native void getValue(float vec2s[]);
  public native float getX();
  public native float getY();

  public native void setValue(float vec2s[]);
  public native void setValue(float x, float y);
  public native void setValue(ConstSFVec2f vec);
  public native void setValue(SFVec2f vec);

  public native String toString();
}
