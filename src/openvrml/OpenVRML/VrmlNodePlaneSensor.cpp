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

#include "VrmlNodePlaneSensor.h"
#include "VrmlNodeType.h"
#include "MathUtils.h"
#include "System.h"
#include "Viewer.h"
#include "VrmlScene.h"

// PlaneSensor factory. 

static VrmlNode *creator( VrmlScene *scene ) 
{
  return new VrmlNodePlaneSensor(scene);
}


// Define the built in VrmlNodeType:: "PlaneSensor" fields

VrmlNodeType *VrmlNodePlaneSensor::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("PlaneSensor", creator);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addExposedField("autoOffset", VrmlField::SFBOOL);
  t->addExposedField("enabled", VrmlField::SFBOOL);
  t->addExposedField("maxPosition", VrmlField::SFVEC2F);
  t->addExposedField("minPosition", VrmlField::SFVEC2F);
  t->addExposedField("offset", VrmlField::SFVEC3F);
  t->addEventOut("isActive", VrmlField::SFBOOL);
  t->addEventOut("translation_changed", VrmlField::SFVEC3F);
  t->addEventOut("trackPoint_changed", VrmlField::SFVEC3F);

  return t;
}

VrmlNodeType & VrmlNodePlaneSensor::nodeType() const 
{
    return *defineType(0);
}


VrmlNodePlaneSensor::VrmlNodePlaneSensor( VrmlScene *scene ) :
  VrmlNodeChild(scene),
  d_autoOffset(true),
  d_enabled(true),
  d_maxPosition(-1.0, -1.0),
  d_isActive(false),
  d_parentTransform(0)
{
  setModified();
  Midentity(d_activationMatrix);
}

// need copy constructor for d_parentTransform ...

VrmlNodePlaneSensor::~VrmlNodePlaneSensor()
{
}


VrmlNode *VrmlNodePlaneSensor::cloneMe() const
{
  return new VrmlNodePlaneSensor(*this);
}


VrmlNodePlaneSensor* VrmlNodePlaneSensor::toPlaneSensor() const
{ return (VrmlNodePlaneSensor*) this; }


ostream& VrmlNodePlaneSensor::printFields(ostream& os, int indent)
{
  if (! d_autoOffset.get()) PRINT_FIELD(autoOffset);
  if (! d_enabled.get())    PRINT_FIELD(enabled);
  if (! FPEQUAL(d_maxPosition.getX(), -1.0) ||
      ! FPEQUAL(d_maxPosition.getY(), -1.0))
    PRINT_FIELD(maxPosition);
  if (! FPEQUAL(d_minPosition.getX(), -1.0) ||
      ! FPEQUAL(d_minPosition.getY(), -1.0))
    PRINT_FIELD(minPosition);

  if (! FPZERO(d_offset.getX()) ||
      ! FPZERO(d_offset.getY()) ||
      ! FPZERO(d_offset.getZ()) )
    PRINT_FIELD(offset);

  return os;
}

// Cache a pointer to (one of the) parent transforms for converting
// hits into local coords.

void VrmlNodePlaneSensor::accumulateTransform( VrmlNode *parent )
{
  d_parentTransform = parent;
}

VrmlNode* VrmlNodePlaneSensor::getParentTransform() { return d_parentTransform; }


// This is not correct. The local coords are computed for one instance,
// need to convert p to local coords for each instance (DEF/USE) of the 
// sensor...

void VrmlNodePlaneSensor::activate( double timeStamp,
				    bool isActive,
				    double *p )
{
  // Become active
  if ( isActive && ! d_isActive.get() )
    {
      d_isActive.set(isActive);

      float V[3] = { p[0], p[1], p[2] };
      //double M[4][4];
      inverseTransform( d_activationMatrix );
      VM( V, d_activationMatrix, V );
      d_activationPoint.set(V);
#if 0
      theSystem->warn(" planesensor: activate at (%g %g %g)\n",
		      p[0],p[1],p[2]);
      theSystem->warn(" planesensor: local coord (%g %g %g)\n",
		      V[0],V[1],V[2]);
#endif
      eventOut( timeStamp, "isActive", d_isActive );
    }

  // Become inactive
  else if ( ! isActive && d_isActive.get() )
    {
#if 0
      theSystem->warn(" planesensor: deactivate\n");
#endif
      d_isActive.set(isActive);
      eventOut( timeStamp, "isActive", d_isActive );

      // auto offset
      if ( d_autoOffset.get() )
	{
	  d_offset = d_translation;
	  eventOut( timeStamp, "offset_changed", d_offset );
	}
    }

  // Tracking
  else if ( isActive )
    {
      float V[3] = { p[0], p[1], p[2] };
      //double M[4][4];
      //inverseTransform( M );
      VM( V, d_activationMatrix, V );
      d_trackPoint.set(V);
      eventOut( timeStamp, "trackPoint_changed", d_trackPoint );

      float t[3];
      t[0] = V[0] - d_activationPoint.getX() + d_offset.getX();
      t[1] = V[1] - d_activationPoint.getY() + d_offset.getY();
      t[2] = 0.0;

      if ( d_minPosition.getX() == d_maxPosition.getX() )
	t[0] = d_minPosition.getX();
      else if ( d_minPosition.getX() < d_maxPosition.getX() )
	{
	  if (t[0] < d_minPosition.getX())
	    t[0] = d_minPosition.getX();
	  else if (t[0] > d_maxPosition.getX())
	    t[0] = d_maxPosition.getX();
	}

      if ( d_minPosition.getY() == d_maxPosition.getY() )
	t[1] = d_minPosition.getY();
      else if ( d_minPosition.getY() < d_maxPosition.getY() )
	{
	  if (t[1] < d_minPosition.getY())
	    t[1] = d_minPosition.getY();
	  else if (t[1] > d_maxPosition.getY())
	    t[1] = d_maxPosition.getY();
	}

      d_translation.set(t);
      eventOut( timeStamp, "translation_changed", d_translation );
    }
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodePlaneSensor::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "autoOffset" ) == 0 )
    return &d_autoOffset;
  else if ( strcmp( fieldName, "enabled" ) == 0 )
    return &d_enabled;
  else if ( strcmp( fieldName, "enabled" ) == 0 )
    return &d_enabled;
  else if ( strcmp( fieldName, "enabled" ) == 0 )
    return &d_enabled;
  else if ( strcmp( fieldName, "enabled" ) == 0 )
    return &d_enabled;
  
  // eventOuts
  else if ( strcmp( fieldName, "isActive" ) == 0 )
    return &d_isActive;
  else if ( strcmp( fieldName, "translation" ) == 0 )
    return &d_translation;
  else if ( strcmp( fieldName, "trackPoint" ) == 0 )
    return &d_trackPoint;
  
  return VrmlNodeChild::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodePlaneSensor::setField(const char *fieldName,
				   const VrmlField &fieldValue)
{
  if TRY_FIELD(autoOffset, SFBool)
  else if TRY_FIELD(enabled, SFBool)
  else if TRY_FIELD(maxPosition, SFVec2f)
  else if TRY_FIELD(minPosition, SFVec2f)
  else if TRY_FIELD(offset, SFVec3f)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}

