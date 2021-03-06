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

# include "geo_location.h"
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
     * @brief Represents GeoLocation node instances.
     */
    class OPENVRML_LOCAL geo_location_node :
        public abstract_node<geo_location_node>,
        public child_node {

        friend class openvrml_node_x3d_geospatial::geo_location_metatype;

        class add_children_listener : public event_listener_base<self_t>,
                                      public mfnode_listener {
        public:
            explicit add_children_listener(self_t & node);
            virtual ~add_children_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const mfnode & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class remove_children_listener : public event_listener_base<self_t>,
                                         public mfnode_listener {
        public:
            explicit remove_children_listener(self_t & node);
            virtual ~remove_children_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const mfnode & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        add_children_listener add_children_listener_;
        remove_children_listener remove_children_listener_;
        exposedfield<mfnode> children_;
        exposedfield<sfvec3d> geo_coords_;
        sfnode geo_origin_;
        mfstring geo_system_;
        sfvec3f bbox_center_;
        sfvec3f bbox_size_;

    public:
        geo_location_node(const node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~geo_location_node() OPENVRML_NOTHROW;
    };


    /**
     * @var geo_location_node::geo_location_metatype
     *
     * @brief Class object for GeoLocation nodes.
     */

    /**
     * @var geo_location_node::add_children_listener geo_location_node::add_children_listener_
     *
     * @brief add_children eventIn
     */

    /**
     * @var geo_location_node::remove_children_listener geo_location_node::remove_children_listener_
     *
     * @brief remove_children eventIn
     */

    /**
     * @var geo_location_node::children_
     *
     * @brief children exposedField
     */

    /**
     * @var geo_location_node::geo_coords_
     *
     * @brief geo_coords exposedField
     */

    /**
     * @var geo_location_node::geo_origin_
     *
     * @brief geo_origin field
     */

    /**
     * @var geo_location_node::geo_system_
     *
     * @brief geo_system field
     */

    /**
     * @var geo_location_node::bbox_center_
     *
     * @brief bbox_center field
     */

    /**
     * @var geo_location_node::bbox_size_
     *
     * @brief bbox_size field
     */

    geo_location_node::add_children_listener::
    add_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    geo_location_node::add_children_listener::
    ~add_children_listener() OPENVRML_NOTHROW
    {}

    void geo_location_node::add_children_listener::
    do_process_event(const mfnode & /* fraction */, double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    geo_location_node::remove_children_listener::
    remove_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    geo_location_node::remove_children_listener::
    ~remove_children_listener() OPENVRML_NOTHROW
    {}

    void geo_location_node::remove_children_listener::
    do_process_event(const mfnode & /* fraction */, double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    geo_location_node::
    geo_location_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        add_children_listener_(*this),
        remove_children_listener_(*this),
        children_(*this),
        geo_coords_(*this),
        geo_system_(openvrml_node_x3d_geospatial::default_geo_system),
        bbox_size_(make_vec3f(-1.0f, -1.0f, -1.0f))
    {}

    /**
     * @brief Destroy.
     */
    geo_location_node::~geo_location_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_geospatial::geo_location_metatype::id =
    "urn:X-openvrml:node:GeoLocation";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c geo_location_metatype.
 */
openvrml_node_x3d_geospatial::geo_location_metatype::
geo_location_metatype(openvrml::browser & browser):
    node_metatype(geo_location_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_geospatial::geo_location_metatype::~geo_location_metatype()
    OPENVRML_NOTHROW
{}

# define GEO_LOCATION_INTERFACE_SEQ                                     \
    ((exposedfield, sfnode,   "metadata",       metadata))              \
    ((eventin,      mfnode,   "addChildren",    add_children_listener_)) \
    ((eventin,      mfnode,   "removeChildren", remove_children_listener_)) \
    ((exposedfield, mfnode,   "children",       children_))             \
    ((exposedfield, sfvec3d,  "geoCoords",      geo_coords_))           \
    ((field,        sfnode,   "geoOrigin",      geo_origin_))           \
    ((field,        mfstring, "geoSystem",      geo_system_))           \
    ((field,        sfvec3f,  "bboxCenter",     bbox_center_))          \
    ((field,        sfvec3f,  "bboxSize",       bbox_size_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_geospatial,
                                              geo_location_metatype,
                                              geo_location_node,
                                              GEO_LOCATION_INTERFACE_SEQ)
