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

#ifndef VRMLNODETEXTURECOORDINATE_H
#define VRMLNODETEXTURECOORDINATE_H

#include "VrmlNode.h"
#include "VrmlMFVec2f.h"


class VrmlNodeTextureCoordinate : public VrmlNode {

public:

  // Define the fields of TextureCoordinate nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeTextureCoordinate(VrmlScene *);
  virtual ~VrmlNodeTextureCoordinate();

  virtual bool accept(VrmlNodeVisitor & visitor);
  
  virtual VrmlNodeTextureCoordinate* toTextureCoordinate() const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  VrmlMFVec2f &coordinate()	{ return d_point; }

private:

  VrmlMFVec2f d_point;
  
};

#endif
