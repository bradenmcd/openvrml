// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
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

# include <limits>
# include <private.h>
# include "bounding_volume.h"
# include "field_value.h"
# include "frustum.h"

/**
 * @class openvrml::bounding_volume
 *
 * @brief A bounding volume.
 *
 * All the geometry in a scene maintains a bounding volume to help speed up
 * rendering and picking. Although currently we're just using spheres, the plan
 * is to eventually use tighter bounds like axis aligned boxes around nodes
 * that are expected to be static. That probably means boxes for geometry and
 * spheres for grouping nodes.
 *
 * @see openvrml::bounding_sphere
 * @see openvrml::axis_aligned_bounding_box
 */

/**
 * @enum openvrml::bounding_volume::intersection
 *
 * @brief Indicates the type of intersection.
 */

/**
 * @var openvrml::bounding_volume::intersection openvrml::bounding_volume::outside
 *
 * @brief Indicates that the tested volume is entirely outside the target
 *        volume.
 */

/**
 * @var openvrml::bounding_volume::intersection openvrml::bounding_volume::partial
 *
 * @brief Indicates that the tested volume intersects with the target volume.
 */

/**
 * @var openvrml::bounding_volume::intersection openvrml::bounding_volume::inside
 *
 * @brief Indicates that the tested volume is entirely inside the target
 *        volume.
 */

/**
 * @brief Destroy.
 */
openvrml::bounding_volume::~bounding_volume() OPENVRML_NOTHROW
{}

/**
 * @brief Maximize the bounding volume.
 *
 * Delegates to <code>bounding_volume::do_maximize</code>.
 *
 * @see maximized
 */
void openvrml::bounding_volume::maximize()
{
    this->do_maximize();
}

/**
 * @fn void openvrml::bounding_volume::do_maximize()
 *
 * @brief Called by <code>bounding_volume::maximize</code>.
 *
 * @see maximize
 */

/**
 * @brief Indicates whether the bounding volume is maximized.
 *
 * The convention is that nodes that should be rendered unconditionally set a
 * maximum bounding volume, ensuring that the branch they are on does not get
 * pruned during culling. Stuff like the picking code needs a way to
 * differentiate this from just a really big bounding volume, or an unset
 * bounding volume.
 *
 * @return @c true if the bounding voume is maximized; @c false otherwise.
 *
 * @see openvrml::bounding_volume::maximize
 */
bool openvrml::bounding_volume::maximized() const OPENVRML_NOTHROW
{
    return this->do_maximized();
}

/**
 * @fn bool openvrml::bounding_volume::do_maximized() const
 *
 * @brief Called by <code>bounding_volume::maximized</code>.
 *
 * @return @c true if the bounding voume is maximized; @c false otherwise.
 *
 * @see openvrml::bounding_volume::maximized
 */

/**
 * @brief Intersect this <code>bounding_volume</code> with a
 *        <code>frustum</code>.
 *
 * The test assumes that the frustum is in the canonical
 * looking-down-negative-z orientation, so the bounding volume is going to have
 * to be transformed into the frustum's space. (Alternatives include
 * transforming the frustum into the bvolume's space, or transforming both of
 * them into the projection space. Lots of tradeoffs involved, but transforming
 * the bvolume is probably the simplest approach overall.)
 *
 * @param[in] frustum   the frustum.
 *
 * @return inside, outside, or partial.
 *
 * @see openvrml::bounding_volume::transform
 * @see openvrml::bounding_volume::ortho_transform
 */
openvrml::bounding_volume::intersection
openvrml::bounding_volume::
intersect_frustum(const openvrml::frustum & frustum) const
{
    return this->do_intersect_frustum(frustum);
}

/**
 * @fn openvrml::bounding_volume::intersection openvrml::bounding_volume::do_intersect_frustum(const openvrml::frustum & frustum) const
 *
 * @brief Called by <code>bounding_volume::intersect_frustum</code>
 *
 * @param[in] frustum   the frustum.
 *
 * @return inside, outside, or partial.
 *
 * @see openvrml::bounding_volume::transform
 * @see openvrml::bounding_volume::ortho_transform
 */

/**
 * @brief Extend the bounding_volume to enclose @p bv.
 *
 * Delegates to
 * <code>bounding_volume::do_extend(const bounding_sphere &)</code> and
 * <code>bounding_volume::do_extend(const axis_alligned_bounding_box &)</code>.
 *
 * @param[in] bv    a bounding volume.
 */
