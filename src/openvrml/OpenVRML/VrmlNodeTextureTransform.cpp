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

#include "VrmlNodeTextureTransform.h"
#include "VrmlNodeType.h"
#include "VrmlNodeVisitor.h"
#include "MathUtils.h"
#include "Viewer.h"

static VrmlNode *creator( VrmlScene *s ) {
  return new VrmlNodeTextureTransform(s); 
}


// Define the built in VrmlNodeType:: "TextureTransform" fields

VrmlNodeType *VrmlNodeTextureTransform::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("TextureTransform", creator);
      t->reference();
    }

  VrmlNode::defineType(t);	// Parent class
  t->addExposedField("center", VrmlField::SFVEC2F);
  t->addExposedField("rotation", VrmlField::SFFLOAT);
  t->addExposedField("scale", VrmlField::SFVEC2F);
  t->addExposedField("translation", VrmlField::SFVEC2F);

  return t;
}

VrmlNodeType & VrmlNodeTextureTransform::nodeType() const 
{
    return *defineType(0);
}


VrmlNodeTextureTransform::VrmlNodeTextureTransform(VrmlScene *scene) :
  VrmlNode(scene),
  d_center(0.0, 0.0),
  d_rotation(0.0),
  d_scale(1.0, 1.0),
  d_translation(0.0, 0.0)
{
}

VrmlNodeTextureTransform::~VrmlNodeTextureTransform()
{
}

bool VrmlNodeTextureTransform::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodeTextureTransform* VrmlNodeTextureTransform::toTextureTransform() const 
{ return (VrmlNodeTextureTransform*) this; }


ostream& VrmlNodeTextureTransform::printFields(ostream& os, int indent)
{
  if (! FPZERO(d_center.getX()) ||
      ! FPZERO(d_center.getY()))
    PRINT_FIELD(center);

  if (! FPZERO(d_rotation.get()))
    PRINT_FIELD(rotation);

  if (! FPEQUAL(d_scale.getX(), 1.0) ||
      ! FPEQUAL(d_scale.getY(), 1.0))
    PRINT_FIELD(scale);
  if (! FPZERO(d_translation.getX()) ||
      ! FPZERO(d_translation.getY()))
    PRINT_FIELD(translation);
      
  return os;
}


void VrmlNodeTextureTransform::render(Viewer *viewer, VrmlRenderContext rc)
{
  viewer->setTextureTransform( d_center.get(),
			       d_rotation.get(),
			       d_scale.get(),
			       d_translation.get() );
  clearModified();
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeTextureTransform::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "center" ) == 0 )
    return &d_center;
  else if ( strcmp( fieldName, "rotation" ) == 0 )
    return &d_rotation;
  else if ( strcmp( fieldName, "scale" ) == 0 )
    return &d_scale;
  else if ( strcmp( fieldName, "translation" ) == 0 )
    return &d_translation;
  
  return VrmlNode::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeTextureTransform::setField(const char *fieldName,
					const VrmlField &fieldValue)
{
  if TRY_FIELD(center, SFVec2f)
  else if TRY_FIELD(rotation, SFFloat)
  else if TRY_FIELD(scale, SFVec2f)
  else if TRY_FIELD(translation, SFVec2f)
  else
    VrmlNode::setField(fieldName, fieldValue);
}

const VrmlSFVec2f & VrmlNodeTextureTransform::getCenter() const {
    return this->d_center;
}

const VrmlSFFloat & VrmlNodeTextureTransform::getRotation() const {
    return this->d_rotation;
}

const VrmlSFVec2f & VrmlNodeTextureTransform::getScale() const {
    return this->d_scale;
}

const VrmlSFVec2f & VrmlNodeTextureTransform::getTranslation() const {
    return this->d_translation;
}
