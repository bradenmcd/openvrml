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

#include "VrmlNodeSphere.h"
#include "VrmlNodeType.h"
#include "Viewer.h"

#include "VrmlBSphere.h"


static VrmlNode *creator( VrmlScene *s ) { return new VrmlNodeSphere(s); }


// Define the built in VrmlNodeType:: "Sphere" fields

VrmlNodeType *VrmlNodeSphere::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Sphere", creator);
      t->reference();
    }

  VrmlNodeGeometry::defineType(t);	// Parent class
  t->addField("radius", VrmlField::SFFLOAT);

  return t;
}

VrmlNodeType & VrmlNodeSphere::nodeType() const
{
    return *defineType(0);
}


VrmlNodeSphere::VrmlNodeSphere(VrmlScene *scene) :
  VrmlNodeGeometry(scene),
  d_radius(1.0)
{
  setBVolumeDirty(true); // lazy calc of bvolume
}

VrmlNodeSphere::~VrmlNodeSphere()
{
}

VrmlNode *VrmlNodeSphere::cloneMe() const
{
  return new VrmlNodeSphere(*this);
}


ostream& VrmlNodeSphere::printFields(ostream& os, int )
{
  if (d_radius.get() != 1.0)
    os << "radius " << d_radius;
  return os;
}

Viewer::Object VrmlNodeSphere::insertGeometry(Viewer *viewer, VrmlRenderContext rc)
{
  return viewer->insertSphere(d_radius.get());
}

// Set the value of one of the node fields.

void VrmlNodeSphere::setField(const char *fieldName,
			      const VrmlField &fieldValue)
{
  if TRY_FIELD(radius, SFFloat)
  else
    VrmlNodeGeometry::setField(fieldName, fieldValue);
  setBVolumeDirty(true);
}

VrmlNodeSphere* VrmlNodeSphere::toSphere() const //LarryD Mar 08/99
{ return (VrmlNodeSphere*) this; }


const VrmlBVolume*
VrmlNodeSphere::getBVolume() const
{
  //cout << "VrmlNodeSphere::getBVolume()" << endl;
  //static VrmlBSphere* sphere_sphere = new VrmlBSphere();
  if (this->isBVolumeDirty()) {
    ((VrmlNodeSphere*)this)->d_bsphere.setRadius(d_radius.get());
    ((VrmlNode*)this)->setBVolumeDirty(false); // logical const
    //cout << "VrmlNodeSphere::getBVolume():recalc:";
  }
  //else 
  //cout << "VrmlNodeSphere::getBVolume():clean :";
  //b_bsphere->dump(cout) << endl;
  return &d_bsphere;
}

