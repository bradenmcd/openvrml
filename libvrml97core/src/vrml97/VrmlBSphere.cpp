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

#include <float.h> // posix-sez: yes, others? FLT_MAX
#include <iostream.h>
#include "VrmlFrustum.h"
#include "VrmlAABox.h"
#include "VrmlBSphere.h"
#include "MathUtils.h"
#include "VrmlSFVec3f.h"



VrmlBSphere::VrmlBSphere()
{
  reset();
}

VrmlBSphere::VrmlBSphere(const VrmlBSphere& rhs)
{
  r = rhs.r;
  c[0] = rhs.c[0];
  c[1] = rhs.c[1];
  c[2] = rhs.c[2];
}

VrmlBSphere::~VrmlBSphere()
{
}


VrmlBSphere*
VrmlBSphere::toBSphere() const
{
  return (VrmlBSphere*)this;
}


VrmlAABox*
VrmlBSphere::toAABox() const
{
  return (VrmlAABox*)0;
}


void
VrmlBSphere::reset()
{
  r = -1.0f;
  c[0] = 0.0f;
  c[1] = 0.0f;
  c[2] = 0.0f;
}


static float
sphere_plane_distance(const VrmlBSphere& bs, const float N[3], float D)
{
  // r = Ax + By + Cz + D
  //
  const float* c = bs.getCenter();
  float d = N[0]*c[0] + N[1]*c[1] + N[2]*c[2] - D;
  return d;
}


int
VrmlBSphere::isectFrustum(const VrmlFrustum& frust) const
{
  //cout << "VrmlBSphere::isectFrustum():WARNING:not implemented" << endl;

  if (this->isMAX())
    return VrmlBVolume::BV_PARTIAL;

  if (this->r == -1.0f)
    return VrmlBVolume::BV_PARTIAL; // ???

  int code = VrmlBVolume::BV_INSIDE;

  // quick test against near/far planes since we know in the vrml97
  // viewing case they are parallel to the xy plane. (vrml97 restricts
  // the view volume, but other systems might not, so be careful to
  // confirm the assumption before reusing this code).
  // 

  // distance from the center of the sphere to the near plane.
  //
  float znear = -frust.z_near;
  float d = znear - c[2];
  if (d < -r)
    return VrmlBVolume::BV_OUTSIDE;
  if (d < r)
    code = VrmlBVolume::BV_PARTIAL;  


  // distance from the sphere center to the far plane. same logic as
  // above. this is all much easier with a diagram...
  //
  float zfar = -frust.z_far;
  d = c[3] - zfar;
  if (d < -r)
    return VrmlBVolume::BV_OUTSIDE;
  if (d < r)
    code = VrmlBVolume::BV_PARTIAL;  
  

  // test against the top, the same logic will be used to test against
  // the other sides. we still want to find the distanct to the plane, but
  // unlike the near/far planes we have to use the dot product.
  //
  d = sphere_plane_distance(*this, frust.top_plane, frust.top_plane[3]);
  if (d < -r)
    return VrmlBVolume::BV_OUTSIDE;
  if (d < r)
    code = VrmlBVolume::BV_PARTIAL;  


  d = sphere_plane_distance(*this, frust.bot_plane, frust.bot_plane[3]);
  if (d < -r)
    return VrmlBVolume::BV_OUTSIDE;
  if (d < r)
    code = VrmlBVolume::BV_PARTIAL;  


  d = sphere_plane_distance(*this, frust.left_plane, frust.left_plane[3]);
  if (d < -r)
    return VrmlBVolume::BV_OUTSIDE;
  if (d < r)
    code = VrmlBVolume::BV_PARTIAL;  


  d = sphere_plane_distance(*this, frust.right_plane, frust.right_plane[3]);
  if (d < -r)
    return VrmlBVolume::BV_OUTSIDE;
  if (d < r)
    code = VrmlBVolume::BV_PARTIAL;  


  return code;
  
}

void
VrmlBSphere::extend(const VrmlBVolume& bv)
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
  // doing two tests isn't a big deal, if there get to be too many
  // more kinds of bounding volumes, move to a double dispatch
  // pattern.
}


