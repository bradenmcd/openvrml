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

#include "VrmlNodeLight.h"
#include "MathUtils.h"
#include "VrmlNodeType.h"



// Define the built in VrmlNodeType:: "Light" fields

VrmlNodeType *VrmlNodeLight::defineType(VrmlNodeType *t)
{
  VrmlNodeChild::defineType(t);	// Parent class
  t->addExposedField("ambientIntensity", VrmlField::SFFLOAT);
  t->addExposedField("color", VrmlField::SFCOLOR);
  t->addExposedField("intensity", VrmlField::SFFLOAT);
  t->addExposedField("on", VrmlField::SFBOOL);

  return t;
}


VrmlNodeType & VrmlNodeLight::nodeType() const
{
    return *defineType(0);
}


VrmlNodeLight::VrmlNodeLight(VrmlScene *scene) :
  VrmlNodeChild(scene),
  d_ambientIntensity(0.0),
  d_color(1.0, 1.0, 1.0),
  d_intensity(1.0),
  d_on(true)
{
}

VrmlNodeLight::~VrmlNodeLight()
{
}


VrmlNodeLight* VrmlNodeLight::toLight() const
{ return (VrmlNodeLight*) this; }


void VrmlNodeLight::renderScoped(Viewer *)
{
}


ostream& VrmlNodeLight::printFields(ostream& os, int indent)
{
  if (! FPZERO(d_ambientIntensity.get()))
    PRINT_FIELD(ambientIntensity);
  if (! FPEQUAL(d_color.getR(), 1.0) ||
      ! FPEQUAL(d_color.getG(), 1.0) ||
      ! FPEQUAL(d_color.getB(), 1.0) )
    PRINT_FIELD(color);
  if (! FPEQUAL(d_intensity.get(), 1.0))
    PRINT_FIELD(intensity);
  if (! d_on.get() ) PRINT_FIELD(on);

  return os;
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeLight::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "ambientIntensity" ) == 0 )
    return &d_ambientIntensity;
  else if ( strcmp( fieldName, "color" ) == 0 )
    return &d_color;
  else if ( strcmp( fieldName, "intensity" ) == 0 )
    return &d_intensity;
  else if ( strcmp( fieldName, "on" ) == 0 )
    return &d_on;

  return VrmlNode::getField( fieldName );
}


// Set the value of one of the node fields.

void VrmlNodeLight::setField(const char *fieldName,
			     const VrmlField &fieldValue)
{
  if TRY_FIELD(ambientIntensity, SFFloat)
  else if TRY_FIELD(color, SFColor)
  else if TRY_FIELD(intensity, SFFloat)
  else if TRY_FIELD(on, SFBool)
  else 
    VrmlNode::setField(fieldName, fieldValue);
}
