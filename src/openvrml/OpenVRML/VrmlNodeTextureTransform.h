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

#ifndef  _VRMLNODETEXTURETRANSFORM_
#define  _VRMLNODETEXTURETRANSFORM_

#include "VrmlNode.h"
#include "VrmlSFFloat.h"
#include "VrmlSFVec2f.h"

class Viewer;

class VrmlNodeTextureTransform : public VrmlNode {

public:

  // Define the fields of TextureTransform nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeTextureTransform(VrmlScene *);
  virtual ~VrmlNodeTextureTransform();

  virtual bool accept(VrmlNodeVisitor & visitor);
  
  virtual VrmlNodeTextureTransform* toTextureTransform()	const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual void render(Viewer *, VrmlRenderContext rc);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);
  
  const VrmlSFVec2f & getCenter() const;
  const VrmlSFFloat & getRotation() const;
  const VrmlSFVec2f & getScale() const;
  const VrmlSFVec2f & getTranslation() const;

private:

  VrmlSFVec2f d_center;
  VrmlSFFloat d_rotation;
  VrmlSFVec2f d_scale;
  VrmlSFVec2f d_translation;
  
};

#endif _VRMLNODETEXTURETRANSFORM_

