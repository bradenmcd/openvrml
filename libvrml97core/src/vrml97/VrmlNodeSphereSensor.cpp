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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "VrmlNodeSphereSensor.h"
#include "VrmlNodeType.h"
#include "MathUtils.h"
#include "System.h"
#include "Viewer.h"
#include "VrmlScene.h"

// SphereSensor factory. 

static VrmlNode *creator( VrmlScene *scene ) 
{
  return new VrmlNodeSphereSensor(scene);
}


// Define the built in VrmlNodeType:: "SphereSensor" fields

VrmlNodeType *VrmlNodeSphereSensor::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("SphereSensor", creator);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addExposedField("autoOffset", VrmlField::SFBOOL);
  t->addExposedField("enabled", VrmlField::SFBOOL);
  t->addExposedField("offset", VrmlField::SFROTATION);
  t->addEventOut("isActive", VrmlField::SFBOOL);
  t->addEventOut("rotation_changed", VrmlField::SFROTATION);
  t->addEventOut("trackPoint_changed", VrmlField::SFVEC3F);

  return t;
}

VrmlNodeType & VrmlNodeSphereSensor::nodeType() const 
{
    return *defineType(0);
}


VrmlNodeSphereSensor::VrmlNodeSphereSensor( VrmlScene *scene ) :
  VrmlNodeChild(scene),
  d_autoOffset(true),
  d_enabled(true),
  d_offset(0,1,0,0),
  d_isActive(false)
{
  setModified();
}


VrmlNodeSphereSensor::~VrmlNodeSphereSensor()
{
}


VrmlNode *VrmlNodeSphereSensor::cloneMe() const
{
  return new VrmlNodeSphereSensor(*this);
}

VrmlNodeSphereSensor* VrmlNodeSphereSensor::toSphereSensor() const	// mgiger 6/16/00
{
	return (VrmlNodeSphereSensor*) this;
}

ostream& VrmlNodeSphereSensor::printFields(ostream& os, int indent)
{
  if (! d_autoOffset.get()) PRINT_FIELD(autoOffset);
  if (! d_enabled.get())    PRINT_FIELD(enabled);

  if (! FPZERO(d_offset.getX()) ||
      ! FPEQUAL(d_offset.getY(), 1.0) ||
      ! FPZERO(d_offset.getZ()) ||
      ! FPZERO(d_offset.getAngle()) )
    PRINT_FIELD(offset);

  return os;
}

const VrmlField *VrmlNodeSphereSensor::getField(const char *fieldName) const
{
  if ( strcmp( fieldName, "autoOffset" ) == 0 )
    return &d_autoOffset;
  else if ( strcmp( fieldName, "enabled" ) == 0 )
    return &d_enabled;
  else if ( strcmp( fieldName, "offset" ) == 0 )
    return &d_offset;

  // eventOuts
  else if ( strcmp( fieldName, "isActive" ) == 0 )
    return &d_isActive;
  else if ( strcmp( fieldName, "rotation" ) == 0 )
    return &d_rotation;
  else if ( strcmp( fieldName, "trackPoint" ) == 0 )
    return &d_trackPoint;
  
  return VrmlNodeChild::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodeSphereSensor::setField(const char *fieldName,
				   const VrmlField &fieldValue)
{
  if TRY_FIELD(autoOffset, SFBool)
  else if TRY_FIELD(enabled, SFBool)
  else if TRY_FIELD(offset, SFRotation)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}

void VrmlNodeSphereSensor::activate( double timeStamp,
				    bool isActive,
				    double *p )
{
		// Become active
	if ( isActive && ! d_isActive.get() ) {
		d_isActive.set(isActive);
		
			// set activation point in world coords
                const float floatVec[3] = { p[0], p[1], p[2] };
		d_activationPoint.set(floatVec);
		
		if(d_autoOffset.get())
			d_rotation = d_offset;
		
			// calculate the center of the object in world coords
		float V[3] = { 0.0, 0.0, 0.0 };
		double M[4][4];
		inverseTransform( M );
		VM( V, M, V );
		d_centerPoint.set(V);
		
			// send message
		eventOut( timeStamp, "isActive", d_isActive );
	}

		// Become inactive
	else if ( ! isActive && d_isActive.get() ) {
		d_isActive.set(isActive);
		eventOut( timeStamp, "isActive", d_isActive );

			// save auto offset of rotation
		if ( d_autoOffset.get() ) {
			d_offset = d_rotation;
			eventOut( timeStamp, "offset_changed", d_offset );
		}
	}

		// Tracking
	else if ( isActive ) {

		// get local coord for touch point
		float V[3] = { p[0], p[1], p[2] };
		double M[4][4];
		inverseTransform( M );
		VM( V, M, V );
		d_trackPoint.set(V);
		eventOut( timeStamp, "trackPoint_changed", d_trackPoint );
		
		float V2[3] = { p[0], p[1], p[2] };
                float tempv[3];
		Vdiff(tempv, V2, d_centerPoint.get());
                VrmlSFVec3f dir1(tempv);
		double dist = dir1.length();				// get the length of the pre-normalized vector
		dir1 = dir1.normalize();
		Vdiff(tempv, d_activationPoint.get(), d_centerPoint.get());
                VrmlSFVec3f dir2(tempv);
		dir2 = dir2.normalize();
		
		Vcross(tempv, dir1.get(), dir2.get());
                VrmlSFVec3f cx(tempv);

		VrmlSFRotation newRot(cx, dist * acos(dir1.dot(dir2)));
		if ( d_autoOffset.get() )
			newRot = newRot.multiply(d_offset);
		d_rotation = newRot;
		
		eventOut( timeStamp, "rotation_changed", d_rotation );
	}
}
