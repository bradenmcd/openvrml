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

#ifndef VRMLNODENAVIGATIONINFO_H
#define VRMLNODENAVIGATIONINFO_H

#include "VrmlNodeChild.h"
#include "VrmlMFFloat.h"
#include "VrmlMFString.h"
#include "VrmlSFBool.h"
#include "VrmlSFFloat.h"

class VrmlNodeNavigationInfo : public VrmlNodeChild {

public:

  // Define the built in VrmlNodeType:: "NavigationInfo"
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeNavigationInfo(VrmlScene *scene);
  virtual ~VrmlNodeNavigationInfo();

  virtual VrmlNode *cloneMe() const;

  virtual VrmlNodeNavigationInfo* toNavigationInfo() const;

  // Bindable nodes must notify the scene of their existence.
  virtual void addToScene( VrmlScene *s, const char *relUrl );

  virtual ostream& printFields(ostream& os, int indent);

  virtual void eventIn(double timeStamp,
		       const char *eventName,
		       const VrmlField *fieldValue);

  // Get a field or eventOut by name.
  virtual const VrmlField *getField(const char *fieldName) const;
  
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  const float * avatarSize()		{ return d_avatarSize.get(); }
  bool headlightOn()		{ return d_headlight.get(); }
  float speed()			{ return d_speed.get(); }
  float visibilityLimit()	{ return d_visibilityLimit.get(); }

private:

  VrmlMFFloat d_avatarSize;
  VrmlSFBool d_headlight;
  VrmlSFFloat d_speed;
  VrmlMFString d_type;
  VrmlSFFloat d_visibilityLimit;

  VrmlSFBool d_isBound;		// eventOut
};

#endif
