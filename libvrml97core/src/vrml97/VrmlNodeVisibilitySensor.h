//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//
//  VrmlNodeVisibilitySensor.h

#ifndef VRMLNODEVISIBILITYSENSOR_H
#define VRMLNODEVISIBILITYSENSOR_H

#include "VrmlNodeChild.h"
#include "VrmlSFBool.h"
#include "VrmlSFRotation.h"
#include "VrmlSFTime.h"
#include "VrmlSFVec3f.h"

class VrmlScene;

class VrmlNodeVisibilitySensor : public VrmlNodeChild {

public:

  // Define the fields of VisibilitySensor nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeVisibilitySensor( VrmlScene *scene = 0);
  virtual ~VrmlNodeVisibilitySensor();

  virtual VrmlNode *cloneMe() const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual void render(Viewer *);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

private:

  // Fields
  VrmlSFVec3f d_center;
  VrmlSFBool d_enabled;
  VrmlSFVec3f d_size;

  // Internal state
  VrmlSFBool d_isActive;
  VrmlSFTime d_enterTime;
  VrmlSFTime d_exitTime;
};

#endif
