package vrml;

public class InvalidFieldException extends IllegalArgumentException
{
  /**
   * Constructs an InvalidFieldException with no detail message.
   */
  public InvalidFieldException()
  {
    super();
  }

  /**
   * Constructs an InvalidFieldException with the specified detail message.
   * A detail message is a String that describes this particular exception.
   *
   * @param s the detail message
   */
  public InvalidFieldException(String s)
  {
    super(s);
  }
}
