package vrml.field;

import vrml.ConstField;

public class ConstSFRotation extends ConstField
{
  private native void CreateObject(float axisX, float axisY, float axisZ, float angle);

  public ConstSFRotation(float axisX, float axisY, float axisZ, float angle)
  {
    CreateObject(axisX, axisY, axisZ, angle);
  }

  public native void getValue(float rotations[]);

  public native String toString();
}
