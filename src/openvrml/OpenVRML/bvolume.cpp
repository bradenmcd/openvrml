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
 * @var BVolume::inside
 *
 * @brief Results of an intersection; indicates that the tested volume is
 *      entirely inside the target volume.
 */

/**
 * @var BVolume::outside
 *
 * @brief Results of an intersection; indicates that the tested volume is
 *      entirely outside the target volume.
 */

/**
 * @var BVolume::partial
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
 * @fn BVolume::Intersection BVolume::intersectFrustum(const VrmlFrustum & frustum) const
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
 * @param frustum   the frustum.
 *
 * @return inside, outside, or partial.
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
 * @fn void BVolume::orthoTransform(const mat4f & M)
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
 * @param M orthonormal transformation matrix in mat4f format
 *
 * @see MathUtils
 */

/**
 * @fn void BVolume::transform(const mat4f & M)
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
 * @param M affine transformation matrix in mat4f format
 *
 * @see MathUtils
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
 * @var BSphere::center
 *
 * @brief The center of the sphere.
 */

/**
 * @var BSphere::radius
 *
 * @brief The radius of the sphere.
 */

BSphere::BSphere()
{
    this->reset();
}

BSphere::~BSphere()
{}

void BSphere::reset()
{
    this->radius = -1.0f;
    this->center = vec3f(0.0, 0.0, 0.0);
}


namespace {
    float sphere_plane_distance(const BSphere & bs, const float N[3], float D)
    {
        //
        // r = Ax + By + Cz + D
        //
        const vec3f & c = bs.getCenter();
        float d = N[0] * c[0] + N[1] * c[1] + N[2] * c[2] - D;
        return d;
    }
}

