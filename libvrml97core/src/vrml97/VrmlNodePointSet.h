//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//  See the file COPYING for license details.
//
//  VrmlNodePointSet.h

#ifndef  _VRMLNODEPOINTSET_
#define  _VRMLNODEPOINTSET_

#include "VrmlNodeGeometry.h"
#include "VrmlSFNode.h"
#include "VrmlBSphere.h"


class VrmlNodePointSet : public VrmlNodeGeometry {

public:

  // Define the fields of pointSet nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodePointSet(VrmlScene *);
  virtual ~VrmlNodePointSet();

  virtual VrmlNode *cloneMe() const;
  virtual void cloneChildren(VrmlNamespace *);
  virtual void updateModified(VrmlNodePath& path);
  virtual bool isModified() const;

  virtual void clearFlags();

  virtual void addToScene( VrmlScene *s, const char *relUrl );

  virtual void copyRoutes(VrmlNamespace *ns) const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  const VrmlBVolume* getBVolume() const;

protected:

  VrmlSFNode d_color;
  VrmlSFNode d_coord;

  VrmlBSphere d_bsphere;
  void recalcBSphere();
};

#endif _VRMLNODEPOINTSET_
