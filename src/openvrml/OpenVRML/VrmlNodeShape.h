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

#ifndef  _VRMLNODESHAPE_
#define  _VRMLNODESHAPE_

#include "VrmlNodeChild.h"
#include "field.h"
#include "Viewer.h"

class VrmlNodeShape : public VrmlNodeChild {

public:

  // Define the fields of Shape nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeShape(VrmlScene *);
  virtual ~VrmlNodeShape();

  virtual bool accept(VrmlNodeVisitor & visitor);
  virtual void resetVisitedFlag();
  
  virtual bool isModified() const;
  virtual void updateModified(VrmlNodePath& path, int flags);
  virtual void clearFlags();
  const VrmlBVolume* getBVolume() const;

  virtual VrmlNodeShape* toShape()	const;

  virtual void addToScene( VrmlScene *s, const char *relUrl );

  virtual ostream& printFields(ostream& os, int indent);

  virtual void render(Viewer *, VrmlRenderContext rc);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  const VrmlSFNode & getAppearance() const;
  void setAppearance(const VrmlSFNode & appearance);
  
  const VrmlSFNode & getGeometry() const;
  void setGeometry(const VrmlSFNode & geometry);

private:

  VrmlSFNode d_appearance;
  VrmlSFNode d_geometry;

  Viewer::Object d_viewerObject; // move to VrmlNode.h ? ...
  
};

#endif _VRMLNODESHAPE_
