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

#ifndef VRMLNODEAPPEARANCE_H
#define VRMLNODEAPPEARANCE_H

#include "VrmlNodeChild.h"
#include "VrmlSFNode.h"

class VrmlNodeType;
class VrmlScene;

class VrmlNodeAppearance : public VrmlNode {

public:

  // Define the built in VrmlNodeType:: "Appearance"
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeAppearance(VrmlScene *);
  virtual ~VrmlNodeAppearance();

  // Copy the node.
  virtual VrmlNode *cloneMe() const;
  virtual void cloneChildren( VrmlNamespace* );

  virtual VrmlNodeAppearance* toAppearance() const;

  virtual bool isModified() const;
  virtual void updateModified(VrmlNodePath& path);
  virtual void clearFlags();	// Clear childrens flags too.

  virtual void addToScene( VrmlScene *s, const char *relativeUrl );

  virtual void copyRoutes(VrmlNamespace *ns) const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual void render(Viewer *, VrmlRenderContext rc);

  // Get a field or eventOut by name.
  virtual const VrmlField *getField(const char *fieldName) const;

  virtual void setField(const char *fieldName,
			const VrmlField &fieldValue);

  VrmlNode *material()	{ return (VrmlNode *) d_material.get(); }
  VrmlNode *texture()	{ return (VrmlNode *) d_texture.get(); }
  VrmlNode *textureTransform()
    { return (VrmlNode *) d_textureTransform.get(); }

protected:

  VrmlSFNode d_material;
  VrmlSFNode d_texture;
  VrmlSFNode d_textureTransform;

};

#endif // VRMLNODEAPPEARANCE_H

