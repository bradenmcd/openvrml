//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//
//  VrmlNodeNormalInt.h

#ifndef VRMLNODENORMALINT_H
#define VRMLNODENORMALINT_H

#include "VrmlNodeChild.h"
#include "VrmlSFFloat.h"
#include "VrmlMFFloat.h"
#include "VrmlMFVec3f.h"

class VrmlScene;

class VrmlNodeNormalInt : public VrmlNodeChild {

public:

  // Define the fields of NormalInt nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeNormalInt( VrmlScene *scene = 0);
  virtual ~VrmlNodeNormalInt();

  virtual VrmlNode *cloneMe() const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual void eventIn(double timeStamp,
		       const char *eventName,
		       const VrmlField *fieldValue);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

private:

  // Fields
  VrmlMFFloat d_key;
  VrmlMFVec3f d_keyValue;

  // State
  VrmlMFVec3f d_value;
};

#endif
