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

#ifndef  _VRMLNODEINDEXEDSET_
#define  _VRMLNODEINDEXEDSET_

#include "VrmlNodeGeometry.h"
#include "VrmlSFBool.h"
#include "VrmlSFFloat.h"
#include "VrmlSFNode.h"
#include "VrmlMFInt.h"

class VrmlNodeIndexedSet : public VrmlNodeGeometry {

public:

  // Define the fields of indexed face set nodes
  static VrmlNodeType *defineType(VrmlNodeType *t);

  VrmlNodeIndexedSet(VrmlScene *);
  virtual ~VrmlNodeIndexedSet();

  virtual bool isModified() const;
  virtual void updateModified(VrmlNodePath& path);
  virtual void clearFlags();

  virtual void addToScene( VrmlScene *s, const char *relUrl );

  virtual void copyRoutes(VrmlNamespace *ns) const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  virtual VrmlNodeColor *color();

  virtual VrmlNode* getCoordinate();
  virtual const VrmlMFInt& getCoordIndex() const;
  virtual bool getColorPerVertex(){ return d_colorPerVertex.get(); } // LarryD  Feb18/99
  virtual const VrmlMFInt& getColorIndex() const; // LarryD  Feb18/99

protected:

  VrmlSFNode d_color;
  VrmlMFInt d_colorIndex;
  VrmlSFBool d_colorPerVertex;

  VrmlSFNode d_coord;
  VrmlMFInt d_coordIndex;

};

#endif _VRMLNODEINDEXEDSET_
