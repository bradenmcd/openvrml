package vrml.node;

import vrml.Event;
import vrml.Field;
import vrml.BaseNode;

public abstract class Script extends BaseNode
{ 
  static
  {
    System.loadLibrary("openvrml");
  }

  // This method is called before any event is generated
  public void initialize() { }

  // Get a Field by name.
  //   Throws an InvalidFieldException if fieldName isn't a valid
  //   field name for a node of this type.
  protected native final Field getField(String fieldName);

  // Get an EventOut by name.
  //   Throws an InvalidEventOutException if eventOutName isn't a valid
  //   eventOut name for a node of this type.
  protected native final Field getEventOut(String eventOutName);

  // Get an EventIn by name.
  //   Throws an InvalidEventInException if eventInName isn't a valid
  //   eventIn name for a node of this type.
  protected native final Field getEventIn(String eventInName);

  // processEvents() is called automatically when the script receives 
  //   some set of events. It shall not be called directly except by its subclass.
  //   count indicates the number of events delivered.
  public void processEvents(int count, Event events[]) { }

  // processEvent() is called automatically when the script receives 
  // an event. 
  public void processEvent(Event event) { }

  // eventsProcessed() is called after every invocation of processEvents().
  public void eventsProcessed() { }

  // shutdown() is called when this Script node is deleted.
  public void shutdown() { }

  public native String toString();
}
