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

#ifndef  _VRMLNODESPHERE_
#define  _VRMLNODESPHERE_

#include "VrmlNodeGeometry.h"
#include "field.h"
#include "VrmlBSphere.h"

class VrmlNodeSphere : public VrmlNodeGeometry {

public:

  // Define the fields of sphere nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeSphere(VrmlScene *);
  virtual ~VrmlNodeSphere();

  virtual bool accept(VrmlNodeVisitor & visitor);
  
  virtual ostream& printFields(ostream& os, int indent);

  virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  virtual VrmlNodeSphere* toSphere() const; //LarryD Mar 08/99
  virtual float getRadius() { return d_radius.get(); }   //LarryD Mar 08/99

  const VrmlBVolume* getBVolume() const;

protected:

  VrmlSFFloat d_radius;
  VrmlBSphere d_bsphere;

};

#endif _VRMLNODESPHERE_

