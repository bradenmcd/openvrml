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
# include <private.h>
# include <openvrml/browser.h>
# include "collision.h"
# include "grouping_node_base.h"

namespace {

    class OPENVRML_LOCAL collision_node :
        public openvrml_node_vrml97::grouping_node_base<collision_node> {

        friend class openvrml_node_vrml97::collision_metatype;

        exposedfield<openvrml::sfbool> collide_;
        openvrml::sfnode proxy_;
        openvrml::sftime collide_time_;
        sftime_emitter collide_time_emitter_;
        openvrml::sfbool is_active_;
        sfbool_emitter is_active_emitter_;

    public:
        collision_node(const openvrml::node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~collision_node() OPENVRML_NOTHROW;

    private:
        virtual bool do_modified() const
            OPENVRML_THROW1(boost::thread_resource_error);
    };


    /**
     * @class collision_node
     *
     * @brief Collision node instances.
     */

    /**
     * @var class collision_node::collision_metatype
     *
     * @brief Class object for Collision nodes.
     */

    /**
     * @var openvrml::sfbool collision_node::collide_
     *
     * @brief collide exposedField.
     */

    /**
     * @var openvrml::sfnode collision_node::proxy_
     *
     * @brief proxy field.
     */

    /**
     * @var openvrml::sftime collision_node::collide_time_
     *
     * @brief collideTime eventOut.
     */

    /**
     * @var openvrml::sftime_emitter collision_node::collide_time_emitter_
     *
     * @brief collideTime eventOut.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node instance.
     * @param scope     the scope to which the node belongs.
     */
    collision_node::
    collision_node(const openvrml::node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        child_node(type, scope),
        grouping_node(type, scope),
        openvrml_node_vrml97::grouping_node_base<collision_node>(type, scope),
        collide_(*this, true),
        collide_time_emitter_(*this, this->collide_time_),
        is_active_emitter_(*this, this->is_active_)
    {}

    /**
     * @brief Destroy.
     */
    collision_node::~collision_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Determine whether the node has been modified.
     *
     * @return @c true if the node or one of its children has been modified,
     *      @c false otherwise.
     */
    bool collision_node::do_modified() const
        OPENVRML_THROW1(boost::thread_resource_error)
    {
        return (this->proxy_.value() && this->proxy_.value()->modified())
            || (this->openvrml_node_vrml97::grouping_node_base<collision_node>::
                do_modified());
    }
}

/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::collision_metatype::id =
    "urn:X-openvrml:node:Collision";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this class object.
 */
openvrml_node_vrml97::collision_metatype::
collision_metatype(openvrml::browser & browser):
    node_metatype(collision_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::collision_metatype::~collision_metatype()
    OPENVRML_NOTHROW
{}

# define COLLISION_INTERFACE_SEQ                                        \
    ((eventin,      mfnode,  "addChildren",    add_children_listener_)) \
    ((eventin,      mfnode,  "removeChildren", remove_children_listener_)) \
    ((exposedfield, mfnode,  "children",       children_))              \
    ((exposedfield, sfbool,  "collide",        collide_))               \
    ((field,        sfvec3f, "bboxCenter",     bbox_center_))           \
    ((field,        sfvec3f, "bboxSize",       bbox_size_))             \
    ((field,        sfnode,  "proxy",          proxy_))                 \
    ((eventout,     sftime,  "collideTime",    collide_time_emitter_))  \
    ((exposedfield, sfnode,  "metadata",       metadata))               \
    ((exposedfield, sfbool,  "enabled",        collide_))               \
    ((eventout,     sfbool,  "isActive",       is_active_emitter_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              collision_metatype,
                                              collision_node,
                                              COLLISION_INTERFACE_SEQ)
