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

#include "VrmlNodeBox.h"

#include "VrmlNodeType.h"
#include "MathUtils.h"
#include "Viewer.h"
#include "VrmlBSphere.h"


// Make a VrmlNodeBox

static VrmlNode *creator( VrmlScene *s ) { return new VrmlNodeBox(s); }


// Define the built in VrmlNodeType:: "Box" fields

VrmlNodeType *VrmlNodeBox::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;	// Define type only once.
      t = st = new VrmlNodeType("Box", creator);
      t->reference();
    }

  VrmlNodeGeometry::defineType(t);	// Parent class
  t->addField("size", VrmlField::SFVEC3F);

  return t;
}

VrmlNodeType & VrmlNodeBox::nodeType() const
{
    return *defineType(0);
}


VrmlNodeBox::VrmlNodeBox(VrmlScene *scene) :
  VrmlNodeGeometry(scene),
  d_size(2.0, 2.0, 2.0)
{
  setBVolumeDirty(true); // lazy calc of bvolume
}

VrmlNodeBox::~VrmlNodeBox()
{
}

VrmlNode *VrmlNodeBox::cloneMe() const
{
  return new VrmlNodeBox(*this);
}


ostream& VrmlNodeBox::printFields(ostream& os, int )
{
  if ( ! FPEQUAL(d_size.getX(), 2.0) ||
       ! FPEQUAL(d_size.getY(), 2.0) ||
       ! FPEQUAL(d_size.getZ(), 2.0) )
    os << " size " << d_size;
  return os;
}

Viewer::Object VrmlNodeBox::insertGeometry(Viewer *viewer, VrmlRenderContext rc)
{
  return viewer->insertBox(d_size.getX(), d_size.getY(), d_size.getZ());
}

// Set the value of one of the node fields.

void VrmlNodeBox::setField(const char *fieldName,
			   const VrmlField &fieldValue)
{
  if TRY_FIELD(size, SFVec3f)
  else
    VrmlNodeGeometry::setField(fieldName, fieldValue);
  setBVolumeDirty(true);
}

VrmlNodeBox* VrmlNodeBox::toBox() const //LarryD Mar 08/99
{ return (VrmlNodeBox*) this; }

const VrmlSFVec3f& VrmlNodeBox::getSize() const   // LarryD Mar 08/99
{  return d_size; }

const VrmlBVolume*
VrmlNodeBox::getBVolume() const
{
  //cout << "VrmlNodeBox::getBVolume()" << endl;
  //static VrmlBSphere* box_sphere = (VrmlBSphere*)0;
  //if (!box_sphere) 
  //box_sphere = new VrmlBSphere();
  if (this->isBVolumeDirty()) {
    float corner[3] = { d_size.getX()/2.0f, d_size.getY()/2.0f, d_size.getZ()/2.0f };
    float r = Vlength(corner);
    ((VrmlNodeBox*)this)->d_bsphere.setRadius(r);
    ((VrmlNodeBox*)this)->setBVolumeDirty(false); // logical const
  }
  //cout << "VrmlNodeBox::getBVolume():";
  //box_sphere->dump(cout) << endl;
  return &d_bsphere;
}
