package vrml.field;

import vrml.MField;

public class MFString extends MField
{
  public native int getSize();
  public native void clear();
  public native void delete(int index);

  private native void CreateObject(int size, String s[]);

  public MFString() { CreateObject(0, null); }
  public MFString(int size, String s[]) { CreateObject(size, s); }
  public MFString(String s[]) { CreateObject(s.length, s); }

  public native void getValue(String s[]);

  public native String get1Value(int index);
	
  public void setValue(String s[]) { setValue(s.length, s); }
  public native void setValue(int size, String s[]);
  public native void setValue(MFString s);
  public native void setValue(ConstMFString s);

  public native void set1Value(int index, String s);
  public native void set1Value(int index, ConstSFString s);
  public native void set1Value(int index, SFString s);

  public native void addValue(String s);
  public native void addValue(ConstSFString s);
  public native void addValue(SFString s);

  public native void insertValue(int index, String s);
  public native void insertValue(int index, ConstSFString s);
  public native void insertValue(int index, SFString s);

  public native String toString();
}
