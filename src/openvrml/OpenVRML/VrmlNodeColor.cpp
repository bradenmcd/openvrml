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

#include "VrmlNodeColor.h"
#include "VrmlNodeVisitor.h"
#include "VrmlNodeType.h"


static VrmlNode *creator( VrmlScene *s ) { return new VrmlNodeColor(s); }


// Define the built in VrmlNodeType:: "Color" fields

VrmlNodeType *VrmlNodeColor::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;	// Only define type once.
      t = st = new VrmlNodeType("Color", creator);
      t->reference();
    }

  VrmlNode::defineType(t);	// Parent class
  t->addExposedField("color", VrmlField::MFCOLOR);

  return t;
}

VrmlNodeType & VrmlNodeColor::nodeType() const
{
    return *defineType(0);
}


VrmlNodeColor::VrmlNodeColor(VrmlScene *scene) : VrmlNode(scene)
{
}

VrmlNodeColor::~VrmlNodeColor()
{
}

bool VrmlNodeColor::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodeColor* VrmlNodeColor::toColor() const
{ return (VrmlNodeColor*) this; }


ostream& VrmlNodeColor::printFields(ostream& os, int indent)
{
  if (d_color.getLength() > 0) PRINT_FIELD(color);

  return os;
}


// Get the value of a field or eventOut.

const VrmlField *VrmlNodeColor::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "color" ) == 0 )
    return &d_color;
  return VrmlNode::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodeColor::setField(const char *fieldName,
			     const VrmlField &fieldValue)
{
  if TRY_FIELD(color, MFColor)
  else
    VrmlNode::setField(fieldName, fieldValue);
}
