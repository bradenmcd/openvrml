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

#include "VrmlNodeCoordinate.h"
#include "VrmlNodeType.h"


static VrmlNode *creator( VrmlScene *s ) { return new VrmlNodeCoordinate(s); }


// Define the built in VrmlNodeType:: "Coordinate" fields

VrmlNodeType *VrmlNodeCoordinate::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Coordinate", creator);
      t->reference();
    }

  VrmlNode::defineType(t);	// Parent class
  t->addExposedField("point", VrmlField::MFVEC3F);

  return t;
}


VrmlNodeType & VrmlNodeCoordinate::nodeType() const
{
    return *defineType(0);
}


VrmlNodeCoordinate::VrmlNodeCoordinate(VrmlScene *scene) : VrmlNode(scene)
{
}

VrmlNodeCoordinate::~VrmlNodeCoordinate()
{
}


VrmlNode *VrmlNodeCoordinate::cloneMe() const
{
  return new VrmlNodeCoordinate(*this);
}


VrmlNodeCoordinate* VrmlNodeCoordinate::toCoordinate() const
{ return (VrmlNodeCoordinate*)this; }


ostream& VrmlNodeCoordinate::printFields(ostream& os, int indent)
{
  if (d_point.getLength() > 0) PRINT_FIELD(point);

  return os;
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeCoordinate::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "point" ) == 0 )
    return &d_point;

  return VrmlNode::getField( fieldName );
}


// Set the value of one of the node fields.

void VrmlNodeCoordinate::setField(const char *fieldName,
				  const VrmlField &fieldValue)
{
  if TRY_FIELD(point, MFVec3f)
  else
    VrmlNode::setField(fieldName, fieldValue);
}
