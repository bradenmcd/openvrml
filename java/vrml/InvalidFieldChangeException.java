package vrml;

public class InvalidFieldChangeException extends IllegalArgumentException
{
  /**
   * Constructs an InvalidFieldChangeException with no detail message.
   */
  public InvalidFieldChangeException()
  {
    super();
  }

  /**
   * Constructs an InvalidFieldChangeException with the specified detail 
   * message. A detail message is a String that describes this particular
   * exception.
   *
   * @param s the detail message
   */
  public InvalidFieldChangeException(String s)
  {
    super(s);
  }
}
