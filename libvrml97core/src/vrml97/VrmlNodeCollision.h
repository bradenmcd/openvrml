//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//
//  VrmlNodeCollision.h

#ifndef VRMLNODECOLLISION_H
#define VRMLNODECOLLISION_H

#include "VrmlNodeGroup.h"
#include "VrmlSFBool.h"
#include "VrmlSFNode.h"
#include "VrmlSFTime.h"


class VrmlNodeCollision : public VrmlNodeGroup {

public:

  // Define the fields of Collision nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeCollision(VrmlScene *);
  virtual ~VrmlNodeCollision();

  virtual VrmlNode *cloneMe() const;
  virtual void cloneChildren(VrmlNamespace *);

  virtual bool isModified() const;
  virtual void clearFlags();	// Clear childrens flags too.

  virtual void addToScene( VrmlScene *s, const char *rel );

  virtual void copyRoutes(VrmlNamespace *ns) const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

private:

  VrmlSFBool d_collide;
  VrmlSFNode d_proxy;

  // eventOut
  VrmlSFTime d_collideTime;
  
};

#endif // VRMLNODECOLLISION_H

