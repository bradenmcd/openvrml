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

#ifdef _WIN32
#include <winconfig.h>
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream.h>     // ostream for dump
#include "private.h"
#include "MathUtils.h"    // Vcross(), Vnorm(), etc
#include "VrmlFrustum.h"

namespace OpenVRML {

/**
 * @class VrmlFrustum
 *
 * @brief A view frustum.
 *
 * A frustum is more or less a truncated pyramid. This class represents
 * frustums with their wide end facing down the -z axis, and their (theoretical)
 * tip at the origin. A frustum is a convenient representation of the volume of
 * virtual space visible through the on-screen window when using a perspective
 * projection.
 *
 * @see VrmlNode::render
 * @see Viewer
 */

/**
 * @var VrmlFrustum::fovy
 *
 * @brief Vertical field of view.
 */

/**
 * @var VrmlFrustum::fovx
 *
 * @brief Horizontal field of view.
 */

/**
 * @var VrmlFrustum::z_near
 *
 * @brief Distance to the near clipping plane.
 */

/**
 * @var VrmlFrustum::z_far
 *
 * @brief Distance to the far clipping plane.
 */

/**
 * @var VrmlFrustum::left_plane
 *
 * @brief Left (looking down -z) side clip plane.
 *
 * Format is (a,b,c,d) where (a,b,c) is the plane normal and d is the plane
 * offset. For the momement the eyepoint is always the origin, so d is going
 * to be 0.
 */

/**
 * @brief Constructs a default frustum.
 *
 * fovy, fovx, near and far all get set to -1.
 */
VrmlFrustum::VrmlFrustum() {
    fovy = -1;
    fovx = -1;
    z_near = -1;
    z_far = -1;
}

/**
 * @brief Constructs and initializes a frustum.
 *
 * The field of view should be less than 180 degrees. Extreme aspect ratios
 * are unlikely to work well. The near and far plane distances are always
 * positive (think distance, not position). anear must be less than afar.
 * This is supposed to look like gluPerspective.
 *
 * @param afovy vertical field of view in degrees
 * @param aaspect ratio of view width to height (not the ratio of
 *                the horizontal to vertial field-of-views)
 * @param anear distance to near clip plane
 * @param afar distance to far clip plane
 *
 */
VrmlFrustum::VrmlFrustum(float afovy, float aaspect, double anear, double afar) {
    using OpenVRML_::pi;
    
    fovy = (afovy / 360.0) * 2.0 * pi;
    float cy = (float)tan(fovy/2.0);
    fovx = 2.0*atan(cy*aaspect);
    z_near = anear;
    z_far = afar;
    update();
}

/**
 * @brief Update the plane equations.
 *
 * The plane equations are derived from the other members.
 */
void VrmlFrustum::update() {
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


/**
 * @brief Dumps the frustum to the given stream. 
 */
std::ostream & VrmlFrustum::dump(std::ostream & ostr) const {
    using OpenVRML_::pi;
    
    ostr << "VrmlFrustum {" << endl;
    ostr << z_near << endl;
    ostr << z_far << endl;
    ostr << fovx * (360.0 / (pi * 2.0)) << endl;
    ostr << fovy * (360.0 / (pi * 2.0)) << endl;
    ostr << "}";
    return ostr;
}

} // namespace OpenVRML
