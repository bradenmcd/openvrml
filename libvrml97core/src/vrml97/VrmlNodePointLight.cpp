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

#include "VrmlNodePointLight.h"
#include "MathUtils.h"
#include "VrmlNodeType.h"
#include "VrmlScene.h"
#include "Viewer.h"

// Return a new VrmlNodePointLight
static VrmlNode *creator( VrmlScene *s ) { return new VrmlNodePointLight(s); }


// Define the built in VrmlNodeType:: "PointLight" fields

VrmlNodeType *VrmlNodePointLight::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("PointLight", creator);
      t->reference();
    }

  VrmlNodeLight::defineType(t);	// Parent class
  t->addExposedField("attenuation", VrmlField::SFVEC3F);
  t->addExposedField("location", VrmlField::SFVEC3F);
  t->addExposedField("radius", VrmlField::SFFLOAT);

  return t;
}

VrmlNodeType & VrmlNodePointLight::nodeType() const
{
    return *defineType(0);
}


VrmlNodePointLight::VrmlNodePointLight(VrmlScene *scene) :
  VrmlNodeLight(scene),
  d_attenuation(1.0, 0.0, 0.0),
  d_location(0.0, 0.0, 0.0),
  d_radius(100)
{
  if (d_scene) d_scene->addScopedLight(this);
}

VrmlNodePointLight::~VrmlNodePointLight()
{
  if (d_scene) d_scene->removeScopedLight(this);
}


VrmlNode *VrmlNodePointLight::cloneMe() const
{
  return new VrmlNodePointLight(*this);
}


VrmlNodePointLight* VrmlNodePointLight::toPointLight() const
{ return (VrmlNodePointLight*) this; }

void VrmlNodePointLight::addToScene(VrmlScene *s, const char *)
{ if (d_scene != s && (d_scene = s) != 0) d_scene->addScopedLight(this); }


ostream& VrmlNodePointLight::printFields(ostream& os, int indent)
{
  VrmlNodeLight::printFields(os, indent);
  if (! FPEQUAL(d_attenuation.x(), 1.0) ||
      ! FPZERO(d_attenuation.y()) ||
      ! FPZERO(d_attenuation.z()) )
    PRINT_FIELD(attenuation);
  if (! FPZERO(d_location.x()) ||
      ! FPZERO(d_location.y()) ||
      ! FPZERO(d_location.z()) )
    PRINT_FIELD(location);
  if (! FPEQUAL(d_radius.get(), 100.0))
    PRINT_FIELD(radius);

  return os;
}

// This should be called before rendering any geometry nodes in the scene.
// Since this is called from Scene::render() before traversing the
// scene graph, the proper transformation matrix hasn't been set up.
// Somehow it needs to figure out the accumulated xforms of its
// parents and apply them before rendering. This is not easy with
// DEF/USEd nodes...

void VrmlNodePointLight::renderScoped(Viewer *viewer)
{
  if ( d_on.get() && d_radius.get() > 0.0 )
    viewer->insertPointLight( d_ambientIntensity.get(),
			      d_attenuation.get(),
			      d_color.get(),
			      d_intensity.get(),
			      d_location.get(),
			      d_radius.get() );
  clearModified();
}


// Get the value of a field or eventOut.

const VrmlField *VrmlNodePointLight::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "attenuation" ) == 0 )
    return &d_attenuation;
  else if ( strcmp( fieldName, "location" ) == 0 )
    return &d_location;
  else if ( strcmp( fieldName, "radius" ) == 0 )
    return &d_radius;

  return VrmlNodeLight::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodePointLight::setField(const char *fieldName,
				  const VrmlField &fieldValue)
{
  if TRY_FIELD(attenuation, SFVec3f)
  else if TRY_FIELD(location, SFVec3f)
  else if TRY_FIELD(radius, SFFloat)
  else
    VrmlNodeLight::setField(fieldName, fieldValue);
}

const VrmlSFVec3f& VrmlNodePointLight::getAttenuation() const   // LarryD Mar 04/99
{  return d_attenuation; }

const VrmlSFVec3f& VrmlNodePointLight::getLocation() const   // LarryD Mar 04/99
{  return d_location; }
