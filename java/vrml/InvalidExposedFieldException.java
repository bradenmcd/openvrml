package vrml;

public class InvalidExposedFieldException extends IllegalArgumentException
{
  /**
   * Constructs an InvalidExposedFieldException with no detail message.
   */
  public InvalidExposedFieldException()
  {
    super();
  }

  /**
   * Constructs an InvalidExposedFieldException with the specified detail
   * message. A detail message is a String that describes this particular
   * exception.
   *
   * @param s the detail message
   */
  public InvalidExposedFieldException(String s)
  {
    super(s);
  }
}
