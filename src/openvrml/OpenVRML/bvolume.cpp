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

# include <float.h>
# include "private.h"
# include "bvolume.h"
# include "field.h"
# include "VrmlMatrix.h"
# include "VrmlFrustum.h"
# include "MathUtils.h"

namespace OpenVRML {

using namespace OpenVRML_;

/**
 * @class BVolume
 *
 * @brief A bounding volume.
 *
 * All the geometry in a scene maintains a bounding volume to help speed up
 * rendering and picking. Although currently we're just using spheres, the plan
 * is to eventually use tighter bounds like axis aligned boxes around nodes
 * that are expected to be static. That probably means boxes for geometry and
 * spheres for grouping nodes.
 *
 * @see Node::render
 * @see BSphere
 * @see AABox
 */

/**
 * @var BVolume::BV_INSIDE
 *
 * @brief Results of an intersection; indicates that the tested volume is
 *      entirely inside the target volume.
 */

/**
 * @var BVolume::BV_OUTSIDE
 *
 * @brief Results of an intersection; indicates that the tested volume is
 *      entirely outside the target volume.
 */

/**
 * @var BVolume::BV_PARTIAL
 *
 * @brief Results of an intersection; indicates that the tested volume
 *      intersects with the target volume.
 */

/**
 * @brief Destructor.
 */
BVolume::~BVolume() {}

/**
 * @fn bool BVolume::isMAX() const
 *
 * @brief Gets the MAX flag.
 *
 * The convention is that nodes that should be rendered unconditionally set a
 * MAX bvolume, ensuring that the branch they are on does not get pruned during
 * culling. Stuff like the picking code needs a way to differentiate this from
 * just a really big bounding volume, or an unset bounding volume.
 *
 * @return the max flag
 *
 * @see setMax
 */

/**
 * @fn void BVolume::setMAX()
 *
 * @brief Sets the MAX flag.
 *
 * @see isMAX
 */

/**
 * @fn int BVolume::isectFrustum(const VrmlFrustum & f) const
 *
 * @brief Intersect this bvolume with a frustum.
 *
 * The test assumes that the frustum is in the canonical
 * looking-down-negative-z orientation, so the bounding volume is going to have
 * to be transformed into the frustum's space. (Alternatives include
 * transforming the frustum into the bvolume's space, or transforming both of
 * them into the projection space. Lots of tradeoffs involved, but transforming
 * the bvolume is probably the simplest approach overall.)
 *
 * @return one of BV_INSIDE, BV_OUTSIDE, or BV_PARTIAL
 *
 * @see BVolume::transform
 * @see BVolume::orthoTransform
 */

/**
 * @fn void BVolume::extend(const BVolume & b)
 *
 * @brief Extend this bvolume to enclose the given bvolume.
 *
 * This is tricky, because C++ doesn't provide us a way to figure out exactly
 * what sort of bvolume we have been passed, yet we have to know in order to do
 * the appropriate math. What we really need is double dispatch but C++ does
 * not provide it.
 *
 * <p>What the implementation will probably do is use the toBSphere and toAABox
 * methods to test for the actual type of the parameter, and redispatch to
 * extend(sphere) or extend(box).  Alternatively, we could use the double
 * dispatch pattern as described in the Gang of Four patterns book.
 *
 * <p>We need this because nodes like Group don't know until runtime exactly
 * what sort of bounding volumes their children will have. Group could test
 * using toBSphere and toAABox, but it's better to centralize ugly stuff like
 * that.
 *
 * @param b a bounding volume of unknown type
 */

/**
 * @fn void BVolume::extend(const float p[3])
 *
 * @brief Extend this bvolume to enclose the given point.
 *
 * @param p a point
 */

/**
 * @fn void BVolume::extend(const AABox & b)
 *
 * @brief Extend this bvolume to enclose the given box.
 *
 * @param b an axis-aligned box
 */

/**
 * @fn void BVolume::extend(const BSphere & b)
 *
 * @brief Extend this bvolume to enclose the given sphere.
 *
 * @param b a bounding sphere
 */

/**
 * @fn void BVolume::enclose(const float * p, int n)
 *
 * @brief Enclose the given set of points.
 *
 * This resets the volume from any previous values.
 *
 * @param p array of floats, each set of 3 represents a point
 * @param n number of points (not number of floats!)
 */

/**
 * @fn void BVolume::orthoTransform(const VrmlMatrix & M)
 *
 * @brief Transform this bounding volume using an orthogonal transfom.
 *
 * Orthogonal transformations preserve angles. They include
 * translation, rotation, and uniform scaling. It turns out to be
 * so easy to transform bounding spheres by orthogonal
 * transformations that it's worth special casing. The caller is
 * responsible for assuring that the transformation is in fact
 * orthogonal, otherwise the results are undefined. If in doubt,
 * call <code>transform</code> instead and take the speed hit.
 *
 * @param M orthonormal transformation matrix in VrmlMatrix format
 *
 * @see MathUtils
 */

/**
 * @fn void BVolume::transform(const VrmlMatrix & M)
 *
 * @brief Transform this bounding volume using an affine transfom.
 *
 * Affine transformations can include nonuniform scaling. It is much
 * messier to deal with them, but VRML allows nonuniform scaling, so
 * we have to handle it. Note that since all orthogonal transforms
 * are affine, it's safe to always call this routine instead of
 * <code>orthoTransform</code>, but it's likely to be slower.
 * The results are undefined if this routine is called with a
 * non-affine argument. Note that VRML Transform nodes only allow
 * affine transformations, so unless you're doing something tricky
 * this routine should always be safe.
 *
 * @param M affine transformation matrix in VrmlMatrix format
 *
 * @see MathUtils
 */

/**
 * @fn ostream & BVolume::dump(ostream & ostr) const = 0
 *
 * @brief Print a text representation to a stream.
 *
 * Suitable for debugging.
 *
 * @param ostr an open ostream
 *
 * @return the ostream, save a line writing the output operator
 */



/**
 * @class BSphere
 *
 * @brief A bounding sphere.
 *
 * Bounding spheres are very fast to intersect test, but they are fairly loose.
 * If asked to choose just one bounding volume, four out of five graphics
 * professionals surveyed chose the bounding sphere for their patients who
 * chewed bounding volumes.
 */

/**
 * @var BSphere::c
 *
 * @brief The center of the sphere.
 */

/**
 * @var BSphere::r
 *
 * @brief The radius of the sphere.
 */

BSphere::BSphere()
{
  reset();
}

BSphere::BSphere(const BSphere& rhs)
{
  r = rhs.r;
  c[0] = rhs.c[0];
  c[1] = rhs.c[1];
  c[2] = rhs.c[2];
}

BSphere::~BSphere()
{
}


void
BSphere::reset()
{
  r = -1.0f;
  c[0] = 0.0f;
  c[1] = 0.0f;
  c[2] = 0.0f;
}


static float
sphere_plane_distance(const BSphere & bs, const float N[3], float D)
{
  // r = Ax + By + Cz + D
  //
  const float* c = bs.getCenter();
  float d = N[0]*c[0] + N[1]*c[1] + N[2]*c[2] - D;
  return d;
}


int
BSphere::isectFrustum(const VrmlFrustum& frust) const
{
  //cout << "BSphere::isectFrustum():WARNING:not implemented" << endl;

  if (this->isMAX())
    return BVolume::BV_PARTIAL;

  if (this->r == -1.0f)
    return BVolume::BV_PARTIAL; // ???

  int code = BVolume::BV_INSIDE;

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
    return BVolume::BV_OUTSIDE;
  if (d < r)
    code = BVolume::BV_PARTIAL;


  // distance from the sphere center to the far plane. same logic as
  // above. this is all much easier with a diagram...
  //
  float zfar = -frust.z_far;
  d = c[2] - zfar;
  if (d < -r)
    return BVolume::BV_OUTSIDE;
  if (d < r)
    code = BVolume::BV_PARTIAL;


  // test against the top, the same logic will be used to test against
  // the other sides. we still want to find the distanct to the plane, but
  // unlike the near/far planes we have to use the dot product.
  //
  d = sphere_plane_distance(*this, frust.top_plane, frust.top_plane[3]);
  if (d < -r)
    return BVolume::BV_OUTSIDE;
  if (d < r)
    code = BVolume::BV_PARTIAL;


  d = sphere_plane_distance(*this, frust.bot_plane, frust.bot_plane[3]);
  if (d < -r)
    return BVolume::BV_OUTSIDE;
  if (d < r)
    code = BVolume::BV_PARTIAL;


  d = sphere_plane_distance(*this, frust.left_plane, frust.left_plane[3]);
  if (d < -r)
    return BVolume::BV_OUTSIDE;
  if (d < r)
    code = BVolume::BV_PARTIAL;


  d = sphere_plane_distance(*this, frust.right_plane, frust.right_plane[3]);
  if (d < -r)
    return BVolume::BV_OUTSIDE;
  if (d < r)
    code = BVolume::BV_PARTIAL;


  return code;

}

void
BSphere::extend(const BVolume & bv)
{
  const BSphere * bs = 0;
  const AABox * ab = 0;
  if ((bs = dynamic_cast<const BSphere *>(&bv))) {
    this->extend(*bs);
    return;
  }
  if ((ab = dynamic_cast<const AABox *>(&bv))) {
    this->extend(*ab);
    return;
  }
  // doing two tests isn't a big deal, if there get to be too many
  // more kinds of bounding volumes, move to a double dispatch
  // pattern.
}


void
BSphere::extend(const float p[3])
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
  //cout << "BSphere::extend(" << xn << "," << yn << "," << zn << ")" << dn << endl;

