package vrml.field;

import vrml.Field;

public class SFInt32 extends Field
{
  private native void CreateObject(int value);

  public SFInt32() { CreateObject(0); }
  public SFInt32(int value) { CreateObject(value); }

  public native int getValue();	
  public native void setValue(int i);
  public native void setValue(ConstSFInt32 i);
  public native void setValue(SFInt32 i);
	
  public native String toString();
}