void openvrml::bounding_volume::extend(const bounding_volume & bv)
{
    // doing two tests isn't a big deal, if there get to be too many
    // more kinds of bounding volumes, move to a double dispatch
    // pattern.
    const bounding_sphere * bs = 0;
    const axis_aligned_bounding_box * ab = 0;
    if ((bs = dynamic_cast<const bounding_sphere *>(&bv))) {
        this->do_extend(*bs);
    } else if ((ab = dynamic_cast<const axis_aligned_bounding_box *>(&bv))) {
        this->do_extend(*ab);
    }
}

/**
 * @brief Extend the bounding volume to enclose @p p.
 *
 * Delegates to <code>bounding_volume::do_extend</code>.
 *
 * @param[in] p a point
 */
void openvrml::bounding_volume::extend(const vec3f & p)
{
    this->do_extend(p);
}

/**
 * @fn void openvrml::bounding_volume::do_extend(const vec3f & p)
 *
 * @brief Extend the bounding volume to enclose @p p.
 *
 * @param[in] p a point
 */

/**
 * @fn void openvrml::bounding_volume::do_extend(const axis_aligned_bounding_box & bbox)
 *
 * @brief Called by
 *        <code>bounding_volume::extend(const bounding_volume &)</code>.
 *
 * @param[in] bbox  an axis-aligned bounding box.
 */

/**
 * @fn void openvrml::bounding_volume::do_extend(const bounding_sphere & b)
 *
 * @brief Called by
 *        <code>bounding_volume::extend(const bounding_volume &)</code>.
 *
 * @param[in] b a bounding sphere
 */

/**
 * @brief Enclose the given set of points.
 *
 * This resets the volume from any previous values.
 *
 * Delegates to <code>bounding_volume::do_enclose</code>.
 *
 * @param[in] points    points.
 */
void openvrml::bounding_volume::enclose(const std::vector<vec3f> & points)
{
    this->do_enclose(points);
}

/**
 * @fn void openvrml::bounding_volume::do_enclose(const std::vector<vec3f> & points)
 *
 * @brief Called by <code>bounding_volume::enclose</code>.
 *
 * @param[in] points    points.
 */

/**
 * @brief Transform this bounding volume using an orthogonal transfom.
 *
 * Orthogonal transformations preserve angles. They include
 * translation, rotation, and uniform scaling. It turns out to be
 * so easy to transform bounding spheres by orthogonal
 * transformations that it's worth special casing. The caller is
 * responsible for assuring that the transformation is in fact
 * orthogonal, otherwise the results are undefined. If in doubt,
 * call transform instead and take the speed hit.
 *
 * Delegates to <code>bounding_volume::do_ortho_transform</code>.
 *
 * @param[in] M orthonormal transformation matrix.
 */
void openvrml::bounding_volume::ortho_transform(const mat4f & M)
{
    this->do_ortho_transform(M);
}

/**
 * @fn void openvrml::bounding_volume::do_ortho_transform(const mat4f & M)
 *
 * @brief Called by <code>bounding_volume::ortho_transform</code>.
 *
 * @param[in] M orthonormal transformation matrix in mat4f format
 */

/**
 * @brief Transform this bounding volume using an affine transfom.
 *
 * Affine transformations can include nonuniform scaling. It is much
 * messier to deal with them, but VRML allows nonuniform scaling, so
 * we have to handle it. Note that since all orthogonal transforms
 * are affine, it's safe to always call this routine instead of
 * <code>ortho_transform</code>, but it's likely to be slower.
 * The results are undefined if this routine is called with a
 * non-affine argument. Note that VRML Transform nodes only allow
 * affine transformations, so unless you're doing something tricky
 * this routine should always be safe.
 *
 * Delegates to <code>bounding_volume::do_transform</code>.
 *
 * @param[in] M affine transformation matrix.
 */
void openvrml::bounding_volume::transform(const mat4f & M)
{
    this->do_transform(M);
}

/**
 * @fn void openvrml::bounding_volume::do_transform(const mat4f & M)
 *
 * @brief Called by <code>bounding_volume::transform</code>.
 *
 * @param[in] M affine transformation matrix.
 */


/**
 * @class openvrml::bounding_sphere
 *
 * @brief A bounding sphere.
 *
 * Bounding spheres are very fast to intersect test, but they are fairly loose.
 * If asked to choose just one bounding volume, four out of five graphics
 * professionals surveyed chose the bounding sphere for their patients who
 * chewed bounding volumes.
 */

/**
 * @var openvrml::bounding_sphere::center_
 *
 * @brief The center of the sphere.
 */

/**
 * @var openvrml::bounding_sphere::radius_
 *
 * @brief The radius of the sphere.
 */

/**
 * @brief Construct.
 */
