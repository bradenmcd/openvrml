//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//  See the file COPYING for license details.
//
//  VrmlNodeSphereSensor.h

#ifndef VRMLNODESPHERESENSOR_H
#define VRMLNODESPHERESENSOR_H

#include "VrmlNodeChild.h"
#include "VrmlSFBool.h"
#include "VrmlSFRotation.h"
#include "VrmlSFVec3f.h"

class VrmlScene;


class VrmlNodeSphereSensor : public VrmlNodeChild {

public:

  // Define the fields of SphereSensor nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeSphereSensor( VrmlScene *scene = 0);
  virtual ~VrmlNodeSphereSensor();

  virtual VrmlNode *cloneMe() const;

  virtual VrmlNodeSphereSensor* toSphereSensor() const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  void activate( double timeStamp, bool isActive, double *p );

  bool isEnabled() { return d_enabled.get(); }

private:

  // Fields
  VrmlSFBool d_autoOffset;
  VrmlSFBool d_enabled;
  VrmlSFRotation d_offset;

  VrmlSFBool d_isActive;
  VrmlSFRotation d_rotation;
  VrmlSFVec3f d_trackPoint;

  VrmlSFVec3f d_activationPoint;
  VrmlSFVec3f d_centerPoint;
};

#endif
