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

#ifndef VRMLNODEFONTSTYLE_H
#define VRMLNODEFONTSTYLE_H

#include "VrmlNode.h"
#include "VrmlMFString.h"
#include "VrmlSFBool.h"
#include "VrmlSFString.h"
#include "VrmlSFFloat.h"

class VrmlNodeFontStyle : public VrmlNode {

public:

  // Define the fields of FontStyle nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeFontStyle(VrmlScene *);
  virtual ~VrmlNodeFontStyle();

  virtual VrmlNode *cloneMe() const;

  virtual VrmlNodeFontStyle* toFontStyle() const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  VrmlMFString &justify() { return d_justify; }
  float size() { return d_size.get(); }

private:

  VrmlMFString d_family;
  VrmlSFBool d_horizontal;
  VrmlMFString d_justify;
  VrmlSFString d_language;
  VrmlSFBool d_leftToRight;
  VrmlSFFloat d_size;
  VrmlSFFloat d_spacing;
  VrmlSFString d_style;
  VrmlSFBool d_topToBottom;
  
};

#endif
