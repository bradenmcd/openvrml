package vrml;

//public class Event implements Cloneable (this is what the spec specifies!)
public class Event
{
  private int EventPtr;
  public native String getName();
  public native double getTimeStamp();
  public native ConstField getValue();

//	public Object clone();

//	public String toString();
}
