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

#include "VrmlNodeProximitySensor.h"
#include "VrmlNodeType.h"
#include "VrmlNodeVisitor.h"
#include "MathUtils.h"
#include "System.h"
#include "Viewer.h"
#include "VrmlScene.h"

// ProximitySensor factory. 

static VrmlNode *creator( VrmlScene *scene ) 
{
  return new VrmlNodeProximitySensor(scene);
}


// Define the built in VrmlNodeType:: "ProximitySensor" fields

VrmlNodeType *VrmlNodeProximitySensor::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("ProximitySensor", creator);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addExposedField("center", VrmlField::SFVEC3F);
  t->addExposedField("enabled", VrmlField::SFBOOL);
  t->addExposedField("size", VrmlField::SFVEC3F);
  t->addEventOut("enterTime", VrmlField::SFTIME);
  t->addEventOut("exitTime", VrmlField::SFTIME);
  t->addEventOut("isActive", VrmlField::SFBOOL);
  t->addEventOut("orientation_changed", VrmlField::SFROTATION);
  t->addEventOut("position_changed", VrmlField::SFVEC3F);

  return t;
}

VrmlNodeType & VrmlNodeProximitySensor::nodeType() const
{
    return *defineType(0);
}


VrmlNodeProximitySensor::VrmlNodeProximitySensor( VrmlScene *scene ) :
  VrmlNodeChild(scene),
  d_center(0.0, 0.0, 0.0),
  d_enabled(true),
  d_size(0.0, 0.0, 0.0),
  d_isActive(false),
  d_position(0.0, 0.0, 0.0),
  d_enterTime(0.0),
  d_exitTime(0.0)
{
  setModified();
}

VrmlNodeProximitySensor::~VrmlNodeProximitySensor()
{
}

bool VrmlNodeProximitySensor::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

ostream& VrmlNodeProximitySensor::printFields(ostream& os, int indent)
{
  if (! FPZERO(d_center.getX()) ||
      ! FPZERO(d_center.getY()) ||
      ! FPZERO(d_center.getZ()) )
    PRINT_FIELD(center);
  if (! d_enabled.get())
    PRINT_FIELD(enabled);
  if (! FPZERO(d_size.getX()) ||
      ! FPZERO(d_size.getY()) ||
      ! FPZERO(d_size.getZ()) )
    PRINT_FIELD(size);
      
  return os;
}

//
// Generate proximity events. If necessary, events prior to the current
// time are generated due to interpolation of enterTimes and exitTimes. 
// The timestamp should never be increased.
//
// This is in a render() method since the it needs the viewer position
// with respect to the local coordinate system.
// Could do this with VrmlNode::inverseTransform(double [4][4]) now...
//
// The positions and times are not interpolated to report the exact
// place and time of entries and exits from the sensor regions as
// required by the spec, since that would require the last viewer position
// to be stored in the node, which is problematic in the presence of
// DEF/USEd nodes...
// I suppose the scene could keep the last viewer position in the global
// coordinate system and it could be transformed all the way down the
// scenegraph, but that sounds painful.

void VrmlNodeProximitySensor::render(Viewer *viewer, VrmlRenderContext rc)
{
  if (d_enabled.get() &&
      d_size.getX() > 0.0 &&
      d_size.getY() > 0.0 &&
      d_size.getZ() > 0.0 &&
      viewer->getRenderMode() == Viewer::RENDER_MODE_DRAW)
    {
      VrmlSFTime timeNow( theSystem->time() );
      float x, y, z;

      // Is viewer inside the box?
      viewer->getPosition( &x, &y, &z );
      bool inside = (fabs(x - d_center.getX()) <= 0.5 * d_size.getX() &&
		     fabs(y - d_center.getY()) <= 0.5 * d_size.getY() &&
		     fabs(z - d_center.getZ()) <= 0.5 * d_size.getZ());
      bool wasIn = d_isActive.get();

      // Check if viewer has entered the box
      if (inside && ! wasIn)
	{
	  theSystem->debug("PS.%s::render ENTER %g %g %g\n", name(), x, y, z);

	  d_isActive.set(true);
	  eventOut(timeNow.get(), "isActive", d_isActive);

	  d_enterTime = timeNow;
	  eventOut(timeNow.get(), "enterTime", d_enterTime);
	}

      // Check if viewer has left the box
      else if (wasIn && ! inside)
	{
	  theSystem->debug("PS.%s::render EXIT %g %g %g\n", name(), x, y, z);

	  d_isActive.set(false);
	  eventOut(timeNow.get(), "isActive", d_isActive );

	  d_exitTime = timeNow;
	  eventOut(timeNow.get(), "exitTime", d_exitTime);
	}

      // Check for movement within the box
      if (wasIn || inside)
	{
	  if ( ! FPEQUAL(d_position.getX(), x) ||
	       ! FPEQUAL(d_position.getY(), y) ||
	       ! FPEQUAL(d_position.getZ(), z) )
	    {
	      const float positionVec[3] = { x, y, z };
              d_position.set(positionVec);
	      eventOut(timeNow.get(), "position_changed", d_position);
	    }

	  float xyzr[4];
	  viewer->getOrientation( xyzr );
	  if ( ! FPEQUAL(d_orientation.getX(), xyzr[0]) ||
	       ! FPEQUAL(d_orientation.getY(), xyzr[1]) ||
	       ! FPEQUAL(d_orientation.getZ(), xyzr[2]) ||
	       ! FPEQUAL(d_orientation.getAngle(), xyzr[3]) )
	    {
	      d_orientation.set(xyzr);
	      eventOut(timeNow.get(), "orientation_changed", d_orientation);
	    }
	}
    }

  else
    clearModified();
}


const VrmlField *VrmlNodeProximitySensor::getField(const char *fieldName) const
{
  if ( strcmp( fieldName, "center" ) == 0 )
    return &d_center;
  else if ( strcmp( fieldName, "enabled" ) == 0 )
    return &d_enabled;
  else if ( strcmp( fieldName, "size" ) == 0 )
    return &d_size;

  // eventOuts
  else if ( strcmp( fieldName, "isActive" ) == 0 )
    return &d_isActive;
  else if ( strcmp( fieldName, "position" ) == 0 )
    return &d_position;
  else if ( strcmp( fieldName, "orientation" ) == 0 )
    return &d_orientation;
  else if ( strcmp( fieldName, "enterTime" ) == 0 )
    return &d_enterTime;
  else if ( strcmp( fieldName, "exitTime" ) == 0 )
    return &d_exitTime;
  
  return VrmlNodeChild::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodeProximitySensor::setField(const char *fieldName,
				       const VrmlField &fieldValue)
{
  if TRY_FIELD(center, SFVec3f)
  else if TRY_FIELD(enabled, SFBool)
  else if TRY_FIELD(size, SFVec3f)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}

