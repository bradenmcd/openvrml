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

#ifndef  _VRMLNODEPOINTSET_
#define  _VRMLNODEPOINTSET_

#include "VrmlNodeGeometry.h"
#include "VrmlSFNode.h"
#include "VrmlBSphere.h"


class VrmlNodePointSet : public VrmlNodeGeometry {

public:

  // Define the fields of pointSet nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodePointSet(VrmlScene *);
  virtual ~VrmlNodePointSet();

  virtual VrmlNode *cloneMe() const;
  virtual void cloneChildren(VrmlNamespace *);
  virtual void updateModified(VrmlNodePath& path);
  virtual bool isModified() const;

  virtual void clearFlags();

  virtual void addToScene( VrmlScene *s, const char *relUrl );

  virtual void copyRoutes(VrmlNamespace *ns) const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  const VrmlSFNode & getColor() const;
  void setColor(const VrmlSFNode & color);
  
  const VrmlSFNode & getCoord() const;
  void setCoord(const VrmlSFNode & coord);
  
  const VrmlBVolume* getBVolume() const;

protected:

  VrmlSFNode d_color;
  VrmlSFNode d_coord;

  VrmlBSphere d_bsphere;
  void recalcBSphere();
};

#endif _VRMLNODEPOINTSET_
