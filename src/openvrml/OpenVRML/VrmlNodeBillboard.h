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
//  VrmlNodeBillboard.h

#ifndef VRMLNODEBILLBOARD_H
#define VRMLNODEBILLBOARD_H

#include "VrmlNodeGroup.h"
#include "VrmlSFVec3f.h"


class VrmlNodeBillboard : public VrmlNodeGroup {

public:

  // Define the fields of Billboard nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeBillboard(VrmlScene *);
  virtual ~VrmlNodeBillboard();

  virtual bool accept(VrmlNodeVisitor & visitor);
  
  virtual ostream& printFields(ostream& os, int indent);

  virtual void render(Viewer *, VrmlRenderContext rc);

  virtual void accumulateTransform(VrmlNode*);
  virtual VrmlNode* getParentTransform();
  virtual void inverseTransform(Viewer *);
  virtual void inverseTransform(double [4][4]);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  virtual const VrmlNodeBillboard * toBillboard() const;
  virtual VrmlNodeBillboard * toBillboard();

private:

  VrmlSFVec3f d_axisOfRotation;

  VrmlNode *d_parentTransform;
  Viewer::Object d_xformObject;
  
};

#endif // VRMLNODEBILLBOARD_H

