//
// OpenVRML
//
// Copyright (C) 2001  Henri Manson 
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// 

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
