package vrml.field;

import vrml.MField;

public class MFFloat extends MField
{
  public native int getSize();
  public native void clear();
  public native void delete(int index);

  private native void CreateObject(int size, float values[]);

  public MFFloat() { CreateObject(0, null); }
  public MFFloat(int size, float values[]) { CreateObject(size, values); }
  public MFFloat(float values[]) { CreateObject(values.length, values); }

  public native void getValue(float values[]);

  public native float get1Value(int index);

  public native void setValue(float values[]);
  public native void setValue(int size, float values[]);
  public native void setValue(MFFloat value);
  public native void setValue(ConstMFFloat value);

  public native void set1Value(int index, float f);
  public native void set1Value(int index, ConstSFFloat f);
  public native void set1Value(int index, SFFloat f);

  public native void addValue(float f);
  public native void addValue(ConstSFFloat f);
  public native void addValue(SFFloat f);

  public native void insertValue(int index, float f);
  public native void insertValue(int index, ConstSFFloat f);
  public native void insertValue(int index, SFFloat f);

  public native String toString();
}
