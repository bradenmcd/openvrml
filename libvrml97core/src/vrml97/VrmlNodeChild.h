//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//
//  VrmlNodeChild.h

#ifndef VRMLNODECHILD_H
#define VRMLNODECHILD_H

#include "VrmlNode.h"
class VrmlNodeScene;

/**
 * Base class for all nodes that may be children of a Group node. 
 */
class VrmlNodeChild : public VrmlNode {

public:

  // Define the fields of all built in child nodes
  static VrmlNodeType *defineType(VrmlNodeType *t);

  VrmlNodeChild(VrmlScene *);

  virtual VrmlNodeChild* toChild() const;

protected:

};

#endif // VRMLNODECHILD_H

