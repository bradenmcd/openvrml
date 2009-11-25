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

# include "geo_lod.h"
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
     * @brief Represents GeoLOD node instances.
     */
    class OPENVRML_LOCAL geo_lod_node : public abstract_node<geo_lod_node>,
                                        public child_node {
        friend class openvrml_node_x3d_geospatial::geo_lod_metatype;

        class add_children_listener :
                public event_listener_base<self_t>,
                public mfnode_listener
        {
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
        mfnode children_;
        mfnode_emitter children_emitter_;
        sfvec3d center_;
        mfstring child1url_;
        mfstring child2url_;
        mfstring child3url_;
        mfstring child4url_;
        sfnode geo_origin_;
        mfstring geo_system_;
        sffloat range_;
        mfstring root_url_;
        mfnode root_node_;
        sfvec3f bbox_center_;
        sfvec3f bbox_size_;

    public:
        geo_lod_node(const node_type & type,
                     const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~geo_lod_node() OPENVRML_NOTHROW;
    };


    /**
     * @var geo_lod_node::geo_lod_metatype
     *
     * @brief Class object for GeoLOD nodes.
     */

    /**
     * @var geo_lod_node::add_children_listener geo_lod_node::add_children_listener_
     *
     * @brief add_children eventIn
     */

    /**
     * @var geo_lod_node::remove_children_listener geo_lod_node::remove_children_listener_
     *
     * @brief remove_children eventIn
     */

    /**
     * @var geo_lod_node::children_
     *
     * @brief children eventOut
     */

    /**
     * @var geo_lod_node::center_
     *
     * @brief center field
     */

    /**
     * @var geo_lod_node::child1url_
     *
     * @brief child1url field
     */

    /**
     * @var geo_lod_node::child2url_
     *
     * @brief child2url field
     */

    /**
     * @var geo_lod_node::child3url_
     *
     * @brief child3url field
     */

    /**
     * @var geo_lod_node::child4url_
     *
     * @brief child4url field
     */

    /**
     * @var geo_lod_node::geo_origin_
     *
     * @brief geo_origin field
     */

    /**
     * @var geo_lod_node::geo_system_
     *
     * @brief geo_system field
     */

    /**
     * @var geo_lod_node::range_
     *
     * @brief range field
     */

    /**
     * @var geo_lod_node::root_url_
     *
     * @brief root_url field
     */

    /**
     * @var geo_lod_node::root_node_
     *
     * @brief root_node field
     */

    /**
     * @var geo_lod_node::bbox_center_
     *
     * @brief bbox_center field
     */

    /**
     * @var geo_lod_node::bbox_size_
     *
     * @brief bbox_size field
     */

    geo_lod_node::add_children_listener::
    add_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    geo_lod_node::add_children_listener::
    ~add_children_listener() OPENVRML_NOTHROW
    {}

    void geo_lod_node::add_children_listener::
    do_process_event(const mfnode & /* fraction */, double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    geo_lod_node::remove_children_listener::
    remove_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    geo_lod_node::remove_children_listener::
    ~remove_children_listener() OPENVRML_NOTHROW
    {}

    void geo_lod_node::remove_children_listener::
    do_process_event(const mfnode & /* fraction */, double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    geo_lod_node::
    geo_lod_node(const node_type & type,
                 const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        add_children_listener_(*this),
        remove_children_listener_(*this),
        children_emitter_(*this, this->children_),
        geo_system_(openvrml_node_x3d_geospatial::default_geo_system),
        range_(10),
        bbox_size_(make_vec3f(-1.0f, -1.0f, -1.0f))
    {}

    /**
     * @brief Destroy.
     */
    geo_lod_node::~geo_lod_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_geospatial::geo_lod_metatype::id =
    "urn:X-openvrml:node:GeoLOD";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c geo_lod_metatype.
 */
openvrml_node_x3d_geospatial::geo_lod_metatype::
geo_lod_metatype(openvrml::browser & browser):
    node_metatype(geo_lod_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_geospatial::geo_lod_metatype::~geo_lod_metatype()
    OPENVRML_NOTHROW
{}

# define GEO_LOD_INTERFACE_SEQ                                          \
    ((exposedfield, sfnode,   "metadata",       metadata))              \
    ((eventin,      mfnode,   "addChildren",    add_children_listener_)) \
    ((eventin,      mfnode,   "removeChildren", remove_children_listener_)) \
    ((field,        mfnode,   "children",       children_))             \
    ((field,        sfvec3d,  "center",         center_))               \
    ((field,        mfstring, "child1Url",      child1url_))            \
    ((field,        mfstring, "child2Url",      child2url_))            \
    ((field,        mfstring, "child3Url",      child3url_))            \
    ((field,        mfstring, "child4Url",      child4url_))            \
    ((field,        sfnode,   "geoOrigin",      geo_origin_))           \
    ((field,        mfstring, "geoSystem",      geo_system_))           \
    ((field,        sffloat,  "range",          range_))                \
    ((field,        mfstring, "rootUrl",        root_url_))             \
    ((field,        mfnode,   "rootNode",       root_node_))            \
    ((field,        sfvec3f,  "bboxCenter",     bbox_center_))          \
    ((field,        sfvec3f,  "bboxSize",       bbox_size_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_geospatial,
                                              geo_lod_metatype,
                                              geo_lod_node,
                                              GEO_LOD_INTERFACE_SEQ)
