package vrml;

public abstract class BaseNode 
{
  private int NodePtr;

  // Returns the type of the node.  If the node is a prototype
  // it returns the name of the prototype.
  public native String getType();
    
  // Get the Browser that this node is contained in.
  public native Browser getBrowser();
}
