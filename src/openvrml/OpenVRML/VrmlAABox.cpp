//
// OpenVRML
//
// Copyright (C) 2000  Christopher K. St. John
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

#include <iostream.h>
#include "VrmlBSphere.h"
#include "VrmlFrustum.h"
#include "VrmlAABox.h"


VrmlBSphere*
VrmlAABox::toBSphere() const
{
  return (VrmlBSphere*)0;
}


VrmlAABox*
VrmlAABox::toAABox() const
{
  return (VrmlAABox*)this;
}

int
VrmlAABox::isectFrustum(const VrmlFrustum& f) const
{
  cout << "VrmlAABox::isectFrustum():WARNING:not implemented" << endl;
  return VrmlBVolume::BV_PARTIAL;
}


void
VrmlAABox::extend(const VrmlBVolume& bv)
{
  VrmlBSphere* bs = bv.toBSphere();
  if (bs) {
    this->extend(*bs);
    return;
  }
  VrmlAABox* ab = bv.toAABox();
  if (ab) {
    this->extend(*ab);
    return;
  }
}


void 
VrmlAABox::extend(const float p[3]) 
{
}

void
VrmlAABox::extend(const VrmlAABox& b)
{
}

void
VrmlAABox::extend(const VrmlBSphere& b)
{
}

void
VrmlAABox::enclose(const float* p, int n)
{
}

void
VrmlAABox::setMAX()
{
}

bool
VrmlAABox::isMAX() const
{
  return true;
}

void
VrmlAABox::orthoTransform(const double M[4][4]) 
{
}

void
VrmlAABox::transform(const double M[4][4])
{
}


ostream&
VrmlAABox::dump(ostream& o) const
{
  return cout << "<VrmlAABox:not implemented>" << endl;
}