void 
VrmlBSphere::extend(const float p[3]) 
{

  if (this->isMAX())
    return;

  // if this bsphere isn't set yet, then just do an assign. what's it
  // mean to have a zero radius bsphere? is that going to mess
  // anything up (iow, do we ever divide by radius?)
  //
  if (this->r == -1.0f) { // flag, not comparison
    this->r = 0.0f;
    this->c[0] = p[0];
    this->c[1] = p[1];
    this->c[2] = p[2];
    return;
  }

  // you know, we could probably just call extend(sphere) with a
  // radius of zero and it would work out the same.

  float x0 = this->c[0];
  float y0 = this->c[1];
  float z0 = this->c[2];
  float r0 = this->r;
  
  float x1 = p[0];
  float y1 = p[1];
  float z1 = p[2];

  float xn = x1-x0;
  float yn = y1-y0;
  float zn = z1-z0;
  float dn = sqrt(xn*xn+yn*yn+zn*zn);
  //cout << "VrmlBSphere::extend(" << xn << "," << yn << "," << zn << ")" << dn << endl;

  if (FPZERO(dn))
    return;

  if (dn < r0) // point is inside sphere
    return;

  float cr = (dn+r0)/2.0;
  float tmp = (cr-r0)/dn;
  float cx = x0 + xn*tmp;
  float cy = y0 + yn*tmp;
  float cz = z0 + zn*tmp;

  this->r = cr;
  this->c[0] = cx;
  this->c[1] = cy;
  this->c[2] = cz;
}  


void
VrmlBSphere::extend(const VrmlAABox& b)
{
  cout << "VrmlBSphere::extend(VrmlAABox&):WARNING:not implemented" << endl;
}

void
VrmlBSphere::extend(const VrmlBSphere& b)
{
  //cout << "VrmlBSphere::extend(VrmlBSphere&)" << endl;
  //b.dump(cout);

  if (this->isMAX())
    return;

  if (b.isMAX()) {
    this->setMAX();
    return;
  }

  // if the other bsphere isn't set, ignore it?
  //
  if (b.getRadius()==-1.0f)
    return;

  // if this bsphere isn't set yet, then just do an
  // assign.
  //
  if (this->r == -1.0f) { // flag, not comparison
    this->r = b.r;
    this->c[0] = b.c[0];
    this->c[1] = b.c[1];
    this->c[2] = b.c[2];
    return;
  }

  // s0 = ((x0,y0,z0),r0)
  // s1 = ((x1,y1,z1),r1)

  float x0 = this->c[0];
  float y0 = this->c[1];
  float z0 = this->c[2];
  float r0 = this->r;
  //cout << "VrmlBSphere[" << this << "]::extend():0:";
  //cout << x0 << "," << y0 << ","<< z0 << ":"<< r0 << endl;

  float x1 = b.c[0];
  float y1 = b.c[1];
  float z1 = b.c[2];
  float r1 = b.r;
  //cout << "VrmlBSphere[" << this << "]::extend():1:";
  //cout << x1 << "," << y1 << ","<< z1 << ":"<< r1 << endl;

  float xn = x1-x0;
  float yn = y1-y0;
  float zn = z1-z0;
  //cout << "VrmlBSphere[" << this << "]::extend():n:";
  //cout << xn << "," << yn << ","<< zn << endl;
  float dn = sqrt(xn*xn+yn*yn+zn*zn);

  if (FPZERO(dn))
    return;

  if (dn + r1 < r0) { // inside us, so no change
    return;
  }
  if (dn + r0 < r1) { // we're inside them...
    this->r = b.r;
    this->c[0] = b.c[0];
    this->c[1] = b.c[1];
    this->c[2] = b.c[2];
    return;
  }


  //cout << "VrmlBSphere[" << this << "]::extend():dn:" << dn << endl;
  float cr = (dn+r0+r1)/2.0;
  //cout << "VrmlBSphere[" << this << "]::extend():cr:" << cr << endl;
  float tmp = (cr-r0)/dn;
  //cout << "VrmlBSphere[" << this << "]::extend():tmp:" << tmp << endl;
  float cx = x0 + xn*tmp;
  float cy = y0 + yn*tmp;
  float cz = z0 + zn*tmp;
  //cout << "VrmlBSphere[" << this << "]::extend():f:";
  //cout << cx << "," << cy << ","<< cz << ":"<< cr << endl;

  this->r = cr;
  this->setCenter(cx, cy, cz);
}


