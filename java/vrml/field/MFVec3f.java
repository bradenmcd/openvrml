package vrml.field;

import vrml.MField;

public class MFVec3f extends MField
{
  public native int getSize();
  public native void clear();
  public native void delete(int index);

  private native void CreateObject(float vec3s[][]);
  private native void CreateObject(int size, float vec3s[]);

  public MFVec3f() { CreateObject(0, null); }
  public MFVec3f(float vec3s[][]) { CreateObject(vec3s); }
  public MFVec3f(float vec3s[]) { CreateObject(vec3s.length, vec3s); }
  public MFVec3f(int size, float vec3s[]) { CreateObject(size, vec3s); }

  public native void getValue(float vec3s[][]);
  public native void getValue(float vec3s[]);

  public native void get1Value(int index, float vec3s[]);
  public native void get1Value(int index, SFVec3f vec);

  public native void setValue(float vec3s[][]);
  public void setValue(float vec3s[]) { setValue(vec3s.length, vec3s); }
  public native void setValue(int size, float vec3s[]);
  public native void setValue(MFVec3f vecs);
  public native void setValue(ConstMFVec3f vecs);

  public native void set1Value(int index, float x, float y, float z);
  public native void set1Value(int index, ConstSFVec3f vec);
  public native void set1Value(int index, SFVec3f vec);

  public native void addValue(float x, float y, float z);
  public native void addValue(ConstSFVec3f vec);
  public native void addValue(SFVec3f vec);
	
  public native void insertValue(int index, float x, float y, float z);
  public native void insertValue(int index, ConstSFVec3f vec);
  public native void insertValue(int index, SFVec3f vec);

  public native String toString();
}

