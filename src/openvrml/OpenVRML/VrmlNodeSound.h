//
// OpenVRML
//
// Copyright (C) 1999  Kumaran Santhanam
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

#ifndef  _VRMLNODESOUND_
#define  _VRMLNODESOUND_

#include "VrmlNodeChild.h"
#include "field.h"

class VrmlScene;

class VrmlNodeSound : public VrmlNodeChild {

public:

  // Define the fields of Sound nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  virtual bool accept(VrmlNodeVisitor & visitor);
  virtual void resetVisitedFlag();
  
  VrmlNodeSound( VrmlScene *scene = 0 );
  virtual ~VrmlNodeSound();

  virtual void updateModified(VrmlNodePath& path);
  virtual void clearFlags();

  virtual void addToScene(VrmlScene *s, const char *);

  virtual void render(Viewer *, VrmlRenderContext rc);

  virtual VrmlNodeSound* toSound() const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);
  
  const VrmlSFNode & getSource() const;
  void setSource(const VrmlSFNode & source);

private:

  // Fields
  VrmlSFVec3f d_direction;
  VrmlSFFloat d_intensity;
  VrmlSFVec3f d_location;
  VrmlSFFloat d_maxBack;
  VrmlSFFloat d_maxFront;
  VrmlSFFloat d_minBack;
  VrmlSFFloat d_minFront;
  VrmlSFFloat d_priority;
  VrmlSFNode d_source;
  VrmlSFBool d_spatialize;
};

#endif _VRMLNODESOUND_

