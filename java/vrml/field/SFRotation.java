package vrml.field;

import vrml.Field;

public class SFRotation extends Field
{
  private native void CreateObject(float axisX, float axisY, float axisZ, float angle);

  public SFRotation() { CreateObject(0f, 0f, 0f, 0f); }
  public SFRotation(float axisX, float axisY, float axisZ, float angle) {	CreateObject(axisX, axisY, axisZ, angle); }

  public native void getValue(float rotations[]);

  public native void setValue(float rotations[]);
  public native void setValue(float axisX, float axisY, float axisZ, float angle);
  public native void setValue(ConstSFRotation rotation);
  public native void setValue(SFRotation rotation);

  public native String toString();
}
