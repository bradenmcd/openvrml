// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2006, 2007, 2008, 2009  Braden McDaniel
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

# include "h_anim_displacer.h"
# include <openvrml/node_impl_util.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

using namespace openvrml;
using namespace openvrml::node_impl_util;
using namespace std;

namespace {

    /**
     * @brief Represents HAnimDisplacer node instances.
     */
    class OPENVRML_LOCAL h_anim_displacer_node :
        public abstract_node<h_anim_displacer_node> {

        friend class openvrml_node_x3d_h_anim::h_anim_displacer_metatype;

        exposedfield<mfint32> coord_index_;
        exposedfield<mfvec3f> displacements_;
        exposedfield<sfstring> name_;
        exposedfield<sffloat> weight_;

    public:
        h_anim_displacer_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~h_anim_displacer_node() OPENVRML_NOTHROW;
    };


    /**
     * @var h_anim_displacer_node::h_anim_displacer_metatype
     *
     * @brief Class object for HAnimDisplacer nodes.
     */

    /**
     * @var h_anim_displacer_node::coord_index_
     *
     * @brief coord_index exposedField
     */

    /**
     * @var h_anim_displacer_node::displacements_
     *
     * @brief displacements exposedField
     */

    /**
     * @var h_anim_displacer_node::name_
     *
     * @brief name exposedField
     */

    /**
     * @var h_anim_displacer_node::weight_
     *
     * @brief weight exposedField
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    h_anim_displacer_node::
    h_anim_displacer_node(const node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        coord_index_(*this),
        displacements_(*this),
        name_(*this),
        weight_(*this)
    {}

    /**
     * @brief Destroy.
     */
    h_anim_displacer_node::~h_anim_displacer_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_h_anim::h_anim_displacer_metatype::id =
    "urn:X-openvrml:node:HAnimDisplacer";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c h_anim_displacer_metatype.
 */
openvrml_node_x3d_h_anim::h_anim_displacer_metatype::
h_anim_displacer_metatype(openvrml::browser & browser):
    node_metatype(h_anim_displacer_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_h_anim::h_anim_displacer_metatype::
~h_anim_displacer_metatype()
    OPENVRML_NOTHROW
{}

# define H_ANIM_DISPLACER_INTERFACE_SEQ                         \
    ((exposedfield, sfnode,   "metadata",      metadata))       \
    ((exposedfield, mfint32,  "coordIndex",    coord_index_))   \
    ((exposedfield, mfvec3f,  "displacements", displacements_)) \
    ((exposedfield, sfstring, "name",          name_))          \
    ((exposedfield, sffloat,  "weight",        weight_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_h_anim,
                                              h_anim_displacer_metatype,
                                              h_anim_displacer_node,
                                              H_ANIM_DISPLACER_INTERFACE_SEQ)
