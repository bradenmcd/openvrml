//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//
//  %W% %G%
//  VrmlNodeILineSet.h

#ifndef VRMLNODEILINESET_H
#define VRMLNODEILINESET_H

#include "VrmlNodeIndexedSet.h"
#include "VrmlSFBool.h"
#include "VrmlSFFloat.h"
#include "VrmlMFInt.h"

class VrmlNodeILineSet : public VrmlNodeIndexedSet {

public:

  // Define the fields of indexed line set nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeILineSet(VrmlScene *);
  virtual ~VrmlNodeILineSet();

  virtual VrmlNode *cloneMe() const;
  virtual void cloneChildren(VrmlNamespace*);

  virtual Viewer::Object insertGeometry(Viewer *v);

protected:

  // All fields are defined in VrmlNodeIndexedSet
};

#endif
