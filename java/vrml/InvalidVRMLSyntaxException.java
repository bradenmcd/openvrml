package vrml;

public class InvalidVRMLSyntaxException extends Exception
{
  /**
   * Constructs an InvalidVRMLSyntaxException with no detail message.
   */
  public InvalidVRMLSyntaxException()
  {
    super();
  }

  /**
   * Constructs an InvalidVRMLSyntaxException with the specified detail
   * message. A detail message is a String that describes this particular
   * exception.
   *
   * @param s the detail message
   */
  public InvalidVRMLSyntaxException(String s)
  {
    super(s);
  }

  public String getMessage()
  {
    return super.getMessage();
  }
}
