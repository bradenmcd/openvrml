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

#include "VrmlNodeExtrusion.h"
#include "VrmlNodeType.h"
#include "Viewer.h"


static VrmlNode *creator( VrmlScene *s ) { return new VrmlNodeExtrusion(s); }


// Define the built in VrmlNodeType:: "Extrusion" fields

VrmlNodeType *VrmlNodeExtrusion::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Extrusion", creator);
      t->reference();
    }

  VrmlNodeGeometry::defineType(t);	// Parent class

  t->addEventIn("set_crossSection", VrmlField::MFVEC2F);
  t->addEventIn("set_orientation", VrmlField::MFROTATION);
  t->addEventIn("set_scale", VrmlField::MFVEC2F);
  t->addEventIn("set_spine", VrmlField::MFVEC3F);

  t->addField("beginCap", VrmlField::SFBOOL);
  t->addField("ccw", VrmlField::SFBOOL);
  t->addField("convex", VrmlField::SFBOOL);
  t->addField("creaseAngle", VrmlField::SFFLOAT);
  t->addField("crossSection", VrmlField::MFVEC2F);
  t->addField("endCap", VrmlField::SFBOOL);
  t->addField("orientation", VrmlField::MFROTATION);
  t->addField("scale", VrmlField::MFVEC2F);
  t->addField("solid", VrmlField::SFBOOL);
  t->addField("spine", VrmlField::MFVEC3F);

  return t;
}


VrmlNodeType & VrmlNodeExtrusion::nodeType() const
{
    return *defineType(0);
}


VrmlNodeExtrusion::VrmlNodeExtrusion(VrmlScene * scene):
        VrmlNodeGeometry(scene), d_beginCap(true), d_ccw(true), d_convex(true),
        d_creaseAngle(0), d_crossSection(5), d_endCap(true), d_orientation(1),
        d_scale(1), d_solid(true), d_spine(2) {
    this->d_crossSection[0][0] = 1.0f;
    this->d_crossSection[0][1] = 1.0f;
    this->d_crossSection[1][0] = 1.0f;
    this->d_crossSection[1][1] = -1.0f;
    this->d_crossSection[2][0] = -1.0f;
    this->d_crossSection[2][1] = -1.0f;
    this->d_crossSection[3][0] = -1.0f;
    this->d_crossSection[3][1] = 1.0f;
    this->d_crossSection[4][0] = 1.0f;
    this->d_crossSection[4][1] = 1.0f;
    
    this->d_scale[0][0] = 1.0f;
    this->d_scale[0][1] = 1.0f;
    
    this->d_spine[0][0] = 0.0f;
    this->d_spine[0][1] = 0.0f;
    this->d_spine[0][2] = 0.0f;
    this->d_spine[1][0] = 0.0f;
    this->d_spine[1][1] = 1.0f;
    this->d_spine[1][2] = 0.0f;
}

VrmlNodeExtrusion::~VrmlNodeExtrusion()
{
}


VrmlNode *VrmlNodeExtrusion::cloneMe() const
{
  return new VrmlNodeExtrusion(*this);
}


ostream& VrmlNodeExtrusion::printFields(ostream& os, int indent)
{
  if (! d_beginCap.get()) PRINT_FIELD(beginCap);
  if (! d_endCap.get()) PRINT_FIELD(endCap);
  if (! d_ccw.get()) PRINT_FIELD(ccw);
  if (! d_convex.get()) PRINT_FIELD(convex);
  if (! d_solid.get()) PRINT_FIELD(solid);

  if (d_creaseAngle.get() != 0.0) PRINT_FIELD(creaseAngle);
  PRINT_FIELD(crossSection);
  PRINT_FIELD(orientation);
  PRINT_FIELD(scale);
  PRINT_FIELD(spine);

  return os;
}


Viewer::Object VrmlNodeExtrusion::insertGeometry(Viewer *viewer, VrmlRenderContext rc)
{
  Viewer::Object obj = 0;
  if ( d_crossSection.getLength() > 0 &&
       d_spine.getLength() > 1 )
    {
      unsigned int optMask = 0;
      if (d_ccw.get()) optMask |= Viewer::MASK_CCW;
      if (d_convex.get()) optMask |= Viewer::MASK_CONVEX;
      if (d_solid.get()) optMask |= Viewer::MASK_SOLID;
      if (d_beginCap.get()) optMask |= Viewer::MASK_BOTTOM;
      if (d_endCap.get()) optMask |= Viewer::MASK_TOP;

      obj = viewer->insertExtrusion(optMask,
				    d_orientation.getLength(),
				    d_orientation.get(),
				    d_scale.getLength(),
				    d_scale.get(),
				    d_crossSection.getLength(),
				    d_crossSection.get(),
				    d_spine.getLength(),
				    d_spine.get());
    }

  return obj;
}


// Set the value of one of the node fields.

void VrmlNodeExtrusion::setField(const char *fieldName,
				 const VrmlField &fieldValue)
{
  if TRY_FIELD(beginCap, SFBool)
  else if TRY_FIELD(ccw, SFBool)
  else if TRY_FIELD(convex, SFBool)
  else if TRY_FIELD(creaseAngle, SFFloat)
  else if TRY_FIELD(crossSection, MFVec2f)
  else if TRY_FIELD(endCap, SFBool)
  else if TRY_FIELD(orientation, MFRotation)
  else if TRY_FIELD(scale, MFVec2f)
  else if TRY_FIELD(solid, SFBool)
  else if TRY_FIELD(spine, MFVec3f)
  else
    VrmlNodeGeometry::setField(fieldName, fieldValue);
}

VrmlNodeExtrusion* VrmlNodeExtrusion::toExtrusion() const
{ return (VrmlNodeExtrusion*) this; }
