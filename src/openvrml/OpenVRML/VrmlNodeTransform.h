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

#ifndef  _VRMLNODETRANSFORM_
#define  _VRMLNODETRANSFORM_

#include "VrmlNodeGroup.h"
#include "VrmlSFRotation.h"
#include "VrmlSFVec3f.h"
#include "VrmlBSphere.h"

/**
 * VrmlNodeTransform.
 */
class VrmlNodeTransform : public VrmlNodeGroup {

 public:

  // Define the fields of Transform nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeTransform(VrmlScene *);
  virtual ~VrmlNodeTransform();

  virtual VrmlNode *cloneMe() const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual void render(Viewer *, VrmlRenderContext rc);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  virtual void accumulateTransform(VrmlNode*);
  virtual void inverseTransform(Viewer *);
  virtual void inverseTransform(double [4][4]);

  virtual VrmlNodeTransform* toTransform() const;     //LarryD Feb 24/99
  virtual const VrmlSFVec3f& getCenter() const;  //LarryD Feb 24/99
  virtual const VrmlSFRotation& getRotation() const;  //LarryD Feb 24/99
  virtual const VrmlSFVec3f& getScale() const;  //LarryD Feb 24/99
  virtual const VrmlSFRotation& getScaleOrientation() const;  //LarryD Feb 24/99
  virtual const VrmlSFVec3f& getTranslation() const;  //LarryD Feb 

  const VrmlBVolume* getBVolume() const;

  /**
   * Get a matrix representation (in MathUtils format) of the
   * transformation stored in the node fields.
   *
   * @return a copy of the cached transformation matrix
   */
  void getMatrix(double M[4][4]) const;


  /**
   * Take the fields of this transform, and calculate the matching
   * transformation matrix. Store a copy in M. Should this be
   * protected?
   *
   * @param t a transformation node
   * @param flag 0 means calculate transform, 1 means calculate
   *               inverse transform
   * @param M gets a copy of the resulting transform
   */
  static void transform_to_matrix(const VrmlNodeTransform* t_arg, int flag, double M[4][4]);


 protected:

  VrmlSFVec3f d_center;
  VrmlSFRotation d_rotation;
  VrmlSFVec3f d_scale;
  VrmlSFRotation d_scaleOrientation;
  VrmlSFVec3f d_translation;
  Viewer::Object d_xformObject;

  void recalcBSphere();

  /**
   * Resynchronize the cached matrix <code>M</code> with the node
   * fields, but only if M_dirty is true. Think logical const.
   */
  void synch_cached_matrix();

  /**
   * Cached copy (in MathUtils format) of this node's transformation.
   * Currently this is used only by the culling code, but eventually
   * all the matrix manipulation needs to be moved from the Viewer
   * side over into core.
   */
  double M[4][4];

  /**
   * If true, we need to recalculate M. Is this the same as
   * VrmlNode::d_modified? No, since it's entirely a core-side issue,
   * and has nothing to do with the viewer being out of date wrt the
   * core scene graph.
   */
  bool M_dirty;
  
};

#endif _VRMLNODETRANSFORM_
