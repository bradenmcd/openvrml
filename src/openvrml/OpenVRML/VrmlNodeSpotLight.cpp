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

#include "VrmlNodeSpotLight.h"
#include "MathUtils.h"
#include "VrmlNodeType.h"
#include "VrmlNodeVisitor.h"
#include "VrmlScene.h"
#include "Viewer.h"

// Return a new VrmlNodeSpotLight
static VrmlNode *creator( VrmlScene *s ) { return new VrmlNodeSpotLight(s); }


// Define the built in VrmlNodeType:: "SpotLight" fields

VrmlNodeType *VrmlNodeSpotLight::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("SpotLight", creator);
      t->reference();
    }

  VrmlNodeLight::defineType(t);	// Parent class
  t->addExposedField("attenuation", VrmlField::SFVEC3F);
  t->addExposedField("beamWidth", VrmlField::SFFLOAT);
  t->addExposedField("cutOffAngle", VrmlField::SFFLOAT);
  t->addExposedField("direction", VrmlField::SFVEC3F);
  t->addExposedField("location", VrmlField::SFVEC3F);
  t->addExposedField("radius", VrmlField::SFFLOAT);

  return t;
}


VrmlNodeType & VrmlNodeSpotLight::nodeType() const
{
    return *defineType(0);
}


VrmlNodeSpotLight::VrmlNodeSpotLight(VrmlScene *scene) :
  VrmlNodeLight(scene),
  d_attenuation(1.0, 0.0, 0.0),
  d_beamWidth(1.570796),
  d_cutOffAngle(0.785398),
  d_direction(0.0, 0.0, -1.0),
  d_location(0.0, 0.0, 0.0),
  d_radius(100)
{
  if (d_scene) d_scene->addScopedLight(this);
}

VrmlNodeSpotLight::~VrmlNodeSpotLight()
{
  if (d_scene) d_scene->removeScopedLight(this);
}

bool VrmlNodeSpotLight::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodeSpotLight* VrmlNodeSpotLight::toSpotLight() const
{ return (VrmlNodeSpotLight*) this; }


void VrmlNodeSpotLight::addToScene(VrmlScene *s, const char *)
{ if (d_scene != s && (d_scene = s) != 0) d_scene->addScopedLight(this); }


ostream& VrmlNodeSpotLight::printFields(ostream& os, int indent)
{
  VrmlNodeLight::printFields(os, indent);
  if (! FPEQUAL(d_attenuation.getX(), 1.0) ||
      ! FPZERO(d_attenuation.getY()) ||
      ! FPZERO(d_attenuation.getZ()) )
    PRINT_FIELD(attenuation);
  if (! FPEQUAL(d_beamWidth.get(), 1.570796))
    PRINT_FIELD(beamWidth);

  if (! FPEQUAL(d_cutOffAngle.get(), 1.570796))
    PRINT_FIELD(cutOffAngle);
  if (! FPZERO(d_direction.getX()) ||
      ! FPZERO(d_direction.getY()) ||
      ! FPEQUAL(d_direction.getZ(), -1.0) )
    PRINT_FIELD(direction);

  if (! FPZERO(d_location.getX()) ||
      ! FPZERO(d_location.getY()) ||
      ! FPZERO(d_location.getZ()) )
    PRINT_FIELD(location);

  if (! FPEQUAL(d_radius.get(), 100.0))
    PRINT_FIELD(radius);

  return os;
}


// This should be called before rendering any geometry in the scene.
// Since this is called from Scene::render() before traversing the
// scene graph, the proper transformation matrix hasn't been set up.
// Somehow it needs to figure out the accumulated xforms of its
// parents and apply them before rendering. This is not easy with
// DEF/USEd nodes...
void VrmlNodeSpotLight::renderScoped(Viewer *viewer)
{
  if ( d_on.get() && d_radius.get() > 0.0 )
    viewer->insertSpotLight( d_ambientIntensity.get(),
			     d_attenuation.get(),
			     d_beamWidth.get(),
			     d_color.get(),
			     d_cutOffAngle.get(),
			     d_direction.get(),
			     d_intensity.get(),
			     d_location.get(),
			     d_radius.get() );
  clearModified();
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeSpotLight::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "attenuation" ) == 0 )
    return &d_attenuation;
  else if ( strcmp( fieldName, "beamWidth" ) == 0 )
    return &d_beamWidth;
  else if ( strcmp( fieldName, "cutOffAngle" ) == 0 )
    return &d_cutOffAngle;
  else if ( strcmp( fieldName, "direction" ) == 0 )
    return &d_direction;
  else if ( strcmp( fieldName, "location" ) == 0 )
    return &d_location;
  else if ( strcmp( fieldName, "radius" ) == 0 )
    return &d_radius;

  return VrmlNodeLight::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodeSpotLight::setField(const char *fieldName,
				 const VrmlField &fieldValue)
{
  if TRY_FIELD(attenuation, SFVec3f)
  else if TRY_FIELD(beamWidth, SFFloat)
  else if TRY_FIELD(cutOffAngle, SFFloat)
  else if TRY_FIELD(direction, SFVec3f)
  else if TRY_FIELD(location, SFVec3f)
  else if TRY_FIELD(radius, SFFloat)
  else 
    VrmlNodeLight::setField(fieldName, fieldValue);
}

const VrmlSFVec3f& VrmlNodeSpotLight::getAttenuation() const   // LarryD Mar 04/99
{  return d_attenuation; }

const VrmlSFVec3f& VrmlNodeSpotLight::getDirection() const   // LarryD Mar 04/99
{  return d_direction; }

const VrmlSFVec3f& VrmlNodeSpotLight::getLocation() const   // LarryD Mar 04/99
{  return d_location; }
