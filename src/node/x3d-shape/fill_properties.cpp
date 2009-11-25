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

# include "fill_properties.h"
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
     * @brief Represents FillProperties node instances.
     */
    class OPENVRML_LOCAL fill_properties_node :
        public abstract_node<fill_properties_node> {

        friend class openvrml_node_x3d_shape::fill_properties_metatype;

        exposedfield<sfbool> filled_;
        exposedfield<sfcolor> hatch_color_;
        exposedfield<sfbool> hatched_;
        exposedfield<sfint32> hatch_style_;

    public:
        fill_properties_node(const node_type & type, 
                             const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~fill_properties_node() OPENVRML_NOTHROW;
    };


    /**
     * @var fill_properties_node::fill_properties_metatype
     *
     * @brief Class object for FillProperties nodes.
     */

    /**
     * @var fill_properties_node::filled_
     *
     * @brief filled exposedField
     */

    /**
     * @var fill_properties_node::hatch_color_
     *
     * @brief hatch_color exposedField
     */

    /**
     * @var fill_properties_node::hatched_
     *
     * @brief hatched exposedField
     */

    /**
     * @var fill_properties_node::hatch_style_
     *
     * @brief hatch_style exposedField
     */

    
    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    fill_properties_node::
    fill_properties_node(const node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        filled_(*this, true),
        hatch_color_(*this, make_color(1.0, 1.0, 1.0)),
        hatched_(*this, true),
        hatch_style_(*this, 1)
    {}

    /**
     * @brief Destroy.
     */
    fill_properties_node::~fill_properties_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_shape::fill_properties_metatype::id =
    "urn:X-openvrml:node:FillProperties";

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this fill_properties_metatype.
 */
openvrml_node_x3d_shape::fill_properties_metatype::
fill_properties_metatype(openvrml::browser & browser):
    node_metatype(fill_properties_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_shape::fill_properties_metatype::~fill_properties_metatype()
    OPENVRML_NOTHROW
{}

# define FILL_PROPERTIES_INTERFACE_SEQ                    \
    ((exposedfield, sfnode,  "metadata",   metadata))     \
    ((exposedfield, sfbool,  "filled",     filled_))      \
    ((exposedfield, sfcolor, "hatchColor", hatch_color_)) \
    ((exposedfield, sfbool,  "hatched",    hatched_))     \
    ((exposedfield, sfint32, "hatchStyle", hatch_style_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_shape,
                                              fill_properties_metatype,
                                              fill_properties_node,
                                              FILL_PROPERTIES_INTERFACE_SEQ)
