package vrml.field;

import vrml.MField;

public class MFInt32 extends MField
{
  public native int getSize();
  public native void clear();
  public native void delete(int index);

  private native void CreateObject(int size, int values[]);

  public MFInt32() { CreateObject(0, null); }
  public MFInt32(int size, int values[]) { CreateObject(size, values); }
  public MFInt32(int values[]) { CreateObject(values.length, values); }

  public native void getValue(int values[]);

  public native int get1Value(int index);

  public native void setValue(int values[]);
  public native void setValue(int size, int values[]);
  public native void setValue(MFInt32 value);
  public native void setValue(ConstMFInt32 value);

  public native void set1Value(int index, int i);
  public native void set1Value(int index, ConstSFInt32 i);
  public native void set1Value(int index, SFInt32 i);

  public native void addValue(int i);
  public native void addValue(ConstSFInt32 i);
  public native void addValue(SFInt32 i);

  public native void insertValue(int index, int i);
  public native void insertValue(int index, ConstSFInt32 i);
  public native void insertValue(int index, SFInt32 i);

  public native String toString();
}
