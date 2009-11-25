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

# include "geo_metadata.h"
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
     * @brief Represents GeoMetadata node instances.
     */
    class OPENVRML_LOCAL geo_metadata_node :
        public abstract_node<geo_metadata_node>,
        public child_node {

        friend class openvrml_node_x3d_geospatial::geo_metadata_metatype;

        exposedfield<mfnode> data_;
        exposedfield<mfstring> summary_;
        exposedfield<mfstring> url_;

    public:
        geo_metadata_node(const node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~geo_metadata_node() OPENVRML_NOTHROW;
    };


    /**
     * @var geo_metadata_node::geo_metadata_metatype
     *
     * @brief Class object for GeoMetadata nodes.
     */

    /**
     * @var geo_metadata_node::data_
     *
     * @brief data exposedField
     */

    /**
     * @var geo_metadata_node::summary_
     *
     * @brief summary exposedField
     */

    /**
     * @var geo_metadata_node::url_
     *
     * @brief url exposedField
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    geo_metadata_node::
    geo_metadata_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        data_(*this),
        summary_(*this),
        url_(*this)
    {}

    /**
     * @brief Destroy.
     */
    geo_metadata_node::~geo_metadata_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_geospatial::geo_metadata_metatype::id =
    "urn:X-openvrml:node:GeoMetadata";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c geo_metadata_metatype.
 */
openvrml_node_x3d_geospatial::geo_metadata_metatype::
geo_metadata_metatype(openvrml::browser & browser):
    node_metatype(geo_metadata_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_geospatial::geo_metadata_metatype::~geo_metadata_metatype()
    OPENVRML_NOTHROW
{}

# define GEO_METADATA_INTERFACE_SEQ                  \
    ((exposedfield, sfnode,   "metadata", metadata)) \
    ((exposedfield, mfnode,   "data",     data_))    \
    ((exposedfield, mfstring, "summary",  summary_)) \
    ((exposedfield, mfstring, "url",      url_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_geospatial,
                                              geo_metadata_metatype,
                                              geo_metadata_node,
                                              GEO_METADATA_INTERFACE_SEQ)
