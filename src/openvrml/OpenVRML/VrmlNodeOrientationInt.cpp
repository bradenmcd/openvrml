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

#ifdef _WIN32
#include <winconfig.h>
#endif

#include "VrmlNodeOrientationInt.h"
#include "VrmlNodeType.h"
#include "VrmlScene.h"
#include <math.h>

// OrientationInt factory.

static VrmlNode *creator( VrmlScene *scene ) 
{
  return new VrmlNodeOrientationInt(scene);
}


// Define the built in VrmlNodeType:: "OrientationInt" fields

VrmlNodeType *VrmlNodeOrientationInt::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("OrientationInterpolator", creator);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addEventIn("set_fraction", VrmlField::SFFLOAT);
  t->addExposedField("key", VrmlField::MFFLOAT);
  t->addExposedField("keyValue", VrmlField::MFROTATION);
  t->addEventOut("value_changed", VrmlField::SFROTATION);

  return t;
}

VrmlNodeType & VrmlNodeOrientationInt::nodeType() const
{
    return *defineType(0);
}


VrmlNodeOrientationInt::VrmlNodeOrientationInt( VrmlScene *scene ) :
  VrmlNodeChild(scene)
{
}

VrmlNodeOrientationInt::~VrmlNodeOrientationInt()
{
}


VrmlNode *VrmlNodeOrientationInt::cloneMe() const
{
  return new VrmlNodeOrientationInt(*this);
}


ostream& VrmlNodeOrientationInt::printFields(ostream& os, int indent)
{
  if (d_key.size() > 0) PRINT_FIELD(key);
  if (d_keyValue.size() > 0) PRINT_FIELD(keyValue);

  return os;
}


void VrmlNodeOrientationInt::eventIn(double timeStamp,
				     const char *eventName,
				     const VrmlField *fieldValue)
{
  if (strcmp(eventName, "set_fraction") == 0)
    {
      if (! fieldValue->toSFFloat() )
	{
	  theSystem->error
	    ("Invalid type for %s eventIn %s (expected SFFloat).\n",
		nodeType().getName(), eventName);
	  return;
	}
      float f = fieldValue->toSFFloat()->get();

      //printf("OI.set_fraction %g ", f);

      int n = d_key.size() - 1;
      if (f < d_key[0])
	{
	  const float * v0 = d_keyValue[0];
	  //printf(" 0 [%g %g %g %g]\n", v0[0], v0[1], v0[2], v0[3] );
	  d_value.set(v0);
	}
      else if (f > d_key[n])
	{
	  const float * vn = d_keyValue[n];
	  //printf(" n [%g %g %g %g]\n", vn[0], vn[1], vn[2], vn[3] );
	  d_value.set(vn);
	}
      else
	{
	  for (int i=0; i<n; ++i)
	    if (d_key[i] <= f && f <= d_key[i+1])
	      {
		const float * v1 = d_keyValue[i];
		const float * v2 = d_keyValue[i+1];

		// Interpolation factor
		f = (f - d_key[i]) / (d_key[i+1] - d_key[i]);

		float x, y, z, r1, r2;
		r1 = v1[3];

		// Make sure the vectors are not pointing opposite ways
		if (v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2] < 0.0)
		  {
		    x = v1[0] + f * (-v2[0] - v1[0]);
		    y = v1[1] + f * (-v2[1] - v1[1]);
		    z = v1[2] + f * (-v2[2] - v1[2]);
		    r2 = -v2[3];
		  }
		else
		  {
		    x = v1[0] + f * (v2[0] - v1[0]);
		    y = v1[1] + f * (v2[1] - v1[1]);
		    z = v1[2] + f * (v2[2] - v1[2]);
		    r2 = v2[3];
		  }

		// Interpolate angles via the shortest direction
		if (fabs(r2 - r1) > M_PI)
		  {
		    if (r2 > r1) r1 += 2.0 * M_PI;
		    else         r2 += 2.0 * M_PI;
		  }
		float angle = r1 + f * (r2 - r1);
		if (angle >= 2.0 * M_PI) angle -= 2.0 * M_PI;
		else if (angle < 0.0)    angle += 2.0 * M_PI;
		
		//printf(" %g between (%d,%d) [%g %g %g %g]\n", f, i, i+1,
		//x, y, z, r);
                const float rotation[4] = { x, y, z, angle };
		d_value.set(rotation);
		break;
	      }
	}

      // Send the new value
      eventOut(timeStamp, "value_changed", d_value);
    }

  // Check exposedFields
  else
    {
      VrmlNode::eventIn(timeStamp, eventName, fieldValue);

      // This node is not renderable, so don't re-render on changes to it.
      clearModified();
    }
}


// Get the value of a field or eventOut.

const VrmlField *VrmlNodeOrientationInt::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "key" ) == 0 )
    return &d_key;
  else if ( strcmp( fieldName, "keyValue" ) == 0 )
    return &d_keyValue;

  // eventOuts
  else if ( strcmp( fieldName, "value" ) == 0 )
    return &d_value;

  return VrmlNodeChild::getField( fieldName );
}


// Set the value of one of the node fields.

void VrmlNodeOrientationInt::setField(const char *fieldName,
				      const VrmlField &fieldValue)
{
  if TRY_FIELD(key, MFFloat)
  else if TRY_FIELD(keyValue, MFRotation)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}

VrmlNodeOrientationInt* VrmlNodeOrientationInt::toOrientationInt() const 
{ return (VrmlNodeOrientationInt*) this; }

const VrmlMFFloat& VrmlNodeOrientationInt::getKey() const  
{   return d_key; }

const VrmlMFRotation& VrmlNodeOrientationInt::getKeyValue() const  
{ return d_keyValue; }

