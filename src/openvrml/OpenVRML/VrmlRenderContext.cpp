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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "VrmlRenderContext.h"
#include "VrmlBVolume.h"


VrmlRenderContext::VrmlRenderContext()
{
  cull_flag = VrmlBVolume::BV_PARTIAL;
  M = (VrmlMatrix *)0;
  draw_bspheres = false;
}


VrmlRenderContext::VrmlRenderContext(int acull_flag, VrmlMatrix & aM)
{
  cull_flag = acull_flag;
  M = &aM;
  draw_bspheres = false;
}


VrmlRenderContext::VrmlRenderContext(const VrmlRenderContext& rhs)
{
  cull_flag = rhs.cull_flag;
  M = rhs.M;
  draw_bspheres = rhs.draw_bspheres;
}


VrmlRenderContext::~VrmlRenderContext()
{
}


int
VrmlRenderContext::getCullFlag() const
{
  return cull_flag;
}


void
VrmlRenderContext::setCullFlag(int f)
{
  cull_flag = f;
}


void
VrmlRenderContext::setMatrix(VrmlMatrix & aM)
{
  M = &aM;
}

const VrmlMatrix&
VrmlRenderContext::getMatrix()const 
{
  return *(this->M);
}


bool
VrmlRenderContext::getDrawBSpheres()
{
  return draw_bspheres;
}

void
VrmlRenderContext::setDrawBSpheres(bool f)
{
  draw_bspheres = f;
}
