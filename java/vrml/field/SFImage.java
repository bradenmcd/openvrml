package vrml.field;

import vrml.Field;

public class SFImage extends Field
{
  private native void CreateObject(int width, int height, int components, byte pixels[]);

  public SFImage() { CreateObject(0, 0, 0, null); }
  public SFImage(int width, int height, int components, byte pixels[]) { CreateObject(width, height, components, pixels); }
  public native int getWidth();
  public native int getHeight();
  public native int getComponents();
  public native void getPixels(byte pixels[]);
	
  public native void setValue(int width, int height, int components, byte pixels[]);
  public native void setValue(ConstSFImage image);
  public native void setValue(SFImage image);
	
  public native String toString();
}
