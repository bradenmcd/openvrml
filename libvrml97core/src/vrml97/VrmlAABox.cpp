/*
 * VrmlAABox.cpp
 *
 * Part of the libvrml97 core library.
 * 
 * Copyright (c) 2000 Christopher K. St. John
 * cstjohn@acm.org
 * 
 * see file COPYING for details of license.
 *
 */



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
