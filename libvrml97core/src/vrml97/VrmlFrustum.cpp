/*
 * VrmlFrustum.cpp
 *
 * Part of the libvrml97 core library.
 * 
 * Copyright (c) 2000 Christopher K. St. John
 * cstjohn@acm.org
 * 
 * see file COPYING for details of license.
 *
 */

#ifdef _WIN32
#include <winconfig.h>
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>         // tan(), M_PI (probably)
#include <iostream.h>     // ostream for dump
#include "MathUtils.h"    // Vcross(), Vnorm(), etc
#include "VrmlFrustum.h"



VrmlFrustum::VrmlFrustum()
{
  fovy = -1;
  fovx = -1;
  z_near = -1;
  z_far = -1;
}



VrmlFrustum::VrmlFrustum(float afovy, float aaspect, double anear, double afar)
{
  //cout << "VrmlFrustum::VrmlFrustum(" << afovy << "," << aaspect;
  //cout << "," << anear << "," << afar << ")" << endl;
  fovy = (afovy/360.0)*2.0*M_PI;
  float cy = (float)tan(fovy/2.0);
  fovx = 2.0*atan(cy*aaspect);
  z_near = anear;
  z_far = afar;
  update();
}



void
VrmlFrustum::update()
{
  // figure out the corners of the near clipping plane, then use the
  // vectors from the eyepoint to the four corners to figure out the
  // plane equations. the diagram shows how to get the x and z
  // coordinates, finding the y coordinates is analogous.
  // 
  //
  //   -x
  //    |        /
  //    |       /
  //    |      /
  //    |     . <-- find this point
  //    |    /:
  //    |   / :
  //    |  /  :
  //    | /   :
  //    |/ )--:----- angle is fovx/2
  //    --------------------- -z
  //    |<-n->|
  //    |    
  //   +x
  //
  // (finish the diagram later)
  //

  float cy = (float)tan(fovy/2.0);
  float cx = (float)tan(fovx/2.0);

  float NL = -z_near*cx;
  float NR = -NL;
  float NT =  z_near*cy;
  float NB = -NT;

  float tmp0[3];
  float tmp1[3];

  tmp0[0] = NL;
  tmp0[1] = NB;
  tmp0[2] = -z_near;

  tmp1[0] = NL;
  tmp1[1] = NT;
  tmp1[2] = -z_near;

  Vcross(left_plane, tmp0, tmp1);
  Vnorm(left_plane);
  left_plane[3] = 0;

  right_plane[0] = -left_plane[0];
  right_plane[1] = 0;
  right_plane[2] = left_plane[2];
  right_plane[3] = 0;

  tmp0[0] = NL;
  tmp0[1] = NT;
  tmp0[2] = -z_near;

  tmp1[0] = NR;
  tmp1[1] = NT;
  tmp1[2] = -z_near;
    
  Vcross(top_plane, tmp0, tmp1);
  Vnorm(top_plane);
  top_plane[3] = 0;

  bot_plane[0] = 0;
  bot_plane[1] = -top_plane[1];
  bot_plane[2] = top_plane[2];
  bot_plane[3] = 0;
}


ostream&
VrmlFrustum::dump(ostream& ostr) const
{
  ostr << "VrmlFrustum {" << endl;
  ostr << z_near << endl;
  ostr << z_far << endl;
  ostr << fovx*(360.0/(M_PI*2.0)) << endl;
  ostr << fovy*(360.0/(M_PI*2.0)) << endl;
  ostr << "}";
  return ostr;
}
