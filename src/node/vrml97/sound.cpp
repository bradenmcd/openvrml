// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2009  Braden McDaniel
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 3 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, see <http://www.gnu.org/licenses/>.
//

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <boost/array.hpp>
# include <openvrml/node_impl_util.h>
# include <private.h>
# include "sound.h"

namespace {

    class OPENVRML_LOCAL sound_node :
        public openvrml::node_impl_util::abstract_node<sound_node>,
        public openvrml::child_node {

        friend class openvrml_node_vrml97::sound_metatype;

        exposedfield<openvrml::sfvec3f> direction_;
        exposedfield<openvrml::sffloat> intensity_;
        exposedfield<openvrml::sfvec3f> location_;
        exposedfield<openvrml::sffloat> max_back_;
        exposedfield<openvrml::sffloat> max_front_;
        exposedfield<openvrml::sffloat> min_back_;
        exposedfield<openvrml::sffloat> min_front_;
        exposedfield<openvrml::sffloat> priority_;
        exposedfield<openvrml::sfnode> source_;
        openvrml::sfbool spatialize_;

    public:
        sound_node(const openvrml::node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~sound_node() OPENVRML_NOTHROW;

    private:
        virtual void do_render_child(openvrml::viewer & viewer,
                                     openvrml::rendering_context context);
    };

    /**
     * @class sound_node
     *
     * @brief Represents Sound node instances.
     */

    /**
     * @var class sound_node::sound_metatype
     *
     * @brief Class object for Sound instances.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<sound_node>::exposedfield<openvrml::sfvec3f> sound_node::direction_
     *
     * @brief direction exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<sound_node>::exposedfield<openvrml::sffloat> sound_node::intensity_
     *
     * @brief intensity exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<sound_node>::exposedfield<openvrml::sfvec3f> sound_node::location_
     *
     * @brief location exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<sound_node>::exposedfield<openvrml::sffloat> sound_node::max_back_
     *
     * @brief maxBack exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<sound_node>::exposedfield<openvrml::sffloat> sound_node::max_front_
     *
     * @brief maxFront exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<sound_node>::exposedfield<openvrml::sffloat> sound_node::min_back_
     *
     * @brief minBack exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<sound_node>::exposedfield<openvrml::sffloat> sound_node::min_front_
     *
     * @brief minFront exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<sound_node>::exposedfield<openvrml::sffloat> sound_node::priority_
     *
     * @brief priority exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<sound_node>::exposedfield<openvrml::sfnode> sound_node::source_
     *
     * @brief source exposedField.
     */

    /**
     * @var openvrml::sfbool sound_node::spatialize_
     *
     * @brief spatialize field.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the instance.
     * @param scope the scope associated with the instance.
     */
    sound_node::
    sound_node(const openvrml::node_type & type,
               const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<sound_node>(type, scope),
        child_node(type, scope),
        direction_(*this, openvrml::make_vec3f(0, 0, 1)),
        intensity_(*this, 1.0f),
        location_(*this),
        max_back_(*this, 10.0f),
        max_front_(*this, 10.0f),
        min_back_(*this, 1.0f),
        min_front_(*this, 1.0f),
        priority_(*this),
        source_(*this),
        spatialize_(true)
    {}

    /**
     * @brief Destroy.
     */
    sound_node::~sound_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Render the node.
     *
     * @param viewer    a Viewer.
     * @param context   a rendering context.
     */
    void
    sound_node::
    do_render_child(openvrml::viewer & /* viewer */,
                    const openvrml::rendering_context /* context */)
    {
        // If this clip has been modified, update the internal data
        if (this->source_.sfnode::value()
            && this->source_.sfnode::value()->modified()) {
//        this->source.value->render(viewer, context);
        }
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::sound_metatype::id =
    "urn:X-openvrml:node:Sound";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype.
 */
openvrml_node_vrml97::sound_metatype::
sound_metatype(openvrml::browser & browser):
    node_metatype(sound_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::sound_metatype::~sound_metatype() OPENVRML_NOTHROW
{}

# define SOUND_INTERFACE_SEQ                             \
    ((exposedfield, sfvec3f, "direction",  direction_))  \
    ((exposedfield, sffloat, "intensity",  intensity_))  \
    ((exposedfield, sfvec3f, "location",   location_))   \
    ((exposedfield, sffloat, "maxBack",    max_back_))   \
    ((exposedfield, sffloat, "maxFront",   max_front_))  \
    ((exposedfield, sffloat, "minBack",    min_back_))   \
    ((exposedfield, sffloat, "minFront",   min_front_))  \
    ((exposedfield, sffloat, "priority",   priority_))   \
    ((exposedfield, sfnode,  "source",     source_))     \
    ((field,        sfbool,  "spatialize", spatialize_)) \
    ((exposedfield, sfnode,  "metadata",   metadata))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              sound_metatype,
                                              sound_node,
                                              SOUND_INTERFACE_SEQ)