openvrml::bounding_sphere::bounding_sphere():
    radius_(-1.0)
{}

/**
 * @brief Destroy.
 */
openvrml::bounding_sphere::~bounding_sphere() OPENVRML_NOTHROW
{}

namespace {
    float sphere_plane_distance(const openvrml::bounding_sphere & bs,
                                const float N[3],
                                float D)
    {
        //
        // r = Ax + By + Cz + D
        //
        const openvrml::vec3f & c = bs.center();
        float d = N[0] * c[0] + N[1] * c[1] + N[2] * c[2] - D;
        return d;
    }
}

/**
 * @brief Intersect this bvolume with a frustum.
 *
 * The test assumes that the frustum is in the canonical
 * looking-down-negative-z orientation, so the bounding volume is going to have
 * to be transformed into the frustum's space. (Alternatives include
 * transforming the frustum into the bounding volume's space, or transforming
 * both of them into the projection space. Lots of tradeoffs involved, but
 * transforming the bounding volume is probably the simplest approach overall.)
 *
 * @param[in] frustum   the frustum.
 *
 * @return inside, outside, or partial.
 *
 * @see bounding_volume::transform
 * @see bounding_volume::ortho_transform
 */
openvrml::bounding_volume::intersection
openvrml::bounding_sphere::
do_intersect_frustum(const openvrml::frustum & frustum) const
{
    if (this->maximized()) { return bounding_volume::partial; }
    if (this->radius_ == -1.0f) { return bounding_volume::partial; } // ???

    bounding_volume::intersection code = bounding_volume::inside;

    //
    // Quick test against near/far planes since we know in the VRML97
    // viewing case they are parallel to the xy-plane. (VRML97 restricts
    // the view volume, but other systems might not; so be careful to
    // confirm the assumption before reusing this code).
    //

    //
    // Distance from the center of the sphere to the near plane.
    //
    float znear = -float(frustum.z_near);
    float d = znear - this->center_.z();
    if (d < -this->radius_) { return bounding_volume::outside; }
    if (d < this->radius_) { code = bounding_volume::partial; }

    //
    // Distance from the sphere center to the far plane. Same logic as
    // above.
    //
    float zfar = -float(frustum.z_far);
    d = this->center_.z() - zfar;
    if (d < -this->radius_) { return bounding_volume::outside; }
    if (d < this->radius_) { code = bounding_volume::partial; }

    //
    // Test against the top; the same logic will be used to test against
    // the other sides. We still want to find the distanct to the plane; but
    // unlike the near/far planes we have to use the dot product.
    //
    d = sphere_plane_distance(*this, frustum.top_plane, frustum.top_plane[3]);
    if (d < -this->radius_) { return bounding_volume::outside; }
    if (d < this->radius_) { code = bounding_volume::partial; }

    d = sphere_plane_distance(*this, frustum.bot_plane, frustum.bot_plane[3]);
    if (d < -this->radius_) { return bounding_volume::outside; }
    if (d < this->radius_) { code = bounding_volume::partial; }

    d = sphere_plane_distance(*this,
                              frustum.left_plane,
                              frustum.left_plane[3]);
    if (d < -this->radius_) { return bounding_volume::outside; }
    if (d < this->radius_) { code = bounding_volume::partial; }

    d = sphere_plane_distance(*this,
                              frustum.right_plane,
                              frustum.right_plane[3]);
    if (d < -this->radius_) { return bounding_volume::outside; }
    if (d < this->radius_) { code = bounding_volume::partial; }

    return code;
}

/**
 * @brief Extend to enclose @p p.
 *
 * @param[in] p a point.
 */
void openvrml::bounding_sphere::do_extend(const vec3f & p)
{
    using openvrml_::fequal;

    if (this->maximized()) { return; }

    // if this bsphere isn't set yet, then just do an assign. what's it
    // mean to have a zero radius bsphere? is that going to mess
    // anything up (iow, do we ever divide by radius?)
    //
    if (this->radius_ == -1.0f) { // flag, not comparison
        this->radius_ = 0.0f;
        this->center_ = p;
        return;
    }

    // you know, we could probably just call extend(sphere) with a
    // radius of zero and it would work out the same.

    float x0 = this->center_.x();
    float y0 = this->center_.y();
    float z0 = this->center_.z();
    float r0 = this->radius_;

    float x1 = p.x();
    float y1 = p.y();
    float z1 = p.z();

    float xn = x1 - x0;
    float yn = y1 - y0;
    float zn = z1 - z0;
    float dn = float(sqrt(xn * xn + yn * yn + zn * zn));

    if (fequal(dn, 0.0f)) { return; }

    if (dn < r0) {
        // point is inside sphere
        return;
    }

    float cr = float((dn + r0) / 2.0);
    float tmp = (cr - r0) / dn;
    float cx = x0 + xn * tmp;
    float cy = y0 + yn * tmp;
    float cz = z0 + zn * tmp;

    this->radius_ = cr;
    this->center_.x(cx);
    this->center_.y(cy);
    this->center_.z(cz);
}

