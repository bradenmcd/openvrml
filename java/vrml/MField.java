package vrml;

public abstract class MField extends Field
{
  public abstract int getSize();
  public abstract void clear();
  public abstract void delete(int index);
}
