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

#ifndef VRMLNODECYLINDERSENSOR_H
#define VRMLNODECYLINDERSENSOR_H

#include "VrmlNodeChild.h"
#include "field.h"

class VrmlScene;

class VrmlNodeCylinderSensor : public VrmlNodeChild {

public:

  // Define the fields of CylinderSensor nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeCylinderSensor( VrmlScene *scene = 0);
  virtual ~VrmlNodeCylinderSensor();

  virtual bool accept(VrmlNodeVisitor & visitor);
  
  virtual VrmlNodeCylinderSensor* toCylinderSensor() const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  void activate( double timeStamp, bool isActive, double *p );

  bool isEnabled() { return d_enabled.get(); }

private:

  // Fields
  VrmlSFBool d_autoOffset;
  VrmlSFFloat d_diskAngle;
  VrmlSFBool d_enabled;
  VrmlSFFloat d_maxAngle;
  VrmlSFFloat d_minAngle;
  VrmlSFFloat d_offset;

  VrmlSFBool d_isActive;
  VrmlSFRotation d_rotation;
  VrmlSFVec3f d_trackPoint;

};

#endif
