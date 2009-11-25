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

# include "nurbs_set.h"
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
     * @brief Represents NurbsSet node instances.
     */
    class OPENVRML_LOCAL nurbs_set_node :
        public abstract_node<nurbs_set_node>,
        public child_node {

        friend class openvrml_node_x3d_nurbs::nurbs_set_metatype;

        class add_geometry_listener : public event_listener_base<self_t>,
                                      public mfnode_listener {
        public:
            explicit add_geometry_listener(self_t & node);
            virtual ~add_geometry_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const mfnode & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class remove_geometry_listener :
                public event_listener_base<self_t>,
                public mfnode_listener
        {
        public:
            explicit remove_geometry_listener(self_t & node);
            virtual ~remove_geometry_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const mfnode & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        add_geometry_listener add_geometry_listener_;
        remove_geometry_listener remove_geometry_listener_;
        exposedfield<mfnode> geometry_;
        exposedfield<sffloat> tessellation_scale_;
        sfvec3f bbox_center_;
        sfvec3f bbox_size_;

    public:
        nurbs_set_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~nurbs_set_node() OPENVRML_NOTHROW;
    };


    /**
     * @var nurbs_set_node::nurbs_set_metatype
     *
     * @brief Class object for NurbsSet nodes.
     */

    /**
     * @var nurbs_set_node::add_geometry_
     *
     * @brief add_geometry eventIn
     */

    /**
     * @var nurbs_set_node::remove_geometry_
     *
     * @brief remove_geometry eventIn
     */

    /**
     * @var nurbs_set_node::geometry_
     *
     * @brief geometry exposedField
     */

    /**
     * @var nurbs_set_node::tessellation_scale_
     *
     * @brief tessellation_scale exposedField
     */

    /**
     * @var nurbs_set_node::bbox_center_
     *
     * @brief bbox_center field
     */

    /**
     * @var nurbs_set_node::bbox_size_
     *
     * @brief bbox_size field
     */

    nurbs_set_node::add_geometry_listener::
    add_geometry_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    nurbs_set_node::add_geometry_listener::
    ~add_geometry_listener() OPENVRML_NOTHROW
    {}

    void nurbs_set_node::add_geometry_listener::
    do_process_event(const mfnode & /* fraction */, double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    nurbs_set_node::remove_geometry_listener::
    remove_geometry_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    nurbs_set_node::remove_geometry_listener::
    ~remove_geometry_listener() OPENVRML_NOTHROW
    {}

    void nurbs_set_node::remove_geometry_listener::
    do_process_event(const mfnode & /* fraction */, double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the @c node_type associated with this node.
     * @param scope the @c scope to which the node belongs.
     */
    nurbs_set_node::
    nurbs_set_node(const node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        add_geometry_listener_(*this),
        remove_geometry_listener_(*this),
        geometry_(*this),
        tessellation_scale_(*this, 1),
        bbox_size_(make_vec3f(-1.0f, -1.0f, -1.0f))
    {}

    /**
     * @brief Destroy.
     */
    nurbs_set_node::~nurbs_set_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_nurbs::nurbs_set_metatype::id =
    "urn:X-openvrml:node:NurbsSet";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c nurbs_set_metatype.
 */
openvrml_node_x3d_nurbs::nurbs_set_metatype::
nurbs_set_metatype(openvrml::browser & browser):
    node_metatype(nurbs_set_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_nurbs::nurbs_set_metatype::~nurbs_set_metatype()
    OPENVRML_NOTHROW
{}

# define NURBS_SET_INTERFACE_SEQ                                        \
    ((exposedfield, sfnode,  "metadata",          metadata))            \
    ((eventin,      mfnode,  "addGeometry",       add_geometry_listener_)) \
    ((eventin,      mfnode,  "removeGeometry",    remove_geometry_listener_)) \
    ((exposedfield, mfnode,  "geometry",          geometry_))           \
    ((exposedfield, sffloat, "tessellationScale", tessellation_scale_)) \
    ((field,        sfvec3f, "bboxCenter",        bbox_center_))        \
    ((field,        sfvec3f, "bboxSize",          bbox_size_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_nurbs,
                                              nurbs_set_metatype,
                                              nurbs_set_node,
                                              NURBS_SET_INTERFACE_SEQ)
