package vrml;

public class InvalidEventOutException extends IllegalArgumentException
{
  /**
   * Constructs an InvalidEventOutException with no detail message.
   */
  public InvalidEventOutException()
  {
    super();
  }

  /**
   * Constructs an InvalidEventOutException with the specified detail message.
   * A detail message is a String that describes this particular exception.
   *
   * @param s the detail message
   */
  public InvalidEventOutException(String s)
  {
    super(s);
  }
}
