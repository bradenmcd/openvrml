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

#ifndef  _VRMLNODEELEVATIONGRID_
#define  _VRMLNODEELEVATIONGRID_

#include "VrmlNodeGeometry.h"
#include "VrmlSFBool.h"
#include "VrmlSFFloat.h"
#include "VrmlSFInt32.h"
#include "VrmlSFNode.h"
#include "VrmlMFFloat.h"

class VrmlNodeElevationGrid : public VrmlNodeGeometry {

public:

  // Define the fields of elevationGrid nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeElevationGrid(VrmlScene *);
  virtual ~VrmlNodeElevationGrid();

  virtual VrmlNode *cloneMe() const;
  virtual void cloneChildren(VrmlNamespace *);

  virtual bool isModified() const;
  virtual void updateModified(VrmlNodePath& path);
  virtual void clearFlags();

  virtual void addToScene( VrmlScene *s, const char *relUrl );

  virtual void copyRoutes(VrmlNamespace *ns) const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  virtual VrmlNodeElevationGrid* toElevationGrid() const; //LarryD Mar 09/99

  const VrmlSFNode & getColor() const;
  void setColor(const VrmlSFNode & color);

  const VrmlSFNode & getNormal() const;
  void setNormal(const VrmlSFNode & normal);

  const VrmlSFNode & getTexCoord() const;
  void setTexCoord(const VrmlSFNode & texCoord);

  const VrmlSFBool & getCcw() const;
  void setCcw(const VrmlSFBool & ccw);

  const VrmlSFBool & getColorPerVertex() const;
  void setColorPerVertex(const VrmlSFBool & colorPerVertex);

  const VrmlSFFloat & getCreaseAngle() const;
  void setCreaseAngle(const VrmlSFFloat & creaseAngle);

  const VrmlMFFloat & getHeight() const;
  void setHeight(const VrmlMFFloat & height);

  const VrmlSFBool & getNormalPerVertex() const;
  void setNormalPerVertex(const VrmlSFBool & normalPerVertex);

  const VrmlSFBool & getSolid() const;
  void setSolid(const VrmlSFBool & solid);

  const VrmlSFInt32 & getXDimension() const;
  void setXDimension(const VrmlSFInt32 & xDimension);

  const VrmlSFFloat & getXSpacing() const;
  void setXSpacing(const VrmlSFFloat & xSpacing);

  const VrmlSFInt32 & getZDimension() const;
  void setZDimension(const VrmlSFInt32 & zDimension);

  const VrmlSFFloat & getZSpacing() const;
  void setZSpacing(const VrmlSFFloat & zSpacing);

protected:

  VrmlSFNode d_color;
  VrmlSFNode d_normal;
  VrmlSFNode d_texCoord;

  VrmlSFBool d_ccw;
  VrmlSFBool d_colorPerVertex;
  VrmlSFFloat d_creaseAngle;
  VrmlMFFloat d_height;
  VrmlSFBool d_normalPerVertex;
  VrmlSFBool d_solid;
  VrmlSFInt32 d_xDimension;
  VrmlSFFloat d_xSpacing;
  VrmlSFInt32 d_zDimension;
  VrmlSFFloat d_zSpacing;

};

#endif _VRMLNODEELEVATIONGRID_
