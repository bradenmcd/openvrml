package vrml.field;

import vrml.MField;

public class MFVec2f extends MField
{
  public native int getSize();
  public native void clear();
  public native void delete(int index);
	
  private native void CreateObject(float vec2s[][]);
  private native void CreateObject(int size, float vec2s[]);

  public MFVec2f() { CreateObject(0, null); }
  public MFVec2f(float vec2s[][]) { CreateObject(vec2s); }
  public MFVec2f(float vec2s[]) { CreateObject(vec2s.length, vec2s); }
  public MFVec2f(int size, float vec2s[]) { CreateObject(size, vec2s); }

  public native void getValue(float vec2s[][]);
  public native void getValue(float vec2s[]);

  public native void get1Value(int index, float vec2s[]);
  public native void get1Value(int index, SFVec2f vec);

  public native void setValue(float vec2s[][]);
  public void setValue(float vec2s[]) { setValue(vec2s.length, vec2s); }
  public native void setValue(int size, float vec2s[]);
  public native void setValue(MFVec2f vecs);
  public native void setValue(ConstMFVec2f vecs);

  public native void set1Value(int index, float x, float y);
  public native void set1Value(int index, ConstSFVec2f vec);
  public native void set1Value(int index, SFVec2f vec);

  public native void addValue(float x, float y);
  public native void addValue(ConstSFVec2f vec);
  public native void addValue(SFVec2f vec);

  public native void insertValue(int index, float x, float y);
  public native void insertValue(int index, ConstSFVec2f vec);
  public native void insertValue(int index, SFVec2f vec);

  public native String toString();
}
