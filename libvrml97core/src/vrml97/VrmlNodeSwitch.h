//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//  See the file COPYING for license details.
//
//  VrmlNodeSwitch.h

#ifndef  _VRMLNODESWITCH_
#define  _VRMLNODESWITCH_

#include "VrmlMFNode.h"
#include "VrmlSFInt.h"
#include "VrmlNodeChild.h"
#include "VrmlBSphere.h"

class VrmlNodeSwitch : public VrmlNodeChild {

public:

  // Define the fields of all built in switch nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeSwitch(VrmlScene *);
  virtual ~VrmlNodeSwitch();

  virtual VrmlNode *cloneMe() const;
  void cloneChildren(VrmlNamespace *);

  virtual VrmlNodeSwitch* toSwitch() const; //LarryD

  virtual bool isModified() const;
  virtual void updateModified(VrmlNodePath& path);
  virtual void clearFlags();

  virtual void addToScene( VrmlScene *s, const char *relUrl );

  virtual void copyRoutes( VrmlNamespace *ns ) const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual void render(Viewer *, VrmlRenderContext rc);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  VrmlMFNode *getChoiceNodes()  { return &d_choice;} 
  virtual int getWhichChoice() { return d_whichChoice.get(); }  

  const VrmlBVolume* getBVolume() const;

protected:

  VrmlMFNode d_choice;
  VrmlSFInt d_whichChoice;
  
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

