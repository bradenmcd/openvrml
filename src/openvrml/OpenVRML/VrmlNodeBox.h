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

#ifndef VRMLNODEBOX_H
#define VRMLNODEBOX_H

#include "VrmlNodeGeometry.h"
#include "field.h"
#include "VrmlBSphere.h"

class VrmlScene;

class VrmlNodeBox : public VrmlNodeGeometry {

public:

  // Define the fields of box nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeBox(VrmlScene *);
  virtual ~VrmlNodeBox();

  virtual bool accept(VrmlNodeVisitor & visitor);
  
  virtual ostream& printFields(ostream& os, int indent);

  virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  virtual VrmlNodeBox* toBox() const; //LarryD Mar 08/99
  virtual const VrmlSFVec3f& getSize() const;  //LarryD Mar 08/99

  const VrmlBVolume* getBVolume() const;

protected:

  VrmlSFVec3f d_size;
  VrmlBSphere d_bsphere;

};

#endif VRMLNODEBOX_H
