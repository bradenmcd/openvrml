
/*
 * VrmlBVolume.cpp
 *
 * Part of the libvrml97 core library.
 * 
 * Copyright (c) 2000 Christopher K. St. John
 * cstjohn@acm.org
 * 
 * see file COPYING for details of license.
 *
 */



#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "VrmlRenderContext.h"
#include "VrmlBVolume.h"


VrmlRenderContext::VrmlRenderContext()
{
  cull_flag = VrmlBVolume::BV_PARTIAL;
  M = (MatrixPtr)0;
  draw_bspheres = false;
}


VrmlRenderContext::VrmlRenderContext(int acull_flag, double aM[4][4])
{
  cull_flag = acull_flag;
  M = aM;
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
VrmlRenderContext::setMatrix(double aM[4][4])
{
  M = aM;
}


VrmlRenderContext::MatrixPtr
VrmlRenderContext::getMatrix()
{
  return M;
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
