//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// 
//  VrmlNodeBackground.h

#ifndef VRMLNODEBACKGROUND_H
#define VRMLNODEBACKGROUND_H

#include "VrmlNodeChild.h"
#include "field.h"
#include "Image.h"
#include "Viewer.h"

//class Viewer;
class VrmlScene;

class VrmlNodeBackground : public VrmlNodeChild {

public:

  // Define the fields of Background nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeBackground(VrmlScene *);
  virtual ~VrmlNodeBackground();

  virtual bool accept(VrmlNodeVisitor & visitor);
  
  virtual VrmlNodeBackground* toBackground() const;

  virtual void addToScene( VrmlScene *s, const char *relativeUrl );

  virtual ostream& printFields(ostream& os, int indent);

  // render backgrounds once per scene, not via the render() method
  void renderBindable(Viewer *);

  virtual void eventIn(double timeStamp,
		       const char *eventName,
		       const VrmlField & fieldValue);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  size_t nGroundAngles()	{ return d_groundAngle.getLength(); }
  const float * groundAngle()	{ return d_groundAngle.get(); }
  const float * groundColor()	{ return d_groundColor.get(); }

  size_t nSkyAngles()		{ return d_skyAngle.getLength(); }
  const float * skyAngle()	{ return d_skyAngle.get(); }
  const float * skyColor()	{ return d_skyColor.get(); }

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

