//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//  See the file COPYING for license details.
//
//  VrmlNodeColor.h

#ifndef VRMLNODECOLOR_H
#define VRMLNODECOLOR_H

#include "VrmlNode.h"
#include "VrmlMFColor.h"

class VrmlScene;

class VrmlNodeColor : public VrmlNode {

public:

  // Define the fields of Color nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeColor(VrmlScene *);
  virtual ~VrmlNodeColor();

  virtual VrmlNode *cloneMe() const;

  virtual VrmlNodeColor* toColor() const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  VrmlMFColor &color()	{ return d_color; }

private:

  VrmlMFColor d_color;
  
};

#endif // VRMLNODECOLOR_H

