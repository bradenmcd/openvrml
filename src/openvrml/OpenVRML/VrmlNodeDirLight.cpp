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

#include "VrmlNodeDirLight.h"
#include "VrmlNodeType.h"
#include "MathUtils.h"
#include "Viewer.h"

// Return a new VrmlNodeDirLight
static VrmlNode *creator( VrmlScene *s ) { return new VrmlNodeDirLight(s); }


// Define the built in VrmlNodeType:: "DirLight" fields

VrmlNodeType *VrmlNodeDirLight::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("DirectionalLight", creator);
      t->reference();
    }

  VrmlNodeLight::defineType(t);	// Parent class
  t->addExposedField("direction", VrmlField::SFVEC3F);

  return t;
}

VrmlNodeType & VrmlNodeDirLight::nodeType() const
{
    return *defineType(0);
}


VrmlNodeDirLight::VrmlNodeDirLight(VrmlScene *scene) :
  VrmlNodeLight(scene),
  d_direction(0.0, 0.0, -1.0)
{
}

VrmlNodeDirLight::~VrmlNodeDirLight()
{
}


VrmlNode *VrmlNodeDirLight::cloneMe() const
{
  return new VrmlNodeDirLight(*this);
}


ostream& VrmlNodeDirLight::printFields(ostream& os, int indent)
{
  VrmlNodeLight::printFields(os, indent);
  if (! FPZERO(d_direction.getX()) ||
      ! FPZERO(d_direction.getY()) ||
      ! FPEQUAL(d_direction.getZ(), -1.0) )
    PRINT_FIELD(direction);

  return os;
}

// This should be called before rendering any sibling nodes.

void VrmlNodeDirLight::render(Viewer *viewer, VrmlRenderContext rc)
{
  if ( d_on.get() )
    viewer->insertDirLight( d_ambientIntensity.get(),
			    d_intensity.get(),
			    d_color.get(),
			    d_direction.get() );
  clearModified();
}

// Get the value of one of the exposedFields

const VrmlField *VrmlNodeDirLight::getField(const char *fieldName) const
{
  if ( strcmp( fieldName, "direction" ) == 0 )
    return &d_direction;
  return VrmlNodeLight::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodeDirLight::setField(const char *fieldName,
				const VrmlField &fieldValue)
{
  if TRY_FIELD(direction, SFVec3f)
  else 
    VrmlNodeLight::setField(fieldName, fieldValue);
}

const VrmlSFVec3f& VrmlNodeDirLight::getDirection() const   // LarryD Mar 04/99
{  return d_direction; }

VrmlNodeDirLight *VrmlNodeDirLight::toDirLight() const // LarryD Mar 04/99
{ return (VrmlNodeDirLight*) this; }