BVolume::Intersection
BSphere::intersectFrustum(const VrmlFrustum & frustum) const
{
    if (this->isMAX()) { return BVolume::partial; }
    if (this->radius == -1.0f) { return BVolume::partial; } // ???

    BVolume::Intersection code = BVolume::inside;

    //
    // Quick test against near/far planes since we know in the VRML97
    // viewing case they are parallel to the xy-plane. (VRML97 restricts
    // the view volume, but other systems might not; so be careful to
    // confirm the assumption before reusing this code).
    //

    //
    // Distance from the center of the sphere to the near plane.
    //
    float znear = -frustum.z_near;
    float d = znear - this->center.z();
    if (d < -this->radius) { return BVolume::outside; }
    if (d < this->radius) { code = BVolume::partial; }

    //
    // Distance from the sphere center to the far plane. Same logic as
    // above.
    //
    float zfar = -frustum.z_far;
    d = this->center.z() - zfar;
    if (d < -this->radius) { return BVolume::outside; }
    if (d < this->radius) { code = BVolume::partial; }

    //
    // Test against the top; the same logic will be used to test against
    // the other sides. We still want to find the distanct to the plane; but
    // unlike the near/far planes we have to use the dot product.
    //
    d = sphere_plane_distance(*this, frustum.top_plane, frustum.top_plane[3]);
    if (d < -this->radius) { return BVolume::outside; }
    if (d < this->radius) { code = BVolume::partial; }

    d = sphere_plane_distance(*this, frustum.bot_plane, frustum.bot_plane[3]);
    if (d < -this->radius) { return BVolume::outside; }
    if (d < this->radius) { code = BVolume::partial; }

    d = sphere_plane_distance(*this, frustum.left_plane, frustum.left_plane[3]);
    if (d < -this->radius) { return BVolume::outside; }
    if (d < this->radius) { code = BVolume::partial; }

    d = sphere_plane_distance(*this, frustum.right_plane, frustum.right_plane[3]);
    if (d < -this->radius) { return BVolume::outside; }
    if (d < this->radius) { code = BVolume::partial; }

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

void BSphere::extend(const vec3f & p)
{
    if (this->isMAX()) { return; }

    // if this bsphere isn't set yet, then just do an assign. what's it
    // mean to have a zero radius bsphere? is that going to mess
    // anything up (iow, do we ever divide by radius?)
    //
    if (this->radius == -1.0f) { // flag, not comparison
        this->radius = 0.0f;
        this->center = p;
        return;
    }

    // you know, we could probably just call extend(sphere) with a
    // radius of zero and it would work out the same.

    float x0 = this->center.x();
    float y0 = this->center.y();
    float z0 = this->center.z();
    float r0 = this->radius;

    float x1 = p.x();
    float y1 = p.y();
    float z1 = p.z();

    float xn = x1 - x0;
    float yn = y1 - y0;
    float zn = z1 - z0;
    float dn = sqrt(xn * xn + yn * yn + zn * zn);

    if (fpzero(dn)) { return; }

    if (dn < r0) {
        // point is inside sphere
        return;
    }

    float cr = (dn + r0) / 2.0;
    float tmp = (cr - r0) / dn;
    float cx = x0 + xn * tmp;
    float cy = y0 + yn * tmp;
    float cz = z0 + zn * tmp;

    this->radius = cr;
    this->center.x(cx);
    this->center.y(cy);
    this->center.z(cz);
}

/**
 * @todo Implement me!
 */
void BSphere::extend(const AABox & b) {
}

void BSphere::extend(const BSphere & b)
{
    if (this->isMAX()) { return; }

    if (b.isMAX()) {
        this->setMAX();
        return;
    }

    // if the other bsphere isn't set, ignore it?
    //
    if (b.radius == -1.0f) { return; }

    // if this bsphere isn't set yet, then just do an
    // assign.
    //
    if (this->radius == -1.0f) { // flag, not comparison
        *this = b;
        return;
    }

    // s0 = ((x0,y0,z0),r0)
    // s1 = ((x1,y1,z1),r1)

    float x0 = this->center.x();
    float y0 = this->center.y();
    float z0 = this->center.z();
    float r0 = this->radius;

    float x1 = b.center.x();
    float y1 = b.center.y();
    float z1 = b.center.z();
    float r1 = b.radius;

    float xn = x1 - x0;
    float yn = y1 - y0;
    float zn = z1 - z0;
    float dn = sqrt(xn * xn + yn * yn + zn * zn);

    if (fpzero(dn)) { return; }

    if (dn + r1 < r0) { // inside us, so no change
        return;
    }
    if (dn + r0 < r1) { // we're inside them...
        *this = b;
        return;
    }

    float cr = (dn + r0 + r1) / 2.0;
    float tmp = (cr - r0) / dn;
    float cx = x0 + xn * tmp;
    float cy = y0 + yn * tmp;
    float cz = z0 + zn * tmp;

    this->radius = cr;
    this->center = vec3f(cx, cy, cz);
}


void BSphere::enclose(const float* p, int n)
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

    if (n < 1) { return; }

    const float* min_p[3] = { &p[0], &p[0], &p[0] };
    const float* max_p[3] = { &p[0], &p[0], &p[0] };

    // find the 6 points with: minx, maxx, miny, maxy, minz, maxz
    //
    int i;
    for (i=1; i<n; ++i) {
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

    this->center.x((max_span0[0] + max_span1[0]) / 2.0);
    this->center.y((max_span0[1] + max_span1[2]) / 2.0);
    this->center.z((max_span0[2] + max_span1[2]) / 2.0);

    this->radius = float(sqrt(this->center.dot(this->center)));

    for (i=0; i<n; ++i) {
        this->extend(vec3f(p[i * 3], p[i * 3 + 1], p[i * 3 + 2]));
    }
}


const vec3f & BSphere::getCenter() const
{
    return this->center;
}

void BSphere::setCenter(const vec3f & center)
{
    this->center = center;
}

float BSphere::getRadius() const
{
    return this->radius;
}

void BSphere::setRadius(const float radius)
{
    this->radius = radius;
}

void BSphere::setMAX()
{
    this->radius = std::numeric_limits<float>::max();
    this->center = vec3f(0.0, 0.0, 0.0);
}

bool BSphere::isMAX() const
{
    if (this->radius == std::numeric_limits<float>::max()) { return true; }
    return false;
}



void BSphere::orthoTransform(const mat4f & M)
{
    using OpenVRML_::length;

    if (this->isMAX()) { return; }
    if (this->radius == -1) { return; }
    // ortho is easy: since we know it's uniform scaling, we can just
    // scale the radius and translate the center, and we're done.
    this->center = this->center * M;

    // uniform scale means we can pick any of the scale elements? wait:
    // can we really do this?
    this->radius *= vec3f(M[0][0], M[1][0], M[2][0]).length();
}


void BSphere::transform(const mat4f & M)
{
    using OpenVRML_::length;

    if (this->isMAX()) { return; }
    if (this->radius == -1) { return; }
    this->center = this->center * M;

    vec3f x_scale_v(M[0][0], M[1][0], M[2][0]);
    vec3f y_scale_v(M[0][1], M[1][1], M[2][1]);
    vec3f z_scale_v(M[0][2], M[1][2], M[2][2]);

    float scale_x = x_scale_v.length();
    float scale_y = y_scale_v.length();
    float scale_z = z_scale_v.length();

    float max_scale = scale_x;
    if (scale_y > max_scale) { max_scale = scale_y; }
    if (scale_z > max_scale) { max_scale = scale_z; }

    this->radius *= max_scale;
}


/**
 * @class AABox
 *
 * @brief An axis-aligned bounding box.
 *
 * @todo This class is currently just a placeholder.
 */

AABox::~AABox() {}

/**
 * @todo Implement me!
 */
BVolume::Intersection
AABox::intersectFrustum(const VrmlFrustum & frustum) const {
    return BVolume::partial;
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


void AABox::extend(const vec3f & p)
{}

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
AABox::orthoTransform(const mat4f & M)
{
}

void
AABox::transform(const mat4f & M)
{
}

} // namespace OpenVRML
