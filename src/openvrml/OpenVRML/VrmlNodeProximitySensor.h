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

#ifndef  _VRMLNODEPROXIMITYSENSOR_
#define  _VRMLNODEPROXIMITYSENSOR_

#include "VrmlNodeChild.h"
#include "VrmlSFBool.h"
#include "VrmlSFRotation.h"
#include "VrmlSFTime.h"
#include "VrmlSFVec3f.h"

class VrmlScene;

class VrmlNodeProximitySensor : public VrmlNodeChild {

public:

  // Define the fields of ProximitySensor nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeProximitySensor( VrmlScene *scene = 0);
  virtual ~VrmlNodeProximitySensor();

  virtual VrmlNode *cloneMe() const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual void render(Viewer *, VrmlRenderContext rc);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

private:

  // Fields
  VrmlSFVec3f d_center;
  VrmlSFBool d_enabled;
  VrmlSFVec3f d_size;

  // Internal state
  VrmlSFBool d_isActive;
  VrmlSFVec3f d_position;
  VrmlSFRotation d_orientation;
  VrmlSFTime d_enterTime;
  VrmlSFTime d_exitTime;
};

#endif _VRMLNODEPROXIMITYSENSOR_