  if (fpzero(dn))
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


void BSphere::extend(const AABox & b) {
    std::cout << "BSphere::extend(VrmlAABox&):WARNING:not implemented"
              << std::endl;
}

void
BSphere::extend(const BSphere& b)
{
  //cout << "BSphere::extend(BSphere&)" << endl;
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
  //cout << "BSphere[" << this << "]::extend():0:";
  //cout << x0 << "," << y0 << ","<< z0 << ":"<< r0 << endl;

  float x1 = b.c[0];
  float y1 = b.c[1];
  float z1 = b.c[2];
  float r1 = b.r;
  //cout << "BSphere[" << this << "]::extend():1:";
  //cout << x1 << "," << y1 << ","<< z1 << ":"<< r1 << endl;

  float xn = x1-x0;
  float yn = y1-y0;
  float zn = z1-z0;
  //cout << "BSphere[" << this << "]::extend():n:";
  //cout << xn << "," << yn << ","<< zn << endl;
  float dn = sqrt(xn*xn+yn*yn+zn*zn);

  if (fpzero(dn))
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


  //cout << "BSphere[" << this << "]::extend():dn:" << dn << endl;
  float cr = (dn+r0+r1)/2.0;
  //cout << "BSphere[" << this << "]::extend():cr:" << cr << endl;
  float tmp = (cr-r0)/dn;
  //cout << "BSphere[" << this << "]::extend():tmp:" << tmp << endl;
  float cx = x0 + xn*tmp;
  float cy = y0 + yn*tmp;
  float cz = z0 + zn*tmp;
  //cout << "BSphere[" << this << "]::extend():f:";
  //cout << cx << "," << cy << ","<< cz << ":"<< cr << endl;

  this->r = cr;
  this->setCenter(cx, cy, cz);
}


void
BSphere::enclose(const float* p, int n)
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
BSphere::getCenter() const
{
  return c;
}

void
BSphere::setCenter(const SFVec3f & center)
{
  SFVec3f::ConstArrayReference centerVec = center.get();
  std::copy(centerVec, centerVec + 3, this->c);
}

void
BSphere::setCenter(float x, float y, float z)
{
  c[0] = x;
  c[1] = y;
  c[2] = z;
}

void
BSphere::setCenter(const float ac[3])
{
  c[0] = ac[0];
  c[1] = ac[1];
  c[2] = ac[2];
}


float
BSphere::getRadius() const
{
  return r;
}

void
BSphere::setRadius(float r)
{
  this->r = r;
}

void
BSphere::setMAX()
{
  r = FLT_MAX;
  c[0] = 0.0;
  c[1] = 0.0;
  c[2] = 0.0;
}

bool
BSphere::isMAX() const
{
  if (r==FLT_MAX)
    return true;
  return false;
}



void
BSphere::orthoTransform(const VrmlMatrix & M)
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
  M.multVecMatrix(old_c,new_c);
  c[0] = new_c[0];
  c[1] = new_c[1];
  c[2] = new_c[2];

