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

#ifndef  _VRMLNODESWITCH_
#define  _VRMLNODESWITCH_

#include "VrmlMFNode.h"
#include "VrmlSFInt32.h"
#include "VrmlNodeChild.h"
#include "VrmlBSphere.h"

class VrmlNodeSwitch : public VrmlNodeChild {

public:

  // Define the fields of all built in switch nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeSwitch(VrmlScene *);
  virtual ~VrmlNodeSwitch();

  virtual bool accept(VrmlNodeVisitor & visitor);
  virtual void resetVisitedFlag();
  
  virtual VrmlNodeSwitch* toSwitch() const; //LarryD

  virtual bool isModified() const;
  virtual void updateModified(VrmlNodePath& path);
  virtual void clearFlags();

  virtual void addToScene( VrmlScene *s, const char *relUrl );

  virtual ostream& printFields(ostream& os, int indent);

  virtual void render(Viewer *, VrmlRenderContext rc);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  const VrmlMFNode & getChoice() const;
  void setChoice(const VrmlMFNode & choice);
  
  const VrmlSFInt32 & getWhichChoice() const;
  void setWhichChoice(const VrmlSFInt32 & choice);

  const VrmlBVolume* getBVolume() const;

protected:

  VrmlMFNode d_choice;
  VrmlSFInt32 d_whichChoice;
  
  /**
   * Cached copy of the bsphere enclosing this node's children.
   */
  VrmlBSphere d_bsphere;

  /**
   * Construct a bounding sphere around this node's children. Store it
   * in d_bsphere.
   */
  virtual void recalcBSphere();
};

#endif _VRMLNODESWITCH_

