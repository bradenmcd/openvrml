//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//  See the file COPYING for license details.
//
//  VrmlNodeIFaceSet.h

#ifndef  _VRMLNODEIFACESET_
#define  _VRMLNODEIFACESET_

#include "VrmlNodeIndexedSet.h"
#include "VrmlSFBool.h"
#include "VrmlSFFloat.h"
#include "VrmlSFNode.h"
#include "VrmlMFInt.h"
#include "VrmlBSphere.h"

class VrmlNodeIFaceSet : public VrmlNodeIndexedSet {

public:

  // Define the fields of indexed face set nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeIFaceSet(VrmlScene *);
  virtual ~VrmlNodeIFaceSet();

  virtual VrmlNode *cloneMe() const;
  virtual void cloneChildren(VrmlNamespace *);

  virtual bool isModified() const;
  virtual void updateModified(VrmlNodePath& path, int flags);
  virtual void clearFlags();

  virtual void addToScene( VrmlScene *s, const char *relUrl );

  virtual void copyRoutes( VrmlNamespace *ns ) const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual Viewer::Object insertGeometry(Viewer *v, VrmlRenderContext rc);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  virtual VrmlNodeIFaceSet* toIFaceSet() const;

  virtual VrmlNode* getNormal();
  virtual const VrmlMFInt& getNormalIndex() const;

  virtual VrmlNode* getTexCoord();
  virtual const VrmlMFInt& getTexCoordIndex() const;

  virtual bool getCcw(){ return d_ccw.get(); }  // LarryD  Feb18/99
  virtual bool getConvex(){ return d_convex.get(); }   // LarryD Feb18/99
  virtual float getCreaseAngle(){ return d_creaseAngle.get();} // LarryD  Feb18/99
  virtual bool getNormalPerVertex(){ return d_normalPerVertex.get();} // LarryD  Feb18/99
  virtual bool getSolid(){ return d_solid.get();} // LarryD  Feb18/99

  const VrmlBVolume* getBVolume() const;

protected:

  VrmlSFBool d_ccw;
  VrmlSFBool d_convex;
  VrmlSFFloat d_creaseAngle;
  VrmlSFNode d_normal;
  VrmlMFInt d_normalIndex;
  VrmlSFBool d_normalPerVertex;
  VrmlSFBool d_solid;
  VrmlSFNode d_texCoord;
  VrmlMFInt d_texCoordIndex;

  VrmlBSphere d_bsphere;
  void recalcBSphere();
  
};

#endif // _VRMLNODEIFACESET_