/**
 * @brief Extend the bounding volume to enclose @p bbox.
 *
 * @todo Implement me!
 *
 * @param[in] bbox  an axis-aligned bounding box
 */
void
openvrml::bounding_sphere::do_extend(const axis_aligned_bounding_box &)
{}

/**
 * @brief Extend this bvolume to enclose the given sphere.
 *
 * @param[in] b a bounding sphere
 */
void openvrml::bounding_sphere::do_extend(const bounding_sphere & b)
{
    using openvrml_::fequal;

    if (this->maximized()) { return; }

    if (b.maximized()) {
        this->maximize();
        return;
    }

    // if the other bsphere isn't set, ignore it?
    //
    if (b.radius_ == -1.0f) { return; }

    // if this bsphere isn't set yet, then just do an
    // assign.
    //
    if (this->radius_ == -1.0f) { // flag, not comparison
        *this = b;
        return;
    }

    // s0 = ((x0,y0,z0),r0)
    // s1 = ((x1,y1,z1),r1)

    float x0 = this->center_.x();
    float y0 = this->center_.y();
    float z0 = this->center_.z();
    float r0 = this->radius_;

    float x1 = b.center_.x();
    float y1 = b.center_.y();
    float z1 = b.center_.z();
    float r1 = b.radius_;

    float xn = x1 - x0;
    float yn = y1 - y0;
    float zn = z1 - z0;
    float dn = float(sqrt(xn * xn + yn * yn + zn * zn));

    if (fequal(dn, 0.0f)) { return; }

    if (dn + r1 < r0) { // inside us, so no change
        return;
    }
    if (dn + r0 < r1) { // we're inside them...
        *this = b;
        return;
    }

    float cr = float((dn + r0 + r1) / 2.0);
    float tmp = (cr - r0) / dn;
    float cx = x0 + xn * tmp;
    float cy = y0 + yn * tmp;
    float cz = z0 + zn * tmp;

    this->radius_ = cr;
    this->center_ = vec3f(cx, cy, cz);
}

/**
 * @brief Enclose the given set of points.
 *
 * This resets the volume from any previous values.
 *
 * @param[in] points    points.
 */
void openvrml::bounding_sphere::do_enclose(const std::vector<vec3f> & points)
{
    // doing an extend() for each point is ok, but there are
    // faster algorithms reference "An Efficient Bounding Sphere"
    // Graphics Gems pg 301. it depends on being able to make multiple
    // passes over the points, so it isn't appropriate for incremental
    // updates, but it would be good here...
    // for(int i=0; i<n; ++i)
    // this->extend(&p[i]);
    //

    *this = bounding_sphere();

    if (points.size() < 1) { return; }

    const vec3f * min_p[3] = { &points[0], &points[0], &points[0] };
    const vec3f * max_p[3] = { &points[0], &points[0], &points[0] };

    //
    // Find the 6 points with: minx, maxx, miny, maxy, minz, maxz.
    //
    size_t i;
    for (i = 1; i < points.size(); ++i) {
        const vec3f & pi = points[i];
        if (pi.x() < min_p[0]->x()) { min_p[0] = &pi; }
        if (pi.y() < min_p[1]->y()) { min_p[1] = &pi; }
        if (pi.z() < min_p[2]->z()) { min_p[2] = &pi; }

        if (pi.x() > max_p[0]->x()) { max_p[0] = &pi; }
        if (pi.y() > max_p[1]->y()) { max_p[1] = &pi; }
        if (pi.z() > max_p[2]->z()) { max_p[2] = &pi; }
    }

    //
    // Pick the two points most distant from one another.
    //
    vec3f span;
    span = *max_p[0] - *min_p[0];
    float dx = span.dot(span);
    span = *max_p[1] - *min_p[1];
    float dy = span.dot(span);
    span = *max_p[2] - *min_p[2];
    float dz = span.dot(span);

    const vec3f * max_span0 = min_p[0];
    const vec3f * max_span1 = max_p[0];
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

    this->center_ = (*max_span0 + *max_span1) / 2.0;

    this->radius_ = float(sqrt(this->center_.dot(this->center_)));

    for (i = 0; i < points.size(); ++i) { this->extend(points[i]); }
}

