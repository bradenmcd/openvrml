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

#include "VrmlNodeMaterial.h"
#include "MathUtils.h"
#include "VrmlNodeType.h"
#include "Viewer.h"

static VrmlNode *creator( VrmlScene *s ) { return new VrmlNodeMaterial(s); }

// Define the built in VrmlNodeType:: "Material" fields

VrmlNodeType *VrmlNodeMaterial::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Material", creator);
      t->reference();
    }

  VrmlNode::defineType(t);	// Parent class
  t->addExposedField("ambientIntensity", VrmlField::SFFLOAT);
  t->addExposedField("diffuseColor", VrmlField::SFCOLOR);
  t->addExposedField("emissiveColor", VrmlField::SFCOLOR);
  t->addExposedField("shininess", VrmlField::SFFLOAT);
  t->addExposedField("specularColor", VrmlField::SFCOLOR);
  t->addExposedField("transparency", VrmlField::SFFLOAT);

  return t;
}

VrmlNodeType & VrmlNodeMaterial::nodeType() const
{
    return *defineType(0);
}

VrmlNodeMaterial::VrmlNodeMaterial(VrmlScene *scene) :
  VrmlNode(scene),
  d_ambientIntensity(0.2),
  d_diffuseColor(0.8, 0.8, 0.8),
  d_emissiveColor(0.0, 0.0, 0.0),
  d_shininess(0.2),
  d_specularColor(0.0, 0.0, 0.0),
  d_transparency(0.0)
{
}

VrmlNodeMaterial::~VrmlNodeMaterial()
{
}


VrmlNode *VrmlNodeMaterial::cloneMe() const
{
  return new VrmlNodeMaterial(*this);
}


VrmlNodeMaterial* VrmlNodeMaterial::toMaterial() const
{ return (VrmlNodeMaterial*) this; }


ostream& VrmlNodeMaterial::printFields(ostream& os, int indent)
{
  if (! FPEQUAL(d_ambientIntensity.get(), 0.2))
    PRINT_FIELD(ambientIntensity);

  if (! FPEQUAL(d_diffuseColor.r(), 0.8) ||
      ! FPEQUAL(d_diffuseColor.g(), 0.8) ||
      ! FPEQUAL(d_diffuseColor.b(), 0.8) )
    PRINT_FIELD(diffuseColor);

  if (! FPZERO(d_emissiveColor.r()) ||
      ! FPZERO(d_emissiveColor.g()) ||
      ! FPZERO(d_emissiveColor.b()) )
    PRINT_FIELD(emissiveColor);

  if (! FPEQUAL(d_shininess.get(), 0.2))
    PRINT_FIELD(shininess);

  if (! FPZERO(d_specularColor.r()) ||
      ! FPZERO(d_specularColor.g()) ||
      ! FPZERO(d_specularColor.b()) )
    PRINT_FIELD(specularColor);

  if (! FPZERO(d_transparency.get()) )
      PRINT_FIELD(transparency);

  return os;
}

// This currently isn't used - see VrmlNodeAppearance.cpp.

void VrmlNodeMaterial::render(Viewer *viewer, VrmlRenderContext rc)
{
  viewer->setMaterial(d_ambientIntensity.get(),
		      d_diffuseColor.get(),
		      d_emissiveColor.get(),
		      d_shininess.get(),
		      d_specularColor.get(),
		      d_transparency.get());
  clearModified();
}


// Get the value of a field or eventOut.

const VrmlField *VrmlNodeMaterial::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "ambientIntensity" ) == 0 )
    return &d_ambientIntensity;
  else if ( strcmp( fieldName, "diffuseColor" ) == 0 )
    return &d_diffuseColor;
  else if ( strcmp( fieldName, "emissiveColor" ) == 0 )
    return &d_emissiveColor;
  else if ( strcmp( fieldName, "shininess" ) == 0 )
    return &d_shininess;
  else if ( strcmp( fieldName, "specularColor" ) == 0 )
    return &d_specularColor;
  else if ( strcmp( fieldName, "transparency" ) == 0 )
    return &d_transparency;
  
  return VrmlNode::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeMaterial::setField(const char *fieldName,
				const VrmlField &fieldValue)
{
  if TRY_FIELD(ambientIntensity, SFFloat)
  else if TRY_FIELD(diffuseColor, SFColor)
  else if TRY_FIELD(emissiveColor, SFColor)
  else if TRY_FIELD(shininess, SFFloat)
  else if TRY_FIELD(specularColor, SFColor)
  else if TRY_FIELD(transparency, SFFloat)
  else
    VrmlNode::setField(fieldName, fieldValue);
}
