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

#include "VrmlNodeNormal.h"
#include "VrmlNodeType.h"


static VrmlNode *creator( VrmlScene *s ) { return new VrmlNodeNormal(s); }


// Define the built in VrmlNodeType:: "Normal" fields

VrmlNodeType *VrmlNodeNormal::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Normal", creator);
      t->reference();
    }

  VrmlNode::defineType(t);	// Parent class
  t->addExposedField("vector", VrmlField::MFVEC3F);

  return t;
}


VrmlNodeType & VrmlNodeNormal::nodeType() const
{
    return *defineType(0);
}


VrmlNodeNormal::VrmlNodeNormal(VrmlScene *scene) : VrmlNode(scene)
{
}

VrmlNodeNormal::~VrmlNodeNormal()
{
}


VrmlNode *VrmlNodeNormal::cloneMe() const
{
  return new VrmlNodeNormal(*this);
}


VrmlNodeNormal* VrmlNodeNormal::toNormal() const
{ return (VrmlNodeNormal*) this; }


ostream& VrmlNodeNormal::printFields(ostream& os, int indent)
{
  if (d_vector.getLength() > 0) PRINT_FIELD(vector);
  return os;
}


// Get the value of a field or eventOut.

const VrmlField *VrmlNodeNormal::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "vector" ) == 0 )
    return &d_vector;

  return VrmlNode::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodeNormal::setField(const char *fieldName,
			      const VrmlField &fieldValue)
{
  if TRY_FIELD(vector, MFVec3f)
  else
    VrmlNode::setField(fieldName, fieldValue);
}
