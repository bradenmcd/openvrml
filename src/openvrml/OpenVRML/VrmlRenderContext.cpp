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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "VrmlRenderContext.h"
#include "bvolume.h"

using namespace OpenVRML;

/**
 * @class OpenVRML::VrmlRenderContext
 *
 * @brief Information needed during a render traversal.
 *
 * The members could be arguments to the <code>VrmlNode::render()</code>
 * method, but there may be many arguments, and adding an argument requires
 * changing nearly every file in the core.
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

/**
 * @var VrmlRenderContext::cull_flag
 *
 * Track the results of intersecting node bounds with the view
 * volume. This could be of some named enum type, but that would
 * mean coupling to VrmlBVolume.h, and we just don't need that.
 *
 * @see VrmlBVolume
 */

/**
 * @var VrmlRenderContext::M;
 *
 * The current modelview matrix in VrmlMatrix format (Same as OGL)
 *
 * @see VrmlMatrix
 */

/**
 * @var VrmlRenderContext::draw_bspheres;
 *
 * Draw the bounding volumes or not.
 */

/**
 * @brief Constructs an empty render context.
 *
 * An empty context should not be passed to <code>VrmlNode::render()</code>.
 * This constructor is useful only for debugging and experimentation.
 */
VrmlRenderContext::VrmlRenderContext()
{
  cull_flag = BVolume::BV_PARTIAL;
  M = (VrmlMatrix *)0;
  draw_bspheres = false;
}

/**
 * @brief Constructs and initializes a render context.
 * 
 * @param acull_flag The cull flag argument will normally be
 *                   <code>VrmlBVolume::BV_PARTIAL</code>
 *
 * @param aM A modelview matrix in VrmlMatrix format. The transform
 *                   can be affine, but the rendering code may take
 *                   advantage of an orthogonal transform if one is
 *                   passed in. This argument should not be null.
 *
 * @see setCullFlag
 */
VrmlRenderContext::VrmlRenderContext(int acull_flag, VrmlMatrix & aM)
{
  cull_flag = acull_flag;
  M = &aM;
  draw_bspheres = false;
}

/**
 * @brief Returns the cull flag.
 *
 * @return the cull flag
 */
int
VrmlRenderContext::getCullFlag() const
{
  return cull_flag;
}

/**
 * @brief Sets the cull flag.
 *
 * Setting to BV_INSIDE means that all the last tested bounding volume was
 * completely inside the view volume, so all the contained bounding volumes
 * must also be inside and we can skip further testing. BV_PARTIAL means that
 * the last test indicated that the bounding volume intersected the view volume,
 * so some of the children may be visible and we must continue testing.
 * BV_OUTSIDE means the last test indicated the bounding volume was completely
 * outside the view volume. However, there's normally no reason to call set with
 * BV_OUTSIDE, since the render method returns immediatly. But who knows, it
 * might be useful some day, so it's an allowed value.
 *
 * <p>Setting the cull flag to BV_INSIDE in VrmlScene at the top of the
 * traversal has the effect of disabling the culling tests. The behavior is
 * undefined if the flag is not one of the allowed values.</p>
 *
 * @param f Should be one of <code>VrmlBVolume::BV_INSIDE</code>,
 *          <code>VrmlBVolume::VB_OUTSIDE</code>,
 *          <code>VrmlBVolume::BV_PARTIAL</code>
 *
 * @see VrmlScene
 * @see VrmlBVolume
 */
void
VrmlRenderContext::setCullFlag(int f)
{
  cull_flag = f;
}

/**
 * @brief Sets the modelview matrix.
 *
 * Don't set it to null. Make sure that it's really in VrmlMatrix format (which
 * is same as OpenGL format) VrmlRenderContext retains a pointer to the passed
 * in matrix, it does not make a copy. All memory management is up to the
 * caller. In practice, the passed-in array will generally be a local in the
 * render() method.
 *
 * @param aM a modelview matrix in VrmlMatrix format. Must be at
 *           least affine, although the render code may optimize for
 *           orthogonal transforms.
 */
void
VrmlRenderContext::setMatrix(VrmlMatrix & aM)
{
  M = &aM;
}

/**
 * @brief Returns a reference to the modelview matrix. 
 *
 * @return the modelview matrix.
 */
const VrmlMatrix&
VrmlRenderContext::getMatrix()const 
{
  return *(this->M);
}

/**
 * @brief Returns the bounding volume debug flag.
 *
 * @returns the bounding volume debug flag. 
 */
bool
VrmlRenderContext::getDrawBSpheres()
{
  return draw_bspheres;
}

/**
 * @brief Sets the bounding volume debug flag.
 *
 * If true, then the renderer may draw the bounding volumes for each primitive.
 * Or maybe not, so you shouldn't depend on this behavior. It does look kinda
 * cool though.
 *
 * @param f bounding volume debug flag.
 *
 * @see Viewer::drawBVolume
 */
void
VrmlRenderContext::setDrawBSpheres(bool f)
{
  draw_bspheres = f;
}
