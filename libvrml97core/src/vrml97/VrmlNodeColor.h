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

#ifndef VRMLNODECOLOR_H
#define VRMLNODECOLOR_H

#include "VrmlNode.h"
#include "VrmlMFColor.h"

class VrmlScene;

class VrmlNodeColor : public VrmlNode {

public:

  // Define the fields of Color nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeColor(VrmlScene *);
  virtual ~VrmlNodeColor();

  virtual VrmlNode *cloneMe() const;

  virtual VrmlNodeColor* toColor() const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  VrmlMFColor &color()	{ return d_color; }

private:

  VrmlMFColor d_color;
  
};

#endif // VRMLNODECOLOR_H

