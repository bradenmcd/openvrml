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

#ifndef  _VRMLNODEEXTRUSION_
#define  _VRMLNODEEXTRUSION_

#include "VrmlNodeGeometry.h"
#include "field.h"

class VrmlNodeExtrusion : public VrmlNodeGeometry {

public:

  // Define the fields of extrusion nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeExtrusion(VrmlScene *);
  virtual ~VrmlNodeExtrusion();

  virtual bool accept(VrmlNodeVisitor & visitor);
  
  virtual ostream& printFields(ostream& os, int indent);

  virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  // Larry
  virtual VrmlNodeExtrusion* toExtrusion() const;
  virtual bool getBeginCap(){ return d_beginCap.get(); }
  virtual bool getCcw(){ return d_ccw.get(); }
  virtual bool getConvex(){ return d_convex.get(); }  
  virtual float getCreaseAngle(){return d_creaseAngle.get();}
  virtual VrmlMFVec2f &getCrossSection() { return d_crossSection; }
  virtual bool getEndCap(){ return d_endCap.get(); }
  virtual VrmlMFRotation &getOrientation() { return d_orientation; } 
  virtual VrmlMFVec2f &getScale() { return d_scale; } 
  virtual bool getSolid(){ return d_solid.get();} 
  virtual VrmlMFVec3f &getSpine() { return d_spine; } 

protected:

  VrmlSFBool d_beginCap;
  VrmlSFBool d_ccw;
  VrmlSFBool d_convex;
  VrmlSFFloat d_creaseAngle;
  VrmlMFVec2f d_crossSection;
  VrmlSFBool d_endCap;
  VrmlMFRotation d_orientation;
  VrmlMFVec2f d_scale;
  VrmlSFBool d_solid;
  VrmlMFVec3f d_spine;

};

#endif _VRMLNODEEXTRUSION_

