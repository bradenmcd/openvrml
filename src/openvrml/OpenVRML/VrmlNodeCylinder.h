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

#ifndef  _VRMLNODECYLINDER_
#define  _VRMLNODECYLINDER_

#include "VrmlNodeGeometry.h"
#include "VrmlSFBool.h"
#include "VrmlSFFloat.h"

class VrmlNodeCylinder : public VrmlNodeGeometry {

public:

  // Define the fields of cylinder nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeCylinder(VrmlScene *);
  virtual ~VrmlNodeCylinder();

  virtual bool accept(VrmlNodeVisitor & visitor);
  
  virtual ostream& printFields(ostream& os, int indent);

  virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  virtual VrmlNodeCylinder* toCylinder() const; //LarryD Mar 08/99
  virtual bool getBottom() { return d_bottom.get(); }  //LarryD Mar 08/99
  virtual bool getSide() { return d_side.get(); }  //LarryD Mar 08/99
  virtual bool getTop() { return d_top.get(); }  //LarryD Mar 08/99
  virtual float getHeight() { return d_height.get(); }  //LarryD Mar 08/99
  virtual float getRadius() { return d_radius.get(); }  //LarryD Mar 08/99

protected:

  VrmlSFBool d_bottom;
  VrmlSFFloat d_height;
  VrmlSFFloat d_radius;
  VrmlSFBool d_side;
  VrmlSFBool d_top;

};

#endif _VRMLNODECYLINDER_

