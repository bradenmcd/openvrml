//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//  See the file COPYING for license details.
//
//  VrmlNodeViewpoint.h

#ifndef  _VRMLNODEVIEWPOINT_
#define  _VRMLNODEVIEWPOINT_

#include "VrmlNodeChild.h"
#include "VrmlField.h"
#include "VrmlSFBool.h"
#include "VrmlSFFloat.h"
#include "VrmlSFRotation.h"
#include "VrmlSFString.h"
#include "VrmlSFTime.h"
#include "VrmlSFVec3f.h"

class VrmlScene;
class VrmlFrustum;
class VrmlBVolume;

class VrmlNodeViewpoint : public VrmlNodeChild {

public:

  // Define the fields of Viewpoint nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeViewpoint(VrmlScene *);
  virtual ~VrmlNodeViewpoint();

  virtual VrmlNode *cloneMe() const;

  virtual VrmlNodeViewpoint* toViewpoint() const;

  virtual void addToScene( VrmlScene *s, const char *relUrl );

  virtual ostream& printFields(ostream& os, int indent);

  virtual void eventIn(double timeStamp,
		       const char *eventName,
		       const VrmlField *fieldValue);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  virtual void accumulateTransform( VrmlNode* );
  virtual VrmlNode* getParentTransform();


  float fieldOfView() const	{ return d_fieldOfView.get(); }
  float orientationX() const	{ return d_orientation.x(); }
  float orientationY() const	{ return d_orientation.y(); }
  float orientationZ() const	{ return d_orientation.z(); }
  float orientationR() const	{ return d_orientation.r(); }
  float positionX() const       { return d_position.x(); }
  float positionY() const	{ return d_position.y(); }
  float positionZ() const	{ return d_position.z(); }

  const char *description() const { return d_description.get() ? d_description.get() : ""; }

  const VrmlBVolume* getBVolume() const;

  /**
   * Determine the inverse of the transform represented by the
   * viewpoint's position and orientation fields. Return the matrix in
   * MathUtils format. Note that this method deals only with the
   * viewpoint node's transform, not with any ancestor transforms.
   *
   * @param IM inverse of the position/orientation transform
   *
   * @see VrmlNode::inverseTransform
   */
  void getInverseMatrix(double IM[4][4]) const;

  // not implemented
  void getFrustum(VrmlFrustum& frust) const; // get a copy

  const char *description() { return d_description.get() ? d_description.get() : ""; }

private:

  VrmlSFFloat d_fieldOfView;
  VrmlSFBool d_jump;
  VrmlSFRotation d_orientation;
  VrmlSFVec3f d_position;
  VrmlSFString d_description;


  VrmlSFBool d_isBound;
  VrmlSFTime d_bindTime;

  VrmlNode *d_parentTransform;

};

#endif // _VRMLNODEVIEWPOINT_

