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

#include "VrmlNodeCollision.h"
#include "MathUtils.h"
#include "VrmlNodeType.h"


static VrmlNode *creator(VrmlScene *s) { return new VrmlNodeCollision(s); }


// Define the built in VrmlNodeType:: "Collision" fields

VrmlNodeType *VrmlNodeCollision::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Collision", creator);
      t->reference();
    }

  VrmlNodeGroup::defineType(t);	// Parent class
  t->addExposedField("collide", VrmlField::SFBOOL);
  t->addField("proxy", VrmlField::SFNODE);
  t->addEventOut("collideTime", VrmlField::SFTIME);

  return t;
}

VrmlNodeType & VrmlNodeCollision::nodeType() const
{
    return *defineType(0);
}


VrmlNodeCollision::VrmlNodeCollision(VrmlScene *scene) :
  VrmlNodeGroup(scene),
  d_collide(true)
{
}

VrmlNodeCollision::~VrmlNodeCollision()
{
}


VrmlNode *VrmlNodeCollision::cloneMe() const
{
  return new VrmlNodeCollision(*this);
}

void VrmlNodeCollision::cloneChildren(VrmlNamespace *ns)
{
  VrmlNodeGroup::cloneChildren(ns);
  if (d_proxy.get())
    d_proxy.set(d_proxy.get()->clone(ns));
}

bool VrmlNodeCollision::isModified() const
{
  return ( (d_proxy.get() && d_proxy.get()->isModified()) ||
	   VrmlNodeGroup::isModified() );
}

void VrmlNodeCollision::clearFlags()
{
  VrmlNodeGroup::clearFlags();
  if (d_proxy.get()) d_proxy.get()->clearFlags();
}


void VrmlNodeCollision::addToScene( VrmlScene *s, const char *rel )
{
  VrmlNodeGroup::addToScene(s, rel);
  if (d_proxy.get()) d_proxy.get()->addToScene(s, rel);
}

// Copy the routes to nodes in the given namespace.

void VrmlNodeCollision::copyRoutes( VrmlNamespace *ns ) const
{
  VrmlNodeGroup::copyRoutes(ns);
  if (d_proxy.get()) d_proxy.get()->copyRoutes(ns);
}

ostream& VrmlNodeCollision::printFields(ostream& os, int indent)
{
  if (! d_collide.get()) PRINT_FIELD(collide);
  if (d_proxy.get()) PRINT_FIELD(proxy);

  VrmlNodeGroup::printFields(os, indent);
  return os;
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeCollision::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "collide" ) == 0 )
    return &d_collide;

  // eventOuts
  else if ( strcmp( fieldName, "collideTime" ) == 0 )
    return &d_collideTime;

  return VrmlNodeGroup::getField( fieldName );
}


// Set the value of one of the node fields.

void VrmlNodeCollision::setField(const char *fieldName,
				 const VrmlField &fieldValue)
{
  if TRY_FIELD(collide, SFBool)
  else if TRY_SFNODE_FIELD(proxy, Child)
  else
    VrmlNodeGroup::setField(fieldName, fieldValue);
}
