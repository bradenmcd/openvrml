package vrml;

public class InvalidRouteException extends IllegalArgumentException
{
  /**
   * Constructs an InvalidRouteException with no detail message.
   */
  public InvalidRouteException()
  {
    super();
  }

  /**
   * Constructs an InvalidRouteException with the specified detail message.
   * A detail message is a String that describes this particular exception.
   *
   * @param s the detail message
   */
  public InvalidRouteException(String s)
  {
    super(s);
  }
}
