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

#ifndef  _VRMLNODEGROUP_
#define  _VRMLNODEGROUP_

#include "VrmlMFNode.h"
#include "VrmlSFString.h"
#include "VrmlSFVec3f.h"
#include "VrmlBSphere.h"

#include "VrmlNodeChild.h"
#include "Viewer.h"

/**
 * VrmlNodeGroup
 */
class VrmlNodeGroup : public VrmlNodeChild {

public:

  // Define the fields of all built in group nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeGroup(VrmlScene *s = 0);
  virtual ~VrmlNodeGroup();

  virtual bool accept(VrmlNodeVisitor & visitor);
  virtual void resetVisitedFlag();
  
  virtual VrmlNodeGroup* toGroup() const;

  virtual bool isModified() const;
  virtual void updateModified(VrmlNodePath& path, int flags);
  virtual void clearFlags();

  virtual void addToScene( VrmlScene *s, const char *relativeUrl );

  virtual ostream& printFields(ostream& os, int indent);

  virtual void render(Viewer *, VrmlRenderContext rc);
  virtual void renderNoCull(Viewer *, VrmlRenderContext rc);

  virtual void accumulateTransform(VrmlNode*);

  void activate( double timeStamp, bool isOver, bool isActive, double *p );

  const VrmlMFNode & getChildren() const;
  void setChildren(const VrmlMFNode & children);
  void addChildren( const VrmlMFNode &children );
  void removeChildren( const VrmlMFNode &children );
  void removeChildren();

  virtual void eventIn(double timeStamp,
		       const char *eventName,
		       const VrmlField *fieldValue);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  int size();
  VrmlNode *child(int index);

  virtual VrmlNode* getParentTransform();

  // LarryD Feb 11/99
  VrmlMFNode *getNodes()  { return &d_children;}

  const VrmlBVolume* getBVolume() const;

protected:

  VrmlSFVec3f d_bboxCenter;
  VrmlSFVec3f d_bboxSize;
  VrmlMFNode d_children;

  VrmlSFString d_relative;
  VrmlNode *d_parentTransform;
  Viewer::Object d_viewerObject;

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

#endif _VRMLNODEGROUP_

