//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
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
#ifndef  VRMLSFROTATION_H
#define  VRMLSFROTATION_H

#include "VrmlField.h"

class VrmlSFRotation : public VrmlField {
public:

  VrmlSFRotation(float x = 0.0, float y = 0.0, float z = 1.0, float r = 0.0);

  virtual ostream& print(ostream& os) const;

  virtual VrmlField *clone() const;

  virtual VrmlFieldType fieldType() const;
  virtual const VrmlSFRotation* toSFRotation() const;
  virtual VrmlSFRotation* toSFRotation();

  float x() const		{ return d_x[0]; }
  float y() const		{ return d_x[1]; }
  float z() const	        { return d_x[2]; }
  float r() const		{ return d_x[3]; }
  float *get()			{ return &d_x[0]; }

  void set(float x, float y, float z, float r)
    { d_x[0] = x; d_x[1] = y; d_x[2] = z; d_x[3] = r; }

  void invert(void);
  void multiply(VrmlSFRotation*);
  
  void ToQuaternion(float* theQuat);
  void FromQuaternion(float* theQuat);
  void MultQuat(float* result, float* quat1, float* quat2);
  
  void slerp(VrmlSFRotation*, float);  

private:
  float d_x[4];

};

#endif
