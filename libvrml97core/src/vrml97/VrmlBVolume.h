//
// OpenVRML
//
// Copyright (C) 2000  Christopher K. St. John
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

#ifndef VRMLBVOLUME_H
#define VRMLBVOLUME_H

#ifndef macintosh
class ostream;
#endif
class VrmlFrustum;
class VrmlAABox;
class VrmlBSphere;



/**
 * A bounding volume. All the geometry in a scene maintains a bounding
 * volume to help speed up rendering and picking. Although currently
 * we're just using spheres, the plan is to eventually use tighter
 * bounds like axis aligned boxes around nodes that are expected to be
 * static. That probably means boxes for geometry and spheres for
 * grouping nodes.
 *
 * @see VrmlNode::render
 * @see VrmlBSphere
 * @see VrmlAABox
 *
 */
class VrmlBVolume {

 public:


  /**
   * Sigh. C++ requires a virtual destructor for classes that are
   * designed to be inherited from. Even worse, it turns out that
   * declaring the destructor inline can have undesirable
   * consequences, so we have to provide a non-inline definition. For
   * details on why see Effective C++ items 14 & 33.
   */
  virtual ~VrmlBVolume() = 0;


  /**
   * Results of an intersection. BV_INSIDE means that the tested
   * volume is entirely inside the target volume, BV_OUTSIDE means
   * that the tested volume is entirely outside the target volume, and
   * BV_PARTIAL means that the two volumes intersect.
   */
  enum { BV_INSIDE=1, BV_OUTSIDE=-1, BV_PARTIAL=0 };


  /**
   * Dynamic cast to a sphere.
   * @return Returns null if this object is not a sphere, otherwise
   *         returns this object cast to a VrmlBSphere pointer.
   */
  virtual VrmlBSphere* toBSphere() const = 0;

  /**
   * Dynamic cast to a box
   * @return Returns null if this object is not a box, otherwise
   *         returns this object cast to a VrmlAABox pointer.
   */
  virtual VrmlAABox* toAABox() const = 0;


  /**
   * Gets the MAX flag. The convention is that nodes that should be
   * rendered unconditionally set a MAX bvolume, ensuring that the
   * branch they are on does not get pruned during culling. Stuff like
   * the picking code needs a way to differentiate this from just a
   * really big bounding volume, or an unset bounding volume.
   *
   * @return the max flag
   * @see setMax
   */
  virtual bool isMAX() const = 0;

  /**
   * Sets the MAX flag.
   * @see isMAX
   */
  virtual void setMAX() = 0;


  /**
   * Intersect this bvolume with a frustum. The test assumes that the
   * frustum is in the canonical looking-down-negative-z orientation,
   * so the bounding volume is going to have to be transformed into
   * the frustum's space. (Alternatives include transforming the
   * frustum into the bvolume's space, or transforming both of them
   * into the projection space. Lots of tradeoffs involved, but
   * transforming the bvolume is probably the simplest approach
   * overall)
   *
   * @return one of BV_INSIDE, BV_OUTSIDE, or BV_PARTIAL
   *
   * @see VrmlBVolume::transform
   * @see VrmlBVolume::orthoTransform
   */
  virtual int isectFrustum(const VrmlFrustum& f) const = 0;


  /**
   * Extend this bvolume to enclose the given bvolume. This is tricky,
   * because C++ doesn't provide us a way to figure out exactly what
   * sort of bvolume we have been passed, yet we have to know in order
   * to do the appropriate math. What we really need is double
   * dispatch but C++ does not provide it.
   * <p>
   * What the implementation will probably do is use the toBSphere and
   * toAABox methods to test for the actual type of the parameter, and
   * redispatch to extend(sphere) or extend(box).  Alternatively, we
   * could use the double dispatch pattern as described in the Gang of
   * Four patterns book.
   * <p>
   * We need this because nodes like Group don't know until runtime
   * exactly what sort of bounding volumes their children will have.
   * Group could test using toBSphere and toAABox, but it's better to
   * centralize ugly stuff like that.
   * 
   * @param b a bounding volume of unknown type
   */
  virtual void extend(const VrmlBVolume& b) =0;

  /**
   * Extend this bvolume to enclose the given point
   * @param p a point
   */
  virtual void extend(const float p[3]) = 0;

  /**
   * Extend this bvolume to enclose the given box
   * @param b an axis-aligned box
   */
  virtual void extend(const VrmlAABox& b) = 0;

  /**
   * Extend this bvolume to enclose the given sphere
   * @param b a bounding sphere
   */
  virtual void extend(const VrmlBSphere& b) = 0;

  /**
   * Enclose the given set of points. This resets the volume from
   * any previous values.
   *
   * @param p array of floats, each set of 3 represents a point
   * @param n number of points (not number of floats!)
   */
  virtual void enclose(const float* p, int n) = 0;


  /**
   * Transform this bounding volume using an orthogonal transfom.
   * Orthogonal transformations preserve angles. They include
   * translation, rotation, and uniform scaling. It turns out to be
   * so easy to transform bounding spheres by orthogonal
   * transformations that it's worth special casing. The caller is
   * responsible for assuring that the transformation is in fact
   * orthogonal, otherwise the results are undefined. If in doubt,
   * call <code>transform</code> instead and take the speed hit.
   *
   * @param M orthonormal transformation matrix in MathUtils format
   *
   * @see MathUtils
   */
  virtual void orthoTransform(const double M[4][4]) = 0;

  /**
   * Transform this bounding volume using an affine transfom.
   * Affine transformations can include nonuniform scaling. It is much
   * messier to deal with them, but VRML allows nonuniform scaling, so
   * we have to handle it. Note that since all orthogonal transforms
   * are affine, it's safe to always call this routine instead of
   * <code>orthoTransform</code>, but it's likely to be slower.
   * The results are undefined if this routine is called with a
   * non-affine argument. Note that VRML Transform nodes only allow
   * affine transformations, so unless you're doing something tricky
   * this routine should always be safe.
   *
   * @param M affine transformation matrix in MathUtils format
   *
   * @see MathUtils
   */
  virtual void transform(const double M[4][4]) = 0;


  /** 
   * Print a text representation to a stream. Suitable for debugging.
   * @param ostr an open ostream
   * @returns the ostream, save a line writing the output operator
   */
  virtual ostream& dump(ostream& ostr) const = 0;
};


#endif // VRMLBVOLUME_H
