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

#include "VrmlNodeCoordinateInt.h"
#include "VrmlNodeType.h"
#include "VrmlNodeVisitor.h"
#include "VrmlScene.h"

// CoordinateInt factory.

static VrmlNode *creator( VrmlScene *scene ) 
{
  return new VrmlNodeCoordinateInt(scene);
}


// Define the built in VrmlNodeType:: "CoordinateInterpolator" fields

VrmlNodeType *VrmlNodeCoordinateInt::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("CoordinateInterpolator", creator);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addEventIn("set_fraction", VrmlField::SFFLOAT);
  t->addExposedField("key", VrmlField::MFFLOAT);
  t->addExposedField("keyValue", VrmlField::MFVEC3F);
  t->addEventOut("value_changed", VrmlField::MFVEC3F);

  return t;
}

VrmlNodeType & VrmlNodeCoordinateInt::nodeType() const
{
    return *defineType(0);
}


VrmlNodeCoordinateInt::VrmlNodeCoordinateInt( VrmlScene *scene ) :
  VrmlNodeChild(scene)
{
}

VrmlNodeCoordinateInt::~VrmlNodeCoordinateInt()
{
}

bool VrmlNodeCoordinateInt::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

ostream& VrmlNodeCoordinateInt::printFields(ostream& os, int indent)
{
  if (d_key.getLength() > 0) PRINT_FIELD(key);
  if (d_keyValue.getLength() > 0) PRINT_FIELD(keyValue);

  return os;
}

void VrmlNodeCoordinateInt::eventIn(double timeStamp,
				    const char *eventName,
				    const VrmlField & fieldValue)
{
  if (strcmp(eventName, "set_fraction") == 0)
    {
      if (!dynamic_cast<const VrmlSFFloat *>(&fieldValue))
	{
	  theSystem->error
	    ("Invalid type for %s eventIn %s (expected SFFloat).\n",
	     nodeType().getName(), eventName);
	  return;
	}
      float f = static_cast<const VrmlSFFloat &>(fieldValue).get();

      size_t nCoords = d_keyValue.getLength() / d_key.getLength();
      size_t n = d_key.getLength() - 1;

      if (f < d_key[0])
	{
	  d_value.set( nCoords, d_keyValue[0] );
	}
      else if (f > d_key[n])
	{
	  d_value.set( nCoords, d_keyValue[n*nCoords] );
	}
      else
	{
	  // Reserve enough space for the new value
	  d_value.set( nCoords, 0 );

	  for (int i=0; i<n; ++i)
	    if (d_key[i] <= f && f <= d_key[i+1])
	      {
		const float * v1 = d_keyValue[i*nCoords];
		const float * v2 = d_keyValue[(i+1)*nCoords];

		f = (f - d_key[i]) / (d_key[i+1] - d_key[i]);

		for (size_t j = 0; j < nCoords; ++j)
		  {
		    this->d_value[j][0] = v1[0] + f * (v2[0] - v1[0]);
		    this->d_value[j][1] = v1[1] + f * (v2[1] - v1[1]);
		    this->d_value[j][2] = v1[2] + f * (v2[2] - v1[2]);
		    v1 += 3;
		    v2 += 3;
		  }

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

const VrmlField *VrmlNodeCoordinateInt::getField(const char *fieldName) const
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

void VrmlNodeCoordinateInt::setField(const char *fieldName,
				     const VrmlField &fieldValue)
{
  if TRY_FIELD(key, MFFloat)
  else if TRY_FIELD(keyValue, MFVec3f)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}
