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

#include "VrmlNodeWorldInfo.h"
#include "VrmlNodeType.h"
#include "VrmlScene.h"


//  WorldInfo factory.

static VrmlNode *creator(VrmlScene *s)
{
  return new VrmlNodeWorldInfo(s);
}

// Define the built in VrmlNodeType:: "WorldInfo" fields

VrmlNodeType *VrmlNodeWorldInfo::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("WorldInfo", creator);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addField("info", VrmlField::MFSTRING);
  t->addField("title", VrmlField::SFSTRING);

  return t;
}

VrmlNodeType & VrmlNodeWorldInfo::nodeType() const
{
    return *defineType(0);
}


VrmlNodeWorldInfo::VrmlNodeWorldInfo(VrmlScene *scene) :
  VrmlNodeChild(scene)
{
}

VrmlNodeWorldInfo::~VrmlNodeWorldInfo()
{
}

VrmlNode *VrmlNodeWorldInfo::cloneMe() const
{
  return new VrmlNodeWorldInfo(*this);
}


ostream& VrmlNodeWorldInfo::printFields(ostream& os, int indent)
{
  if (d_title.get()) PRINT_FIELD(title);
  if (d_info.size() > 0) PRINT_FIELD(info);

  return os;
}

// Set the value of one of the node fields.

void VrmlNodeWorldInfo::setField(const char *fieldName,
				 const VrmlField &fieldValue)
{
  if TRY_FIELD(info, MFString)
  else if TRY_FIELD(title, SFString)
  else
    VrmlNode::setField(fieldName, fieldValue);
}
