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

#include "VrmlNodeColorInt.h"
#include "VrmlNodeType.h"

#include <math.h>

#include "VrmlScene.h"

// ColorInterpolator factory.

static VrmlNode *creator( VrmlScene *scene ) 
{
  return new VrmlNodeColorInt(scene);
}


// Define the built in VrmlNodeType:: "ColorInterpolator" fields

VrmlNodeType *VrmlNodeColorInt::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("ColorInterpolator", creator);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addEventIn("set_fraction", VrmlField::SFFLOAT);
  t->addExposedField("key", VrmlField::MFFLOAT);
  t->addExposedField("keyValue", VrmlField::MFCOLOR);
  t->addEventOut("value_changed", VrmlField::SFCOLOR);

  return t;
}

VrmlNodeType & VrmlNodeColorInt::nodeType() const
{
    return *defineType(0);
}


VrmlNodeColorInt::VrmlNodeColorInt( VrmlScene *scene ) :
  VrmlNodeChild(scene)
{
}

VrmlNodeColorInt::~VrmlNodeColorInt()
{
}


VrmlNode *VrmlNodeColorInt::cloneMe() const
{
  return new VrmlNodeColorInt(*this);
}


ostream& VrmlNodeColorInt::printFields(ostream& os, int indent)
{
  if (d_key.size() > 0) PRINT_FIELD(key);
  if (d_keyValue.size() > 0) PRINT_FIELD(keyValue);

  return os;
}


void VrmlNodeColorInt::eventIn(double timeStamp,
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

      int n = d_key.size() - 1;
      if (f < d_key[0])
	d_value.set( d_keyValue[0][0], d_keyValue[0][1], d_keyValue[0][2] );
      else if (f > d_key[n])
	d_value.set( d_keyValue[n][0], d_keyValue[n][1], d_keyValue[n][2] );
      else
	{
	  //  convert to HSV for the interpolation...
	  for (int i=0; i<n; ++i)
	    if (d_key[i] <= f && f <= d_key[i+1])
	      {
		float *rgb1 = d_keyValue[i];
		float *rgb2 = d_keyValue[i+1];

		f = (f - d_key[i]) / (d_key[i+1] - d_key[i]);
		float h1, s1, v1, h2, s2, v2;
		VrmlSFColor::RGBtoHSV( rgb1[0], rgb1[1], rgb1[2],
				       h1, s1, v1 );
		VrmlSFColor::RGBtoHSV( rgb2[0], rgb2[1], rgb2[2],
				       h2, s2, v2 );

		// Interpolate angles via the shortest direction
		if (fabs(h2 - h1) > 180.0)
		  {
		    if (h2 > h1) h1 += 360.0;
		    else         h2 += 360.0;
		  }
		float h = h1 + f * (h2 - h1);
		if (h >= 360.0) h -= 360.0;
		else if (h < 0.0) h += 360.0;

		float s = s1 + f * (s2 - s1);
		float v = v1 + f * (v2 - v1);
		float r, g, b;
		VrmlSFColor::HSVtoRGB( h, s, v, r, g, b);
		d_value.set( r, g, b);
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

const VrmlField *VrmlNodeColorInt::getField(const char *fieldName) const
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

void VrmlNodeColorInt::setField(const char *fieldName,
				const VrmlField &fieldValue)
{
  if TRY_FIELD(key, MFFloat)
  else if TRY_FIELD(keyValue, MFColor)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}
