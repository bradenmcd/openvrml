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

# include "line_properties.h"
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
     * @brief Represents LineProperties node instances.
     */
    class OPENVRML_LOCAL line_properties_node :
        public abstract_node<line_properties_node> {

        friend class openvrml_node_x3d_shape::line_properties_metatype;

        exposedfield<sfbool> applied_;
        exposedfield<sfint32> line_type_;
        exposedfield<sffloat> linewidth_scale_factor_;

    public:
        line_properties_node(const node_type & type, 
                             const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~line_properties_node() OPENVRML_NOTHROW;
    };


    /**
     * @var line_properties_node::line_properties_metatype
     *
     * @brief Class object for LineProperties nodes.
     */

    /**
     * @var line_properties_node::applied_
     *
     * @brief applied exposedField
     */

    /**
     * @var line_properties_node::line_type_
     *
     * @brief line_type exposedField
     */

    /**
     * @var line_properties_node::linewidth_scale_factor_
     *
     * @brief linewidth_scale_factor exposedField
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    line_properties_node::
    line_properties_node(const node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        applied_(*this, true),
        line_type_(*this, 1),
        linewidth_scale_factor_(*this)
    {}

    /**
     * @brief Destroy.
     */
    line_properties_node::~line_properties_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_shape::line_properties_metatype::id =
    "urn:X-openvrml:node:LineProperties";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c line_properties_metatype.
 */
openvrml_node_x3d_shape::line_properties_metatype::
line_properties_metatype(openvrml::browser & browser):
    node_metatype(line_properties_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_shape::line_properties_metatype::~line_properties_metatype()
    OPENVRML_NOTHROW
{}

# define LINE_PROPERTIES_INTERFACE_SEQ                                  \
    ((exposedfield, sfnode, "metadata", metadata))                      \
    ((exposedfield, sfbool, "applied",  applied_))                      \
    ((exposedfield, sfint32, "lineType", line_type_))                   \
    ((exposedfield, sffloat, "linewidthScaleFactor", linewidth_scale_factor_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_shape,
                                              line_properties_metatype,
                                              line_properties_node,
                                              LINE_PROPERTIES_INTERFACE_SEQ)
