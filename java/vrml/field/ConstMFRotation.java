package vrml.field;

import vrml.ConstMField;

public class ConstMFRotation extends ConstMField
{
  public native int getSize();

  private native void CreateObject(float rotations[][]);
  private native void CreateObject(int size, float rotations[]);

  public ConstMFRotation(float rotations[][]) { CreateObject(rotations); }
  public ConstMFRotation(float rotations[]) { CreateObject(rotations.length, rotations); }
  public ConstMFRotation(int size, float rotations[]) { CreateObject(size, rotations); }
	
  public native void getValue(float rotations[][]);
  public native void getValue(float rotations[]);
	
  public native void get1Value(int index, float rotations[]);
  public native void get1Value(int index, SFRotation rotation);
	
  public native String toString();
}
