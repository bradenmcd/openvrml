//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//
//  %W% %G%
//  VrmlNodeBindable.h

#ifndef VRMLNODEBINDABLE_H
#define VRMLNODEBINDABLE_H

#include "VrmlNodeChild.h"

class VrmlNodeType;
class VrmlField;
class VrmlScene;

class VrmlNodeBindable : public VrmlNodeChild {

public:

  VrmlNodeBindable::VrmlNodeBindable(VrmlScene *s = 0) : VrmlNodeChild(s) {}

  // Define the fields of all built in bindable nodes
  static VrmlNodeType *defineType(VrmlNodeType *t)
    { return VrmlNode::defineType(t); }

  virtual bool isBindableNode()		{ return true; }

};

#endif // VRMLNODEBINDABLE_H

