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

#ifndef VRMLAABOX_H
#define VRMLAABOX_H

#include "common.h"
#include "VrmlBVolume.h"

/**
 * An axis-aligned bounding box.
 * <p>
 * You might have noticed that this class is currently just a
 * placeholder.
 *
 */
class OPENVRML_SCOPE VrmlAABox : public VrmlBVolume {

 public:

  VrmlBSphere* toBSphere() const;
  VrmlAABox* toAABox() const;

  int isectFrustum(const VrmlFrustum& f) const;

  void extend(const VrmlBVolume& b);
  void extend(const float p[3]);
  void extend(const VrmlAABox& b);
  void extend(const VrmlBSphere& b);
  void enclose(const float* p, int n);

  bool isMAX() const;
  void setMAX();

  void orthoTransform(const double M[4][4]);
  void transform(const double M[4][4]);

  ostream& dump(ostream& ostr) const;
};

#endif
