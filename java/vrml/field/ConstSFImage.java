package vrml.field;

import vrml.ConstField;

public class ConstSFImage extends ConstField
{
  private native void CreateObject(int width, int height, int components, byte pixels[]);

  public ConstSFImage(int width, int height, int components, byte pixels[])
  {
    CreateObject(width, height, components, pixels);
  }

  public native int getWidth();
  public native int getHeight();
  public native int getComponents();
  public native void getPixels(byte pixels[]);

  public native String toString();
}
