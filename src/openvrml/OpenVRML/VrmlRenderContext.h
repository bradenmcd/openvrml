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

#ifndef VRMLRENDERCONTEXT_H
#define VRMLRENDERCONTEXT_H

/**
 * Information needed during a render traversal. The members could be
 * arguments to the <code>VrmlNode::render()</code> method, but there
 * may be many arguments, and adding an argument requires changing
 * nearly every file in the core
 * <p>
 * WARNING: This is a concrete base class that is passed by value, not
 * by reference. The idea is to pass everything exactly as if it were
 * a normal argument to the render function. This has some
 * consequences...
 * <p>
 * First off, the members should be primitives or pointers. There
 * shouldn't be anything that requires a lot of work to copy. Calling
 * render() should be cheap. Secondly, while adding a new member to
 * this class is less work than adding a new argument to every
 * render() call, it still requires a recompile of nearly the whole
 * core. That's because pass-by-value needs to know the exact size of
 * what's being passed. Thirdly, you can't subclass this class. It's
 * concrete. If you pass a subclass it will just get truncated. There
 * are some C++ tricks to prohibit inheriting from a class, but let's
 * assume we're all responsbile adults here.
 * <p>
 * Why do it this way? Because it makes writing the render() method
 * easier. We don't have to maintain a stack of states, we just use
 * the call stack instead. That means no heap allocation and
 * deallocation, which is very cool. Also, we don't have to pop the
 * stack at the end of the method. Alternatively, I could have used
 * the "initialization is resource acquisition" pattern, but I felt
 * this was cleaner.
 *
 * @author Christopher K. St. John 
 *
 * @see VrmlNode::render
 * @see VrmlScene::render
 */
class VrmlRenderContext {

 public:

  /**
   * encapsulte some ugliness. see comments on getMatrix()
   * below. (does this really need to be in a public area? be safe and
   * guess yes)
   */
  typedef double (*MatrixPtr)[4];


 protected:

  /**
   * Track the results of intersecting node bounds with the view
   * volume. This could be of some named enum type, but that would
   * mean coupling to VrmlBVolume.h, and we just don't need that.
   *
   * @see VrmlBVolume
   */
  int cull_flag;
  
  /**
   * The current modelview matrix in MathUtils-format.
   *
   * @see MathUtils
   */
  MatrixPtr M;

  /**
   * Draw the bounding volumes or not.
   */
  bool draw_bspheres;


 public:

  /**
   * Constructs an empty render context. An empty context should not
   * be passed to <code>VrmlNode::render()</code>. This constructor is
   * useful only for debugging and experimentation.
   */
  VrmlRenderContext();

  /**
   * Constructs and initializes a render context.
   * 
   * @param acull_flag The cull flag argument will normally be
   *                   <code>VrmlBVolume::BV_PARTIAL</code>
   *
   * @param aM A modelview matrix in MathUtils format. The transform
   *                   can be affine, but the rendering code may take
   *                   advantage of an orthogonal transform if one is
   *                   passed in. This argument should not be null.
   *
   * @see setCullFlag
   */
  VrmlRenderContext(int acull_flag, double aM[4][4]);

  /**
   * Constructs and initializes a render context. The autogenerated
   * copy constructor would do fine in this case, but putting it here
   * explicitly is a reminder that the case has to be considered.
   *
   * @param rhs the render context to copy
   */
  VrmlRenderContext(const VrmlRenderContext& rhs);

  /**
   * Destroy a render context. As with the copy constructor, the
   * autogenerated one would do fine. But notice: the constructor is
   * not and should not be virtual since this is a concrete class.
   */
  ~VrmlRenderContext();


  /**
   * Returns the cull flag.
   * @return the cull flag
   */
  int getCullFlag() const;

  /**
   * Sets the cull flag. Setting to BV_INSIDE means that all the last
   * tested bounding volume was completely inside the view volume, so
   * all the contained bounding volumes must also be inside and we can
   * skip further testing. BV_PARTIAL means that the last test
   * indicated that the bounding volume intersected the view volume,
   * so some of the children may be visible and we must continue
   * testing. BV_OUTSIDE means the last test indicated the bounding
   * volume was completely outside the view volume. However, there's
   * normally no reason to call set with BV_OUTSIDE, since the render
   * method returns immediatly. But who knows, it might be useful some
   * day, so it's an allowed value.
   * <p> 
   * Setting the cull flag to BV_INSIDE in VrmlScene at the top of the
   * traversal has the effect of disabling the culling tests. The
   * behavior is undefined if the flag is not one of the allowed
   * values.
   *
   * @param f Should be one of <code>VrmlBVolume::BV_INSIDE</code>,
   *          <code>VrmlBVolume::VB_OUTSIDE</code>,
   *          <code>VrmlBVolume::BV_PARTIAL</code>
   *
   * @see VrmlScene
   * @see VrmlBVolume
   */
  void setCullFlag(int f);


  /**
   * Returns a pointer to the modelview matrix. Unlike most of the
   * matrix accessor routines, it does not return a copy. Because C++
   * inherits C's array-uglies, we need the MatrixPtr typedef.
   *
   * @return a pointer to the modelview matrix in MathUtils format.
   */
  MatrixPtr getMatrix();

  /**
   * Sets the modelview matrix. Don't set it to null. Make sure that
   * it's really in MathUtils format (which may be different than
   * OpenGL format) VrmlRenderContext retains a pointer to the passed
   * in matrix, it does not make a copy. All memory management is up
   * to the caller. In practice, the passed-in array will generally be
   * a local in the render() method.
   *
   * @param aM a modelview matrix in MathUtils format. Must be at
   *           least affine, although the render code may optimize for
   *           orthogonal transforms.
   */
  void setMatrix(double aM[4][4]);


  /**
   * Sets the bounding volume debug flag. If true, then the renderer
   * may draw the bounding volumes for each primitive. Or maybe not,
   * so you shouldn't depend on this behavior. It does look kinda cool
   * though.
   *
   * @param f bounding volume debug flag.
   *
   * @see Viewer::drawBVolume
   */
  void setDrawBSpheres(bool f);

  /**
   * Returns the bounding volume debug flag.
   * @returns the bounding volume debug flag. 
   */
  bool getDrawBSpheres();

};



#endif // VRMLRENDERCONTEXT_H
