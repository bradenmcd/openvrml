// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
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

# ifdef HAVE_CONFIG_H
#   include "config.h"
# endif

# include "rendering_context.h"

/**
 * @class openvrml::rendering_context
 *
 * @brief Information needed during a render traversal.
 *
 * The members could be arguments to the Node::render method, but there may be
 * many arguments, and adding an argument requires changing nearly every file
 * in the core.
 *
 * WARNING: This is a concrete base class that is passed by value, not
 * by reference. The idea is to pass everything exactly as if it were
 * a normal argument to the render function. This has some
 * consequences...
 *
 * First off, the members should be primitives or pointers. There
 * shouldn't be anything that requires a lot of work to copy. Calling
 * render() should be cheap. Secondly, while adding a new member to
 * this class is less work than adding a new argument to every
 * node::render call, it still requires a recompile of nearly the whole
 * core. That's because pass-by-value needs to know the exact size of
 * what's being passed. Thirdly, you can't subclass this class. It's
 * concrete. If you pass a subclass it will just get truncated.
 *
 * Why do it this way? Because it makes writing the node::render method
 * easier. We don't have to maintain a stack of states, we just use
 * the call stack instead. That means no heap allocation and
 * deallocation, which is very cool. Also, we don't have to pop the
 * stack at the end of the method. Alternatively, I could have used
 * the "initialization is resource acquisition" pattern, but I felt
 * this was cleaner.
 *
 * @author Christopher K. St. John
 *
 * @see node::render
 * @see browser::render
 */

/**
 * @var openvrml::bounding_volume::Intersection openvrml::rendering_context::cull_flag
 *
 * @brief Track the results of intersecting node bounds with the view volume.
 *
 * Setting to bounding_volume::inside means that all the last tested bounding
 * volume was completely inside the view volume, so all the contained bounding
 * volumes must also be inside and we can skip further testing.
 * bounding_volume::partial means that the last test indicated that the
 * bounding volume intersected the view volume, so some of the children may be
 * visible and we must continue testing.  bounding_volume::outside means the
 * last test indicated the bounding volume was completely outside the view
 * volume.  However, there's normally no reason to call set with
 * bounding_volume::outside, since the render method returns immediately.  But
 * who knows; it might be useful some day, so it's an allowed value.
 *
 * Setting the cull flag to bounding_volume::inside in the Browser at the top
 * of the traversal has the effect of disabling the culling tests. The behavior
 * is undefined if the flag is not one of the allowed values.
 *
 * @see openvrml::browser
 * @see openvrml::bounding_volume
 */

/**
 * @var openvrml::mat4f * openvrml::rendering_context::modelview;
 *
 * The current modelview matrix.
 */

/**
 * @var bool openvrml::rendering_context::draw_bounding_spheres;
 *
 * @brief Draw the bounding volumes or not.
 *
 * If true, then the renderer may draw the bounding volumes for each primitive.
 * Or maybe not, so you shouldn't depend on this behavior. It does look kinda
 * cool though.
 */

/**
 * @brief Constructs an empty render context.
 *
 * An empty context should not be passed to VrmlNode::render.
 * This constructor is useful only for debugging and experimentation.
 */
openvrml::rendering_context::rendering_context():
    modelview(0),
    cull_flag(bounding_volume::partial),
    draw_bounding_spheres(false)
{}

/**
 * @brief Constructs and initializes a render context.
 *
 * @param cull_flag the cull flag argument will normally be
 *                  bounding_volume::partial.
 * @param modelview The modelview matrix. The transform can be affine, but the
 *                  rendering code may take advantage of an orthogonal
 *                  transform if one is passed in.
 *
 * @see cull_flag
 */
openvrml::rendering_context::rendering_context(
    const bounding_volume::intersection cull_flag,
    mat4f & modelview):
    modelview(&modelview),
    cull_flag(cull_flag),
    draw_bounding_spheres(false)
{}

/**
 * @brief Sets the modelview matrix.
 *
 * A rendering_context retains a pointer to the passed matrix; it does not make
 * a copy.  All memory management is up to the caller. In practice, the
 * passed-in array will generally be a local variable in the node::render
 * method.
 *
 * @param modelview the modelview matrix. Must be at least affine, although the
 *                  render code may optimize for orthogonal transforms.
 */
void openvrml::rendering_context::matrix(mat4f & modelview)
{
    this->modelview = &modelview;
}

/**
 * @brief Returns a reference to the modelview matrix.
 *
 * @return the modelview matrix.
 */
const openvrml::mat4f & openvrml::rendering_context::matrix() const
{
    assert(this->modelview);
    return *this->modelview;
}
