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

#ifndef VRMLNODEPLANESENSOR_H
#define VRMLNODEPLANESENSOR_H

#include "VrmlNodeChild.h"
#include "VrmlSFBool.h"
#include "VrmlSFFloat.h"
#include "VrmlSFVec2f.h"
#include "VrmlSFVec3f.h"

class VrmlScene;


/**
 * The PlaneSensor node maps pointing device motion into
 * two-dimensional translation in a plane parallel to the Z=0 plane of
 * the local coordinate system. The PlaneSensor node uses the
 * descendent geometry of its parent node to determine whether it is
 * liable to generate events
 */
class VrmlNodePlaneSensor : public VrmlNodeChild {

public:

  // Define the fields of PlaneSensor nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodePlaneSensor( VrmlScene *scene = 0);
  virtual ~VrmlNodePlaneSensor();

  virtual VrmlNode *cloneMe() const;

  virtual VrmlNodePlaneSensor* toPlaneSensor() const;

  virtual ostream& printFields(ostream& os, int indent);

  void activate( double timeStamp, bool isActive, double *p );

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  virtual void accumulateTransform( VrmlNode* );
  virtual VrmlNode* getParentTransform();

  bool isEnabled() { return d_enabled.get(); }

protected:

  // Fields
  VrmlSFBool d_autoOffset;
  VrmlSFBool d_enabled;
  VrmlSFVec2f d_maxPosition;
  VrmlSFVec2f d_minPosition;
  VrmlSFVec3f d_offset;

  VrmlSFBool d_isActive;
  VrmlSFVec3f d_translation;
  VrmlSFVec3f d_trackPoint;

  VrmlSFVec3f d_activationPoint;

  VrmlNode *d_parentTransform;

  /**
   * Track the inverse transform matrix at the time of sensor
   * activation. Mouse deltas are relative to the original
   * transform. This matters because the transform stack on top of the
   * sensor may be modified by the output of the sensor.
   */
  double d_activationMatrix[4][4];

};

#endif
