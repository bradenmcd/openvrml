//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//  See the file COPYING for license details.
//
//  VrmlNodeCylinderSensor.h

#ifndef VRMLNODECYLINDERSENSOR_H
#define VRMLNODECYLINDERSENSOR_H

#include "VrmlNodeChild.h"
#include "VrmlSFBool.h"
#include "VrmlSFFloat.h"
#include "VrmlSFRotation.h"
#include "VrmlSFVec3f.h"

class VrmlScene;

class VrmlNodeCylinderSensor : public VrmlNodeChild {

public:

  // Define the fields of CylinderSensor nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeCylinderSensor( VrmlScene *scene = 0);
  virtual ~VrmlNodeCylinderSensor();

  virtual VrmlNode *cloneMe() const;

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
