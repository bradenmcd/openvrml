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

#ifndef VRMLFRUSTUM_H
#define VRMLFRUSTUM_H

#include <iostream.h>
#include "common.h"


/**
 * A view frustum. A frustum is more or less a truncated pyramid. This
 * class represents frustums with their wide end facing down the -z
 * axis, and their (theoretical) tip at the origin. A frustum is a
 * convenient representation of the volume of virtual space visible
 * through the on-screen window when using a perspective projection.
 *
 * @see VrmlNode::render
 * @see Viewer
 */
class OPENVRML_SCOPE VrmlFrustum {

 public:

  /**
   * Vertical field of view.
   */
  float fovy;

  /**
   * Horizontal field of view.
   */
  float fovx;

  /**
   * Distance to the near clipping plane.
   */
  double z_near;
  /**
   * Distance to the far clipping plane
   */
  double z_far;

  /**
   * Left (looking down -z) side clip plane. Format is (a,b,c,d) where
   * (a,b,c) is the plane normal and d is the plane offset. For the
   * momement the eyepoint is always the origin, so d is going to be
   * 0.
   */
  float left_plane[4];
  float right_plane[4];
  float top_plane[4];
  float bot_plane[4];


  /**
   * Constructs a default frustum. fovy, fovx, near and far all get
   * set to -1.
   */
  VrmlFrustum();

  /**
   * Constructs and initializes a frustum. The field of view should be
   * less than 180 degrees. Extreme aspect ratios are unlikely to work
   * well. The near and far plane distances are always positive (think
   * distance, not position). anear must be less than afar. This is
   * supposed to look like gluPerspective.
   *
   * @param afovy vertical field of view in degrees
   * @param aaspect ratio of view width to height (not the ratio of
   *                the horizontal to vertial field-of-views)
   * @param anear distance to near clip plane
   * @param afar distance to far clip plane
   *
   */
  VrmlFrustum(float afovy, float aaspect, double anear, double afar);

  /**
   * Dumps the frustum to the given stream. 
   */
  ostream& dump(ostream& ostr) const;


 protected:


  /**
   * Update the plane equations. The plane equations are derived
   * from the other memebers.
   */
  void update();
};



#endif // VRMLFRUSTUM_H
