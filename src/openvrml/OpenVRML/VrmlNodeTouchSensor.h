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

#ifndef VRMLNODETOUCHSENSOR_H
#define VRMLNODETOUCHSENSOR_H

#include "VrmlNodeChild.h"
#include "field.h"

class VrmlScene;

class VrmlNodeTouchSensor : public VrmlNodeChild {

public:

  // Define the fields of TouchSensor nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeTouchSensor( VrmlScene *scene = 0);
  virtual ~VrmlNodeTouchSensor();

  virtual bool accept(VrmlNodeVisitor & visitor);
  
  virtual VrmlNodeTouchSensor* toTouchSensor() const;

  virtual ostream& printFields(ostream& os, int indent);

  void activate( double timeStamp, bool isOver, bool isActive, double *p );

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  bool isEnabled() { return d_enabled.get(); }

private:

  // Fields
  VrmlSFBool d_enabled;

  // Internal state
  VrmlSFVec3f d_hitNormal;
  VrmlSFVec3f d_hitPoint;
  VrmlSFVec2f d_hitTexCoord;
  VrmlSFBool d_isActive;
  VrmlSFBool d_isOver;
  VrmlSFTime d_touchTime;

};

#endif
