//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//  See the file COPYING for license details.
//
//  VrmlNodeCylinderSensor.cpp

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "VrmlNodeCylinderSensor.h"
#include "VrmlNodeType.h"
#include "MathUtils.h"
#include "System.h"
#include "Viewer.h"
#include "VrmlScene.h"

// CylinderSensor factory. 

static VrmlNode *creator( VrmlScene *scene ) 
{
  return new VrmlNodeCylinderSensor(scene);
}


// Define the built in VrmlNodeType:: "CylinderSensor" fields

VrmlNodeType *VrmlNodeCylinderSensor::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("CylinderSensor", creator);
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addExposedField("autoOffset", VrmlField::SFBOOL);
  t->addExposedField("diskAngle", VrmlField::SFFLOAT);
  t->addExposedField("enabled", VrmlField::SFBOOL);
  t->addExposedField("maxAngle", VrmlField::SFFLOAT);
  t->addExposedField("minAngle", VrmlField::SFFLOAT);
  t->addExposedField("offset", VrmlField::SFFLOAT);
  t->addEventOut("isActive", VrmlField::SFBOOL);
  t->addEventOut("rotation_changed", VrmlField::SFROTATION);
  t->addEventOut("trackPoint_changed", VrmlField::SFVEC3F);

  return t;
}

VrmlNodeType & VrmlNodeCylinderSensor::nodeType() const 
{
    return *defineType(0);
}


VrmlNodeCylinderSensor::VrmlNodeCylinderSensor( VrmlScene *scene ) :
  VrmlNodeChild(scene),
  d_autoOffset(true),
  d_diskAngle(0.262),
  d_enabled(true),
  d_maxAngle(-1.0),
  d_minAngle(0.0),
  d_offset(0.0),
  d_isActive(false)
{
  setModified();
}


VrmlNodeCylinderSensor::~VrmlNodeCylinderSensor()
{
}


VrmlNode *VrmlNodeCylinderSensor::cloneMe() const
{
  return new VrmlNodeCylinderSensor(*this);
}


ostream& VrmlNodeCylinderSensor::printFields(ostream& os, int indent)
{
  if (! d_autoOffset.get()) PRINT_FIELD(autoOffset);
  if (! FPEQUAL(d_diskAngle.get(), 0.262)) PRINT_FIELD(diskAngle);
  if (! d_enabled.get())    PRINT_FIELD(enabled);
  if (! FPEQUAL(d_maxAngle.get(), -1.0)) PRINT_FIELD(maxAngle);
  if (! FPZERO(d_minAngle.get())) PRINT_FIELD(minAngle);
  if (! FPZERO(d_offset.get())) PRINT_FIELD(offset);

  return os;
}

const VrmlField *VrmlNodeCylinderSensor::getField(const char *fieldName) const
{
  if ( strcmp( fieldName, "autoOffset" ) == 0 )
    return &d_autoOffset;
  else if ( strcmp( fieldName, "diskAngle" ) == 0 )
    return &d_diskAngle;
  else if ( strcmp( fieldName, "enabled" ) == 0 )
    return &d_enabled;
  else if ( strcmp( fieldName, "maxAngle" ) == 0 )
    return &d_maxAngle;
  else if ( strcmp( fieldName, "minAngle" ) == 0 )
    return &d_minAngle;
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

void VrmlNodeCylinderSensor::setField(const char *fieldName,
				      const VrmlField &fieldValue)
{
  if TRY_FIELD(autoOffset, SFBool)
  else if TRY_FIELD(diskAngle, SFFloat)
  else if TRY_FIELD(enabled, SFBool)
  else if TRY_FIELD(maxAngle, SFFloat)
  else if TRY_FIELD(minAngle, SFFloat)
  else if TRY_FIELD(offset, SFFloat)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}