  // uniform scale means we can pick any of the scale elements? wait:
  // can we really do this?
  float tmp_r[3] = { M[0][0], M[1][0], M[2][0] };
  float new_r = Vlength(tmp_r);
  r = new_r*r;
}


void
BSphere::transform(const VrmlMatrix & M)
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
  M.multVecMatrix(old_c,new_c);
  c[0] = new_c[0];
  c[1] = new_c[1];
  c[2] = new_c[2];

  float x_scale_v[3] = { M[0][0], M[1][0], M[2][0] };
  float y_scale_v[3] = { M[0][1], M[1][1], M[2][1] };
  float z_scale_v[3] = { M[0][2], M[1][2], M[2][2] };

  float scale_x = Vlength(x_scale_v);
  float scale_y = Vlength(y_scale_v);
  float scale_z = Vlength(z_scale_v);

  //cout << "BSphere::transform():";
  //cout << scale_x << "," << scale_y << "," << scale_z << endl;

  float max_scale = scale_x;
  if (scale_y > max_scale)
    max_scale = scale_y;
  if (scale_z > max_scale)
    max_scale = scale_z;

  //cout << "BSphere::transform():r=(" << r << "," << max_scale*r << ")" << endl;
  r = max_scale*r;
}

std::ostream & BSphere::dump(std::ostream & out) const {
    return out << "(" << c[0] << "," << c[1] << "," << c[2] << "):" << r;
}