void
VrmlBSphere::enclose(const float* p, int n)
{
  // doing an extend() for each point is ok, but there are
  // faster algorithms reference "An Efficient Bounding Sphere"
  // Graphics Gems pg 301. it depends on being able to make multiple
  // passes over the points, so it isn't appropriate for incremental
  // updates, but it would be good here...
  // for(int i=0; i<n; ++i)
  // this->extend(&p[i*3]);
  //

  this->reset();

  if (n < 1) 
    return;

  const float* min_p[3] = { &p[0], &p[0], &p[0] };
  const float* max_p[3] = { &p[0], &p[0], &p[0] };
  
  // find the 6 points with: minx, maxx, miny, maxy, minz, maxz
  //
  int i;
  for(i=1; i<n; ++i) {
    const float* pi = &p[i*3];
    if (pi[0] < (min_p[0])[0]) min_p[0] = pi;
    if (pi[1] < (min_p[1])[1]) min_p[1] = pi;
    if (pi[2] < (min_p[2])[2]) min_p[2] = pi;

    if (pi[0] > (max_p[0])[0]) max_p[0] = pi;
    if (pi[1] > (max_p[1])[1]) max_p[1] = pi;
    if (pi[2] > (max_p[2])[2]) max_p[2] = pi;
  }

  // pick the two points most distant from one another
  //
  float span[3];
  Vdiff(span, max_p[0], min_p[0]);
  float dx = span[0]*span[0] + span[1]*span[1] + span[2]*span[2];
  Vdiff(span, max_p[1], min_p[1]);
  float dy = span[0]*span[0] + span[1]*span[1] + span[2]*span[2];
  Vdiff(span, max_p[2], min_p[2]);
  float dz = span[0]*span[0] + span[1]*span[1] + span[2]*span[2];

  const float* max_span0 = min_p[0];
  const float* max_span1 = max_p[0];
  float max_span_dist = dx;
  if (dy > max_span_dist) {
    max_span0 = min_p[1];
    max_span1 = max_p[1];
    max_span_dist = dy;
  }
  if (dz > max_span_dist) {
    max_span0 = min_p[2];
    max_span1 = max_p[2];
    max_span_dist = dz;
  }

  this->c[0] = (max_span0[0] + max_span1[0])/2.0;
  this->c[1] = (max_span0[1] + max_span1[2])/2.0;
  this->c[2] = (max_span0[2] + max_span1[2])/2.0;

  float rad_sq = c[0]*c[0] + c[1]*c[1] + c[2]*c[2];
  this->r = (float)sqrt((double)rad_sq);

  for(i=0; i<n; ++i)
    this->extend(&p[i*3]);
}


const float*
VrmlBSphere::getCenter() const
{
  return c;
}

void
VrmlBSphere::setCenter(const VrmlSFVec3f& center)
{
  c[0] = center.x();
  c[1] = center.y();
  c[2] = center.z();
}

void
VrmlBSphere::setCenter(float x, float y, float z)
{
  c[0] = x;
  c[1] = y;
  c[2] = z;
}

void
VrmlBSphere::setCenter(const float ac[3])
{
  c[0] = ac[0];
  c[1] = ac[1];
  c[2] = ac[2];
}


float
VrmlBSphere::getRadius() const
{
  return r;
}

void
VrmlBSphere::setRadius(float r)
{
  this->r = r;
}

void
VrmlBSphere::setMAX()
{
  r = FLT_MAX;
  c[0] = 0.0;
  c[1] = 0.0;
  c[2] = 0.0;
}

bool
VrmlBSphere::isMAX() const
{
  if (r==FLT_MAX) 
    return true;
  return false;
}



void
VrmlBSphere::orthoTransform(const double M[4][4]) 
{
  if (this->isMAX())
    return;
  if (this->r == -1)
    return;
  // ortho is easy: since we know it's uniform scaling, we can just
  // scale the radius and translate the center, and we're done.
  float new_c[3];
  float old_c[3];
  old_c[0] = c[0];
  old_c[1] = c[1];
  old_c[2] = c[2];
  VM(new_c, M, old_c);
  c[0] = new_c[0];
  c[1] = new_c[1];
  c[2] = new_c[2];

  // uniform scale means we can pick any of the scale elements? wait:
  // can we really do this? 
  float tmp_r[3] = { M[0][0], M[0][1], M[0][2] };
  float new_r = Vlength(tmp_r);
  r = new_r*r;
}


void
VrmlBSphere::transform(const double M[4][4])
{
  if (this->isMAX())
    return;
  if (this->r == -1)
    return;
  float new_c[3];
  float old_c[3];
  old_c[0] = c[0];
  old_c[1] = c[1];
  old_c[2] = c[2];
  VM(new_c, M, old_c);
  c[0] = new_c[0];
  c[1] = new_c[1];
  c[2] = new_c[2];

  float x_scale_v[3] = { M[0][0], M[0][1], M[0][2] };
  float y_scale_v[3] = { M[1][0], M[1][1], M[1][2] };
  float z_scale_v[3] = { M[2][0], M[2][1], M[2][2] };

  float scale_x = Vlength(x_scale_v);
  float scale_y = Vlength(y_scale_v);
  float scale_z = Vlength(z_scale_v);

  //cout << "VrmlBSphere::transform():";
  //cout << scale_x << "," << scale_y << "," << scale_z << endl;

  float max_scale = scale_x;
  if (scale_y > max_scale)
    max_scale = scale_y;
  if (scale_z > max_scale)
    max_scale = scale_z;

  //cout << "VrmlBSphere::transform():r=(" << r << "," << max_scale*r << ")" << endl;
  r = max_scale*r;
}



ostream&
VrmlBSphere::dump(ostream& o) const
{
  return cout << "(" << c[0] << "," << c[1] << "," << c[2] << "):" << r;
}
