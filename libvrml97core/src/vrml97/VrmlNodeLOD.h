//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//
//  VrmlNodeLOD.h

#ifndef  _VRMLNODELOD_
#define  _VRMLNODELOD_

#include "VrmlMFNode.h"
#include "VrmlMFFloat.h"
#include "VrmlSFVec3f.h"
#include "VrmlNodeChild.h"
#include "VrmlBSphere.h"


class VrmlNodeLOD : public VrmlNodeChild {

public:

  // Define the fields of all built in LOD nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeLOD(VrmlScene *);
  virtual ~VrmlNodeLOD();

  virtual VrmlNode *cloneMe() const;
  void cloneChildren(VrmlNamespace *);

  virtual bool isModified() const;
  virtual void updateModified(VrmlNodePath& path);
  virtual void clearFlags();

  virtual void addToScene( VrmlScene *s, const char *relUrl );

  virtual void copyRoutes(VrmlNamespace *ns) const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual void render(Viewer *, VrmlRenderContext rc);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  virtual VrmlNodeLOD* toLOD() const;
  VrmlMFNode *getLevel()  { return &d_level;}
  virtual const VrmlMFFloat& getRange() const;
  virtual const VrmlSFVec3f& getCenter() const;

  const VrmlBVolume* getBVolume() const;

protected:

  VrmlMFNode d_level;
  VrmlSFVec3f d_center;
  VrmlMFFloat d_range;

  /**
   * Cached copy of the bsphere enclosing this node's children.
   */
  VrmlBSphere d_bsphere;

  /**
   * Construct a bounding sphere around this node's children. Store it
   * in d_bsphere.
   */
  virtual void recalcBSphere();
};

#endif _VRMLNODELOD_

