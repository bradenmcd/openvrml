//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//
//  VrmlNodeTextureCoordinate.h

#ifndef VRMLNODETEXTURECOORDINATE_H
#define VRMLNODETEXTURECOORDINATE_H

#include "VrmlNode.h"
#include "VrmlMFVec2f.h"


class VrmlNodeTextureCoordinate : public VrmlNode {

public:

  // Define the fields of TextureCoordinate nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeTextureCoordinate(VrmlScene *);
  virtual ~VrmlNodeTextureCoordinate();

  virtual VrmlNode *cloneMe() const;

  virtual VrmlNodeTextureCoordinate* toTextureCoordinate() const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  VrmlMFVec2f &coordinate()	{ return d_point; }

private:

  VrmlMFVec2f d_point;
  
};

#endif
