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

#include "VrmlNodeTouchSensor.h"
#include "VrmlNodeType.h"
#include "VrmlNodeVisitor.h"
#include "MathUtils.h"
#include "System.h"
#include "Viewer.h"
#include "VrmlScene.h"

// TouchSensor factory. 

static VrmlNode *creator( VrmlScene *scene ) 
{
  return new VrmlNodeTouchSensor(scene);
}


// Define the built in VrmlNodeType:: "TouchSensor" fields

VrmlNodeType *VrmlNodeTouchSensor::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("TouchSensor", creator);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addExposedField("enabled", VrmlField::SFBOOL);
  t->addEventOut("hitNormal_changed", VrmlField::SFVEC3F);
  t->addEventOut("hitPoint_changed", VrmlField::SFVEC3F);
  t->addEventOut("hitTexCoord_changed", VrmlField::SFVEC2F);
  t->addEventOut("isActive", VrmlField::SFBOOL);
  t->addEventOut("isOver", VrmlField::SFBOOL);
  t->addEventOut("touchTime", VrmlField::SFTIME);

  return t;
}

VrmlNodeType & VrmlNodeTouchSensor::nodeType() const
{
    return *defineType(0);
}


VrmlNodeTouchSensor::VrmlNodeTouchSensor( VrmlScene *scene ) :
  VrmlNodeChild(scene),
  d_enabled(true),
  d_isActive(false),
  d_isOver(false),
  d_touchTime(0.0)
{
  setModified();
}


VrmlNodeTouchSensor::~VrmlNodeTouchSensor()
{
}

bool VrmlNodeTouchSensor::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodeTouchSensor* VrmlNodeTouchSensor::toTouchSensor() const
{ return (VrmlNodeTouchSensor*) this; }



ostream& VrmlNodeTouchSensor::printFields(ostream& os, int indent)
{
  if (! d_enabled.get())
    PRINT_FIELD(enabled);
  return os;
}

// Doesn't compute the xxx_changed eventOuts yet...

void VrmlNodeTouchSensor::activate( double timeStamp,
				    bool isOver, bool isActive,
				    double * )
{
  if (isOver && !isActive && d_isActive.get())
    {
      d_touchTime.set(timeStamp);
      eventOut( timeStamp, "touchTime", d_touchTime);
      //theSystem->debug("TouchSensor.%s touchTime\n", name());
    }

  if (isOver != d_isOver.get())
    {
      d_isOver.set(isOver);
      eventOut( timeStamp, "isOver", d_isOver );
    }

  if (isActive != d_isActive.get())
    {
      d_isActive.set(isActive);
      eventOut( timeStamp, "isActive", d_isActive );
    }

  // if (isOver && any routes from eventOuts)
  //   generate xxx_changed eventOuts...

}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeTouchSensor::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "enabled" ) == 0 )
    return &d_enabled;

  // eventOuts
  else if ( strcmp( fieldName, "hitNormal" ) == 0 )
    return &d_hitNormal;
  else if ( strcmp( fieldName, "hitPoint" ) == 0 )
    return &d_hitPoint;
  else if ( strcmp( fieldName, "hitTexCoord" ) == 0 )
    return &d_hitTexCoord;
  else if ( strcmp( fieldName, "isActive" ) == 0 )
    return &d_isActive;
  else if ( strcmp( fieldName, "isOver" ) == 0 )
    return &d_isOver;
  else if ( strcmp( fieldName, "touchTime" ) == 0 )
    return &d_touchTime;

  return VrmlNodeChild::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeTouchSensor::setField(const char *fieldName,
				   const VrmlField &fieldValue)
{
  if TRY_FIELD(enabled, SFBool)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}

