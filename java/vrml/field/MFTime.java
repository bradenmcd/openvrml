package vrml.field;

import vrml.MField;

public class MFTime extends MField
{
  public native int getSize();
  public native void clear();
  public native void delete(int index);

  private native void CreateObject(int size, double times[]);

  public MFTime() { CreateObject(0, null); }
  public MFTime(int size, double times[]) { CreateObject(size, times); }
  public MFTime(double times[]) { CreateObject(times.length, times); }

  public native void getValue(double times[]);

  public native double get1Value(int index);

  public void setValue(double times[]) { setValue(times.length, times); }
  public native void setValue(int size, double times[]);
  public native void setValue(MFTime times);
  public native void setValue(ConstMFTime times);

  public native void set1Value(int index, double time);
  public native void set1Value(int index, ConstSFTime time);
  public native void set1Value(int index, SFTime time);

  public native void addValue(double time);
  public native void addValue(ConstSFTime time);
  public native void addValue(SFTime time);

  public native void insertValue(int index, double time);
  public native void insertValue(int index, ConstSFTime time);
  public native void insertValue(int index, SFTime time);

  public native String toString();
}

