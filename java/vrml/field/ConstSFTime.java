package vrml.field;

import vrml.ConstField;

public class ConstSFTime extends ConstField
{
  private native void CreateObject(double time);

  public ConstSFTime(double time) { CreateObject(time); }

  public native double getValue();

  public native String toString();
}
