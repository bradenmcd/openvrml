package vrml;

public class Browser 
{
  private int BrowserPtr;

  public native String toString();
 
  // Browser interface
  public native String getName();    
  public native String getVersion();    
    
  public native float getCurrentSpeed();
    
  public native float getCurrentFrameRate();
    
  public native String getWorldURL();
  public native void replaceWorld(BaseNode[] nodes);
    
  public native BaseNode[] createVrmlFromString(String vrmlSyntax);
  //     throws InvalidVRMLSyntaxException;
    
  public native void createVrmlFromURL(String[] url, BaseNode node, String event);
  //     throws InvalidVRMLSyntaxException;
    
  public native void addRoute(BaseNode fromNode, String fromEventOut,
			      BaseNode toNode, String toEventIn);
    
  public native void deleteRoute(BaseNode fromNode, String fromEventOut,
				 BaseNode toNode, String toEventIn);
    
  public native void loadURL(String[] url, String[] parameter);
  //     throws InvalidVRMLSyntaxException;
    
  public native void setDescription(String description);
}
