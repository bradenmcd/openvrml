package vrml.field;

import vrml.Field;

public class SFTime extends Field
{
  private native void CreateObject(double time);

  public SFTime() { CreateObject(0.0); }
  public SFTime(double time) { CreateObject(time); }

  public native double getValue();

  public native void setValue(double time);
  public native void setValue(ConstSFTime time);
  public native void setValue(SFTime time);

  public native String toString();
}
