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

#ifndef VRMLNODECOORDINATEINT_H
#define VRMLNODECOORDINATEINT_H

#include "VrmlNodeChild.h"
#include "VrmlSFFloat.h"
#include "VrmlMFFloat.h"
#include "VrmlMFVec3f.h"

class VrmlScene;


class VrmlNodeCoordinateInt : public VrmlNodeChild {

public:

  // Define the fields of CoordinateInt nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeCoordinateInt( VrmlScene *scene = 0);
  virtual ~VrmlNodeCoordinateInt();

  virtual bool accept(VrmlNodeVisitor & visitor);
  
  virtual ostream& printFields(ostream& os, int indent);

  virtual void eventIn(double timeStamp,
		       const char *eventName,
		       const VrmlField & fieldValue);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

private:

  // Fields
  VrmlMFFloat d_key;
  VrmlMFVec3f d_keyValue;

  // State
  VrmlMFVec3f d_value;
};

#endif