/**
 * @brief Get the center coordinates.
 *
 * @return the center coordinates.
 */
const openvrml::vec3f & openvrml::bounding_sphere::center() const
{
    return this->center_;
}

/**
 * @brief Set the center coordinates.
 *
 * @param[in] c new center coordinates.
 */
void openvrml::bounding_sphere::center(const vec3f & c)
{
    this->center_ = c;
}

/**
 * @brief Get the radius.
 *
 * @return the radius.
 */
float openvrml::bounding_sphere::radius() const
{
    return this->radius_;
}

/**
 * @brief Set the radius.
 *
 * @param[in] r new radius value.
 */
void openvrml::bounding_sphere::radius(const float r)
{
    this->radius_ = r;
}

/**
 * @brief Extend the bounding sphere to infinity.
 */
void openvrml::bounding_sphere::do_maximize()
{
    this->radius_ = std::numeric_limits<float>::max();
    this->center_ = vec3f(0.0, 0.0, 0.0);
}

/**
 * @brief Whether the bounding_sphere is maximized.
 *
 * @return @c true if the bounding_sphere has been maximized; @c false
 *         otherwise.
 */
bool openvrml::bounding_sphere::do_maximized() const
{
    if (this->radius_ == std::numeric_limits<float>::max()) { return true; }
    return false;
}

/**
 * @brief Orthographically transform the bounding_sphere by @p t.
 *
 * @param[in] t transformation matrix.
 */
void openvrml::bounding_sphere::do_ortho_transform(const mat4f & t)
{
    if (this->maximized()) { return; }
    if (this->radius_ == -1) { return; }
    // ortho is easy: since we know it's uniform scaling, we can just
    // scale the radius and translate the center, and we're done.
    this->center_ *= t;

    // uniform scale means we can pick any of the scale elements? wait:
    // can we really do this?
    this->radius_ *= vec3f(t[0][0], t[1][0], t[2][0]).length();
}

/**
 * @brief Transform the bounding_sphere by @p t.
 *
 * @param[in] t transformation matrix.
 */
void openvrml::bounding_sphere::do_transform(const mat4f & t)
{
    if (this->maximized()) { return; }
    if (this->radius_ == -1) { return; }
    this->center_ *= t;

    vec3f x_scale_v(t[0][0], t[1][0], t[2][0]);
    vec3f y_scale_v(t[0][1], t[1][1], t[2][1]);
    vec3f z_scale_v(t[0][2], t[1][2], t[2][2]);

    float scale_x = x_scale_v.length();
    float scale_y = y_scale_v.length();
    float scale_z = z_scale_v.length();

    float max_scale = scale_x;
    if (scale_y > max_scale) { max_scale = scale_y; }
    if (scale_z > max_scale) { max_scale = scale_z; }

    this->radius_ *= max_scale;
}


/**
 * @class openvrml::axis_aligned_bounding_box
 *
 * @brief An axis-aligned bounding box.
 *
 * @todo This class is currently just a placeholder.
 */

/**
 * @brief Destroy.
 */
openvrml::axis_aligned_bounding_box::~axis_aligned_bounding_box()
    OPENVRML_NOTHROW
{}

/**
 * @todo Implement me!
 */
openvrml::bounding_volume::intersection
openvrml::axis_aligned_bounding_box::
do_intersect_frustum(const openvrml::frustum &) const
{
    return bounding_volume::partial;
}

/**
 * @todo Implement me!
 */
void openvrml::axis_aligned_bounding_box::do_extend(const vec3f &)
{}

/**
 * @todo Implement me!
 */
void
openvrml::axis_aligned_bounding_box::
do_extend(const axis_aligned_bounding_box &)
{}

/**
 * @todo Implement me!
 */
void openvrml::axis_aligned_bounding_box::do_extend(const bounding_sphere &)
{}

/**
 * @brief Enclose the given set of points.
 *
 * This resets the volume from any previous values.
 *
 * @param[in] points    points.
 *
 * @todo Implement me!
 */
void
openvrml::axis_aligned_bounding_box::
do_enclose(const std::vector<vec3f> &)
{}

/**
 * @brief Implement me!
 */
void openvrml::axis_aligned_bounding_box::do_maximize()
{}

/**
 * @brief Implement me!
 */
bool openvrml::axis_aligned_bounding_box::do_maximized() const
{
    return true;
}

/**
 * @brief Implement me!
 */
void openvrml::axis_aligned_bounding_box::do_ortho_transform(const mat4f &)
{}

/**
 * @brief Implement me!
 */
void openvrml::axis_aligned_bounding_box::do_transform(const mat4f &)
{}
