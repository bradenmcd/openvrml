//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//
//  VrmlNodeBox.h

#ifndef VRMLNODEBOX_H
#define VRMLNODEBOX_H

#include "VrmlNodeGeometry.h"
#include "VrmlSFVec3f.h"
#include "VrmlBSphere.h"

class VrmlScene;

class VrmlNodeBox : public VrmlNodeGeometry {

public:

  // Define the fields of box nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeBox(VrmlScene *);
  virtual ~VrmlNodeBox();

  virtual VrmlNode *cloneMe() const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  virtual VrmlNodeBox* toBox() const; //LarryD Mar 08/99
  virtual const VrmlSFVec3f& getSize() const;  //LarryD Mar 08/99

  const VrmlBVolume* getBVolume() const;

protected:

  VrmlSFVec3f d_size;
  VrmlBSphere d_bsphere;

};

#endif VRMLNODEBOX_H
