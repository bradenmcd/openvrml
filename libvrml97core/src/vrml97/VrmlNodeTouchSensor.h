//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//  See the file COPYING for license details.
//
//  VrmlNodeTouchSensor.h

#ifndef VRMLNODETOUCHSENSOR_H
#define VRMLNODETOUCHSENSOR_H

#include "VrmlNodeChild.h"
#include "VrmlSFBool.h"
#include "VrmlSFTime.h"
#include "VrmlSFVec2f.h"
#include "VrmlSFVec3f.h"

class VrmlScene;

class VrmlNodeTouchSensor : public VrmlNodeChild {

public:

  // Define the fields of TouchSensor nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeTouchSensor( VrmlScene *scene = 0);
  virtual ~VrmlNodeTouchSensor();

  virtual VrmlNode *cloneMe() const;

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
