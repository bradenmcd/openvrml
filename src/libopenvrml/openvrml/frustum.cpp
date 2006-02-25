// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 2000  Christopher K. St. John
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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <private.h>
# include "basetypes.h"
# include "frustum.h"

/**
 * @class openvrml::frustum
 *
 * @brief A view frustum.
 *
 * A frustum is more or less a truncated pyramid. This class represents
 * frustums with their wide end facing down the -z axis, and their
 * (theoretical) tip at the origin. A frustum is a convenient representation of
 * the volume of virtual space visible through the on-screen window when using
 * a perspective projection.
 *
 * @see openvrml::node::render
 * @see openvrml::viewer
 */

/**
 * @var openvrml::frustum::fovy
 *
 * @brief Vertical field of view.
 */

/**
 * @var openvrml::frustum::fovx
 *
 * @brief Horizontal field of view.
 */

/**
 * @var openvrml::frustum::z_near
 *
 * @brief Distance to the near clipping plane.
 */

/**
 * @var openvrml::frustum::z_far
 *
 * @brief Distance to the far clipping plane.
 */

/**
 * @var openvrml::frustum::left_plane
 *
 * @brief Left (looking down -z) side clip plane.
 *
 * Format is (a,b,c,d) where (a,b,c) is the plane normal and d is the plane
 * offset. For the momement the eyepoint is always the origin, so d is going
 * to be 0.
 */

/**
 * @var openvrml::frustum::right_plane
 *
 * @brief Right clipping plane.
 */

/**
 * @var openvrml::frustum::top_plane
 *
 * @brief Top clipping plane.
 */

/**
 * @var openvrml::frustum::bot_plane
 *
 * @brief Bottom clipping plane.
 */

/**
 * @brief Construct a default frustum.
 *
 * fovy, fovx, near and far all get set to -1.
 */
openvrml::frustum::frustum():
    fovy(-1),
    fovx(-1),
    z_near(-1),
    z_far(-1)
{}

/**
 * @brief Construct and initialize a frustum.
 *
 * The field of view should be less than 180 degrees. Extreme aspect ratios
 * are unlikely to work well. The near and far plane distances are always
 * positive (think distance, not position). anear must be less than afar.
 * This is supposed to look like gluPerspective.
 *
 * @param[in] fovy      vertical field of view in degrees.
 * @param[in] aspect    ratio of view width to height (not the ratio of
 *                  the horizontal to vertial field-of-views).
 * @param[in] z_near    distance to near clip plane.
 * @param[in] z_far     distance to far clip plane.
 *
 */
openvrml::frustum::frustum(float fovy,
                           float aspect,
                           double z_near,
                           double z_far):
    fovy(float((fovy / 360.0) * 2.0 * openvrml_::pi)),
    fovx(float(2.0 * atan(float(tan(this->fovy / 2.0)) * aspect))),
    z_near(z_near),
    z_far(z_far)
{
    update();
}

/**
 * @brief Update the plane equations.
 *
 * The plane equations are derived from the other members.
 */
void openvrml::frustum::update()
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

    float cy = float(tan(fovy / 2.0));
    float cx = float(tan(fovx / 2.0));

    float NL = float(-z_near * cx);
    float NR = -NL;
    float NT = float(z_near * cy);
    float NB = -NT;

    vec3f tmp0, tmp1;

    tmp0.x(NL);
    tmp0.y(NB);
    tmp0.z(float(-z_near));

    tmp1.x(NL);
    tmp1.y(NT);
    tmp1.z(float(-z_near));

    vec3f v;
    v = (tmp0 * tmp1).normalize();
    left_plane[0] = v.x();
    left_plane[1] = v.y();
    left_plane[2] = v.z();
    left_plane[3] = 0;

    right_plane[0] = -left_plane[0];
    right_plane[1] = 0;
    right_plane[2] = left_plane[2];
    right_plane[3] = 0;

    tmp0.x(NL);
    tmp0.y(NT);
    tmp0.z(float(-z_near));

    tmp1.x(NR);
    tmp1.y(NT);
    tmp1.z(float(-z_near));

    v = (tmp0 * tmp1).normalize();
    top_plane[0] = v.x();
    top_plane[1] = v.y();
    top_plane[2] = v.z();
    top_plane[3] = 0;

    bot_plane[0] = 0;
    bot_plane[1] = -top_plane[1];
    bot_plane[2] = top_plane[2];
    bot_plane[3] = 0;
}
