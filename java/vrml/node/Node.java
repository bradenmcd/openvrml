package vrml.node;

import vrml.BaseNode;
import vrml.Field;
import vrml.ConstField;

//public abstract class Node extends BaseNode
public class Node extends BaseNode
{ 

   // Get an EventIn by name. Return value is write-only.
   //   Throws an InvalidEventInException if eventInName isn't a valid
   //   eventIn name for a node of this type.
   public native final Field getEventIn(String eventInName);

   // Get an EventOut by name. Return value is read-only.
   //   Throws an InvalidEventOutException if eventOutName isn't a valid
   //   eventOut name for a node of this type.
   public native final ConstField getEventOut(String eventOutName);

   // Get an exposed field by name. 
   //   Throws an InvalidExposedFieldException if exposedFieldName isn't a valid
   //   exposedField name for a node of this type.
   public native final Field getExposedField(String exposedFieldName);

//   public String toString();

}
