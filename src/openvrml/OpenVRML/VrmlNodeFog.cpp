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

#include "VrmlNodeFog.h"
#include "VrmlNodeType.h"
#include "VrmlScene.h"
#include "VrmlSFBool.h"
#include "Viewer.h"


//  Fog factory.
//  Since Fog is a bindable child node, the first one created needs
//  to notify its containing scene.

static VrmlNode *creator(VrmlScene *scene)
{
  return new VrmlNodeFog(scene);
}


// Define the built in VrmlNodeType:: "Fog" fields

VrmlNodeType *VrmlNodeFog::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Fog", creator);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addEventIn("set_bind", VrmlField::SFBOOL);
  t->addExposedField("color", VrmlField::SFCOLOR);
  t->addExposedField("fogType", VrmlField::SFSTRING);
  t->addExposedField("visibilityRange", VrmlField::SFFLOAT);
  t->addEventOut("isBound", VrmlField::SFBOOL);

  return t;
}


VrmlNodeType & VrmlNodeFog::nodeType() const
{
    return *defineType(0);
}


VrmlNodeFog::VrmlNodeFog(VrmlScene *scene) :
  VrmlNodeChild(scene),
  d_color(1.0, 1.0, 1.0),
  d_fogType("LINEAR"),
  d_visibilityRange(0.0),
  d_isBound(false)
{
  if (d_scene) d_scene->addFog(this);
}

VrmlNodeFog::~VrmlNodeFog()
{
  if (d_scene) d_scene->removeFog(this);
}


VrmlNode *VrmlNodeFog::cloneMe() const
{
  return new VrmlNodeFog(*this);
}


VrmlNodeFog* VrmlNodeFog::toFog() const
{ return (VrmlNodeFog*) this; }


void VrmlNodeFog::addToScene(VrmlScene *s, const char *)
{ if (d_scene != s && (d_scene = s) != 0) d_scene->addFog(this); }


ostream& VrmlNodeFog::printFields(ostream& os, int indent)
{
  if (d_color.getR() != 1.0 ||
      d_color.getG() != 1.0 ||
      d_color.getB() != 1.0 )
    PRINT_FIELD(color);
  if (strcmp(d_fogType.get(),"LINEAR") != 0)
    PRINT_FIELD(fogType);
  if (d_visibilityRange.get() != 0.0)
    PRINT_FIELD(visibilityRange);

  return os;
}

// Note that this method is not maintaining isBound.

void VrmlNodeFog::eventIn(double timeStamp,
			  const char *eventName,
			  const VrmlField *fieldValue)
{
  if (strcmp(eventName, "set_bind") == 0)
    {
      VrmlNodeFog *current = d_scene->bindableFogTop();
      const VrmlSFBool *b = fieldValue->toSFBool();
      
      if (! b)
	{
	  cerr << "Error: invalid value for Fog::set_bind eventIn "
	       << (*fieldValue) << endl;
	  return;
	}

      if ( b->get() )		// set_bind TRUE
	{
	  if (this != current)
	    {
	      if (current)
		current->eventOut( timeStamp, "isBound", VrmlSFBool(false));
	      d_scene->bindablePush( this );
	      eventOut( timeStamp, "isBound", VrmlSFBool(true) );
	    }
	}
      else			// set_bind FALSE
	{
	  d_scene->bindableRemove( this );
	  if (this == current)
	    {
	      eventOut( timeStamp, "isBound", VrmlSFBool(false));
	      current = d_scene->bindableFogTop();
	      if (current)
		current->eventOut( timeStamp, "isBound", VrmlSFBool(true) );
	    }
	}
    }

  else
    {
      VrmlNode::eventIn(timeStamp, eventName, fieldValue);
    }
}

// Get the value of a field or eventOut.
// The isBound eventOut is only set when queried,
// don't rely on it's value to be valid. This hoses
// the const-ness of the method, of course :(

const VrmlField *VrmlNodeFog::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "color" ) == 0 )
    return &d_color;
  else if ( strcmp( fieldName, "fogType" ) == 0 )
    return &d_fogType;
  else if ( strcmp( fieldName, "visibilityRange" ) == 0 )
    return &d_visibilityRange;
  
  // eventOuts
  else if ( strcmp( fieldName, "isBound" ) == 0 )
    {
      VrmlSFBool* isBound = (VrmlSFBool*) &(this->d_isBound);
      isBound->set( d_scene->bindableFogTop() == this );
      return isBound;
    }

  return VrmlNodeChild::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeFog::setField(const char *fieldName,
			   const VrmlField &fieldValue)
{
  if TRY_FIELD(color, SFColor)
  else if TRY_FIELD(fogType, SFString)
  else if TRY_FIELD(visibilityRange, SFFloat)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}

