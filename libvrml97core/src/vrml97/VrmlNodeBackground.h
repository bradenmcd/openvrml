//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//
//  VrmlNodeBackground.h

#ifndef VRMLNODEBACKGROUND_H
#define VRMLNODEBACKGROUND_H

#include "VrmlNodeChild.h"
#include "VrmlField.h"

#include "Image.h"
#include "VrmlMFColor.h"
#include "VrmlMFFloat.h"
#include "VrmlMFString.h"
#include "VrmlSFBool.h"
#include "VrmlSFString.h"
#include "Viewer.h"

//class Viewer;
class VrmlScene;

class VrmlNodeBackground : public VrmlNodeChild {

public:

  // Define the fields of Background nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType *nodeType() const;

  VrmlNodeBackground(VrmlScene *);
  virtual ~VrmlNodeBackground();

  // Copy the node.
  virtual VrmlNode *cloneMe() const;

  virtual VrmlNodeBackground* toBackground() const;

  virtual void addToScene( VrmlScene *s, const char *relativeUrl );

  virtual ostream& printFields(ostream& os, int indent);

  // render backgrounds once per scene, not via the render() method
  void renderBindable(Viewer *);

  virtual void eventIn(double timeStamp,
		       const char *eventName,
		       const VrmlField *fieldValue);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  int nGroundAngles()		{ return d_groundAngle.size(); }
  float *groundAngle()		{ return d_groundAngle.get(); }
  float *groundColor()		{ return d_groundColor.get(); }

  int nSkyAngles()		{ return d_skyAngle.size(); }
  float *skyAngle()		{ return d_skyAngle.get(); }
  float *skyColor()		{ return d_skyColor.get(); }

private:

  VrmlMFFloat d_groundAngle;
  VrmlMFColor d_groundColor;

  VrmlMFString d_backUrl;
  VrmlMFString d_bottomUrl;
  VrmlMFString d_frontUrl;
  VrmlMFString d_leftUrl;
  VrmlMFString d_rightUrl;
  VrmlMFString d_topUrl;

  VrmlMFFloat d_skyAngle;
  VrmlMFColor d_skyColor;

  VrmlSFBool d_isBound;

  VrmlSFString d_relativeUrl;

  // Texture caches
  Image *d_texPtr[6];
  Image d_tex[6];

  // Display list object for background
  Viewer::Object d_viewerObject;

};

#endif // VRMLNODEBACKGROUND_H

