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

#ifndef VRMLBSPHERE_H
#define VRMLBSPHERE_H

#include "VrmlBVolume.h"
class VrmlSFVec3f;


/**
 * A bounding sphere. Bounding spheres are very fast to intersect
 * test, but they are fairly loose. If asked to choose just one
 * bounding volume, four out of five graphics professionals surveyed
 * chose the bounding sphere for their patients who chewed bounding
 * volumes.
 */
class VrmlBSphere : public VrmlBVolume {

 protected:

  /**
   * The center of the sphere.
   */
  float c[3];

  /**
   * The radius of the sphere.
   */
  float r;

 public:

  VrmlBSphere();
  VrmlBSphere(const VrmlBSphere& rhs);
  virtual ~VrmlBSphere();

  VrmlBSphere* toBSphere() const;
  VrmlAABox* toAABox() const;

  void reset();

  int isectFrustum(const VrmlFrustum& f) const;

  void extend(const VrmlBVolume& b);
  void extend(const float p[3]) ;
  void extend(const VrmlAABox& b);
  void extend(const VrmlBSphere& b);
  void enclose(const float* p, int n);

  virtual bool isMAX() const;
  virtual void setMAX();

  virtual void orthoTransform(const double M[4][4]);
  virtual void transform(const double M[4][4]);

  const float* getCenter() const;
  void setCenter(const VrmlSFVec3f& c);
  void setCenter(float x, float y, float z);
  void setCenter(const float ac[3]);

  float getRadius() const;
  void setRadius(float r);

  ostream& dump(ostream& ostr) const;
};



#endif // VRMLBSPHERE_H
