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

#include "VrmlBVolume.h"

/**
 * @class VrmlBVolume
 *
 * @brief A bounding volume.
 *
 * All the geometry in a scene maintains a bounding volume to help speed up
 * rendering and picking. Although currently we're just using spheres, the plan
 * is to eventually use tighter bounds like axis aligned boxes around nodes
 * that are expected to be static. That probably means boxes for geometry and
 * spheres for grouping nodes.
 *
 * @see VrmlNode::render
 * @see VrmlBSphere
 * @see VrmlAABox
 */

/**
 * @var VrmlBVolume::BV_INSIDE
 *
 * @brief Results of an intersection; indicates that the tested volume is
 *      entirely inside the target volume.
 */

/**
 * @var VrmlBVolume::BV_OUTSIDE
 *
 * @brief Results of an intersection; indicates that the tested volume is
 *      entirely outside the target volume.
 */

/**
 * @var VrmlBVolume::BV_PARTIAL
 *
 * @brief Results of an intersection; indicates that the tested volume
 *      intersects with the target volume.
 */

/**
 * @brief Destructor.
 */
VrmlBVolume::~VrmlBVolume() {}

/**
 * @fn bool VrmlBVolume::isMAX() const
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
 * @fn void VrmlBVolume::setMAX()
 *
 * @brief Sets the MAX flag.
 *
 * @see isMAX
 */

/**
 * @fn int isectFrustum(const VrmlFrustum & f) const
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
 * @see VrmlBVolume::transform
 * @see VrmlBVolume::orthoTransform
 */

/**
 * @fn void VrmlBVolume::extend(const VrmlBVolume & b)
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
 * @fn void VrmlBVolume::extend(const float p[3])
 *
 * @brief Extend this bvolume to enclose the given point.
 *
 * @param p a point
 */

/**
 * @fn void VrmlBVolume::extend(const VrmlAABox & b)
 *
 * @brief Extend this bvolume to enclose the given box.
 *
 * @param b an axis-aligned box
 */

/**
 * @fn void VrmlBVolume::extend(const VrmlBSphere & b)
 *
 * @brief Extend this bvolume to enclose the given sphere.
 *
 * @param b a bounding sphere
 */

/**
 * @fn void VrmlBVolume::enclose(const float * p, int n)
 *
 * @brief Enclose the given set of points.
 *
 * This resets the volume from any previous values.
 *
 * @param p array of floats, each set of 3 represents a point
 * @param n number of points (not number of floats!)
 */

/**
 * @fn void VrmlBVolume::orthoTransform(const VrmlMatrix & M)
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
 * @fn void VrmlBVolume::transform(const VrmlMatrix & M)
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
 * @fn ostream & VrmlBVolume::dump(ostream & ostr) const
 *
 * @brief Print a text representation to a stream.
 *
 * Suitable for debugging.
 *
 * @param ostr an open ostream
 *
 * @return the ostream, save a line writing the output operator
 */
