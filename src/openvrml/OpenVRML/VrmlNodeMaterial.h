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

#ifndef  _VRMLNODEMATERIAL_
#define  _VRMLNODEMATERIAL_

#include "VrmlNode.h"
#include "field.h"

class VrmlNodeMaterial : public VrmlNode {

public:

  // Define the fields of Material nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeMaterial(VrmlScene *);
  virtual ~VrmlNodeMaterial();

  virtual bool accept(VrmlNodeVisitor & visitor);
  
  virtual VrmlNodeMaterial* toMaterial()	const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual void render(Viewer *, VrmlRenderContext rc);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

        float  ambientIntensity() const { return d_ambientIntensity.get(); }
  const float* diffuseColor() const { return d_diffuseColor.get(); }
  const float* emissiveColor() const { return d_emissiveColor.get(); }
        float  shininess() const { return d_shininess.get(); }
  const float* specularColor() const { return d_specularColor.get(); }
        float  transparency() const { return d_transparency.get(); }

private:

  VrmlSFFloat d_ambientIntensity;
  VrmlSFColor d_diffuseColor;
  VrmlSFColor d_emissiveColor;
  VrmlSFFloat d_shininess;
  VrmlSFColor d_specularColor;
  VrmlSFFloat d_transparency;

};

#endif _VRMLNODEMATERIAL_