/**
 * @class AABox
 *
 * @brief An axis-aligned bounding box.
 *
 * This class is currently just a placeholder.
 */

AABox::~AABox() {}

int AABox::isectFrustum(const VrmlFrustum& f) const {
    std::cout << "AABox::isectFrustum():WARNING:not implemented" << std::endl;
    return BVolume::BV_PARTIAL;
}


void
AABox::extend(const BVolume & bv)
{
  const BSphere * bs = 0;
  const AABox * ab = 0;
  if ((bs = dynamic_cast<const BSphere *>(&bv))) {
    this->extend(*bs);
    return;
  }
  if ((ab = dynamic_cast<const AABox *>(&bv))) {
    this->extend(*ab);
    return;
  }
}


void
AABox::extend(const float p[3])
{
}

void
AABox::extend(const AABox& b)
{
}

void
AABox::extend(const BSphere& b)
{
}

void
AABox::enclose(const float* p, int n)
{
}

void
AABox::setMAX()
{
}

bool
AABox::isMAX() const
{
  return true;
}

void
AABox::orthoTransform(const VrmlMatrix & M)
{
}

void
AABox::transform(const VrmlMatrix & M)
{
}

std::ostream & AABox::dump(std::ostream & out) const {
    return out << "<AABox:not implemented>" << std::endl;
}

} // namespace OpenVRML
