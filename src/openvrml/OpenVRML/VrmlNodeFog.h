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

#ifndef VRMLNODEFOG_H
#define VRMLNODEFOG_H

#include "VrmlNodeChild.h"
#include "field.h"

class VrmlScene;

class VrmlNodeFog : public VrmlNodeChild {

public:

  // Define the fields of Fog nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeFog(VrmlScene *);
  virtual ~VrmlNodeFog();

  virtual bool accept(VrmlNodeVisitor & visitor);
  
  virtual VrmlNodeFog* toFog() const;

  virtual void addToScene( VrmlScene *s, const char *relUrl );

  virtual ostream& printFields(ostream& os, int indent);

  virtual void eventIn(double timeStamp,
		       const char *eventName,
		       const VrmlField & fieldValue);

  // Get a field or eventOut by name.
  virtual const VrmlField *getField(const char *fieldName) const;
  
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  const float *color() const	{ return d_color.get(); }
  const char *fogType()	const	{ return d_fogType.get(); }
  float visibilityRange() const	{ return d_visibilityRange.get(); }

private:

  VrmlSFColor d_color;
  VrmlSFString d_fogType;
  VrmlSFFloat d_visibilityRange;

  VrmlSFBool d_isBound;		// eventOut

};

#endif
