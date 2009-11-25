// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
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

# include "geo_origin.h"
# include "geospatial-common.h"
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
     * @brief Represents GeoOrigin node instances.
     */
    class OPENVRML_LOCAL geo_origin_node :
        public abstract_node<geo_origin_node> {

        friend class openvrml_node_x3d_geospatial::geo_origin_metatype;

        exposedfield<sfvec3d> geo_coords_;
        exposedfield<mfstring> geo_system_;
        sfbool rotate_y_up_;

    public:
        geo_origin_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~geo_origin_node() OPENVRML_NOTHROW;
    };


    /**
     * @var geo_origin_node::geo_origin_metatype
     *
     * @brief Class object for GeoOrigin nodes.
     */

    /**
     * @var geo_origin_node::geo_coords_
     *
     * @brief geo_coords exposedField
     */

    /**
     * @var geo_origin_node::geo_system_
     *
     * @brief geo_system exposedField
     */

    /**
     * @var geo_origin_node::rotate_yup_
     *
     * @brief rotate_yup field
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    geo_origin_node::
    geo_origin_node(const node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        geo_coords_(*this),
        geo_system_(*this, openvrml_node_x3d_geospatial::default_geo_system)
    {}

    /**
     * @brief Destroy.
     */
    geo_origin_node::~geo_origin_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_geospatial::geo_origin_metatype::id =
    "urn:X-openvrml:node:GeoOrigin";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c geo_origin_metatype.
 */
openvrml_node_x3d_geospatial::geo_origin_metatype::
geo_origin_metatype(openvrml::browser & browser):
    node_metatype(geo_origin_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_geospatial::geo_origin_metatype::~geo_origin_metatype()
    OPENVRML_NOTHROW
{}

# define GEO_ORIGIN_INTERFACE_SEQ                        \
    ((exposedfield, sfnode,   "metadata",  metadata))    \
    ((exposedfield, sfvec3d,  "geoCoords", geo_coords_)) \
    ((exposedfield, mfstring, "geoSystem", geo_system_)) \
    ((field,        sfbool,   "rotateYUp", rotate_y_up_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_geospatial,
                                              geo_origin_metatype,
                                              geo_origin_node,
                                              GEO_ORIGIN_INTERFACE_SEQ)
