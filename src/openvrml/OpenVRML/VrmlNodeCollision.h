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

#ifndef VRMLNODECOLLISION_H
#define VRMLNODECOLLISION_H

#include "VrmlNodeGroup.h"
#include "VrmlSFBool.h"
#include "VrmlSFNode.h"
#include "VrmlSFTime.h"


class VrmlNodeCollision : public VrmlNodeGroup {

public:

  // Define the fields of Collision nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeCollision(VrmlScene *);
  virtual ~VrmlNodeCollision();

  virtual bool accept(VrmlNodeVisitor & visitor);
  virtual void resetVisitedFlag();
  
  virtual bool isModified() const;
  virtual void clearFlags();	// Clear childrens flags too.

  virtual void addToScene( VrmlScene *s, const char *rel );

  virtual ostream& printFields(ostream& os, int indent);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);
  
  const VrmlSFNode & getProxy() const;
  void setProxy(const VrmlSFNode & proxy);

private:

  VrmlSFBool d_collide;
  VrmlSFNode d_proxy;

  // eventOut
  VrmlSFTime d_collideTime;
  
};

#endif // VRMLNODECOLLISION_H

