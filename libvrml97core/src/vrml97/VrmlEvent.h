//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
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

#ifndef VRMLEVENT_H
#define VRMLEVENT_H

#include "VrmlField.h"
#include <string.h>

class VrmlEvent {

public:

  VrmlEvent(double timeStamp, const char *eventName, const VrmlField *value) :
    d_timeStamp(timeStamp),
    d_eventName(new char[strlen(eventName)+1]),
    d_value(value->clone()),
    d_refCount(1)
  { strcpy(d_eventName, eventName); }

  ~VrmlEvent() { delete [] d_eventName; delete d_value; }

  // VrmlEvents are reference counted.
  // The reference counting is manual (that is, each user of a
  // VrmlEvent, such as the VrmlNode class, calls reference()
  // and dereference() explicitly). Should make it internal...

  // Add/remove references to a VrmlEvent. This is silly, as it
  // requires the users of VrmlEvent to do the reference/derefs...
  VrmlEvent *reference() { ++d_refCount; return this; }
  void dereference() { if (--d_refCount == 0) delete this; }

  double timeStamp() const { return d_timeStamp; }
  const char * name() const { return d_eventName; }
  const VrmlField * value() const { return d_value; }

protected:

  double d_timeStamp;
  char *d_eventName;
  VrmlField *d_value;
  int d_refCount;

};

#endif VRMLEVENT_H

