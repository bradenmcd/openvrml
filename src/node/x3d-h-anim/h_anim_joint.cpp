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

# include "h_anim_joint.h"
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
     * @brief Represents HAnimJoint node instances.
     */
    class OPENVRML_LOCAL h_anim_joint_node :
        public abstract_node<h_anim_joint_node>,
        public child_node {

        friend class openvrml_node_x3d_h_anim::h_anim_joint_metatype;

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
        exposedfield<sfvec3f> center_;
        exposedfield<mfnode> children_;
        exposedfield<mfnode> displacers_;
        exposedfield<sfrotation> limit_orientation_;
        exposedfield<mffloat> llimit_;
        exposedfield<sfstring> name_;
        exposedfield<sfrotation> rotation_;
        exposedfield<sfvec3f> scale_;
        exposedfield<sfrotation> scale_orientation_;
        exposedfield<mfint32> skin_coord_index_;
        exposedfield<mffloat> skin_coord_weight_;
        exposedfield<mffloat> stiffness_;
        exposedfield<sfvec3f> translation_;
        exposedfield<mffloat> ulimit_;
        sfvec3f bbox_center_;
        sfvec3f bbox_size_;

    public:
        h_anim_joint_node(const node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~h_anim_joint_node() OPENVRML_NOTHROW;
    };


    /**
     * @var h_anim_joint_node::h_anim_joint_metatype
     *
     * @brief Class object for HAnimJoint nodes.
     */

    /**
     * @var h_anim_joint_node::add_children_listener h_anim_joint_node::add_children_listener_
     *
     * @brief add_children eventIn listener.
     */

    /**
     * @var h_anim_joint_node::remove_children_listener h_anim_joint_node::remove_children_listener_
     *
     * @brief remove_children eventIn listener.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<h_anim_joint_node>::exposedfield<sfvec3f> h_anim_joint_node::center_
     *
     * @brief center exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<h_anim_joint_node>::exposedfield<mfnode> h_anim_joint_node::children_
     *
     * @brief children exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<h_anim_joint_node>::exposedfield<mfnode> h_anim_joint_node::displacers_
     *
     * @brief displacers exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<h_anim_joint_node>::exposedfield<sfrotation> h_anim_joint_node::limit_orientation_
     *
     * @brief limit_orientation exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<h_anim_joint_node>::exposedfield<mffloat> h_anim_joint_node::llimit_
     *
     * @brief llimit exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<h_anim_joint_node>::exposedfield<sfstring> h_anim_joint_node::name_
     *
     * @brief name exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<h_anim_joint_node>::exposedfield<sfrotation> h_anim_joint_node::rotation_
     *
     * @brief rotation exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<h_anim_joint_node>::exposedfield<sfvec3f> h_anim_joint_node::scale_
     *
     * @brief scale exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<h_anim_joint_node>::exposedfield<sfrotation> h_anim_joint_node::scale_orientation_
     *
     * @brief scale_orientation exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<h_anim_joint_node>::exposedfield<mfint32> h_anim_joint_node::skin_coord_index_
     *
     * @brief skin_coord_index exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<h_anim_joint_node>::exposedfield<mffloat> h_anim_joint_node::skin_coord_weight_
     *
     * @brief skin_coord_weight exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<h_anim_joint_node>::exposedfield<mffloat> h_anim_joint_node::stiffness_
     *
     * @brief stiffness exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<h_anim_joint_node>::exposedfield<sfvec3f> h_anim_joint_node::translation_
     *
     * @brief translation exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<h_anim_joint_node>::exposedfield<mffloat> h_anim_joint_node::ulimit_
     *
     * @brief ulimit exposedField
     */

    /**
     * @var openvrml::sfvec3f h_anim_joint_node::bbox_center_
     *
     * @brief bbox_center field
     */

    /**
     * @var openvrml::sfvec3f h_anim_joint_node::bbox_size_
     *
     * @brief bbox_size field
     */

    h_anim_joint_node::add_children_listener::
    add_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    h_anim_joint_node::add_children_listener::
    ~add_children_listener() OPENVRML_NOTHROW
    {}

    void h_anim_joint_node::add_children_listener::
    do_process_event(const mfnode & /* fraction */, double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    h_anim_joint_node::remove_children_listener::
    remove_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    h_anim_joint_node::remove_children_listener::
    ~remove_children_listener() OPENVRML_NOTHROW
    {}

    void h_anim_joint_node::remove_children_listener::
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
    h_anim_joint_node::
    h_anim_joint_node(const node_type & type,
                     const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        add_children_listener_(*this),
        remove_children_listener_(*this),
        center_(*this),
        children_(*this),
        displacers_(*this),
        limit_orientation_(*this),
        llimit_(*this),
        name_(*this),
        rotation_(*this),
        scale_(*this, make_vec3f(1.0, 1.0, 1.0)),
        scale_orientation_(*this),
        skin_coord_index_(*this),
        skin_coord_weight_(*this),
        stiffness_(*this, vector<float>(3,0.0f)),
        translation_(*this),
        ulimit_(*this),
        bbox_size_(make_vec3f(-1.0f, -1.0f, -1.0f))
    {}

    /**
     * @brief Destroy.
     */
    h_anim_joint_node::~h_anim_joint_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_h_anim::h_anim_joint_metatype::id =
    "urn:X-openvrml:node:HAnimJoint";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c h_anim_joint_metatype.
 */
openvrml_node_x3d_h_anim::h_anim_joint_metatype::
h_anim_joint_metatype(openvrml::browser & browser):
    node_metatype(h_anim_joint_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_h_anim::h_anim_joint_metatype::~h_anim_joint_metatype()
    OPENVRML_NOTHROW
{}

# define H_ANIM_JOINT_INTERFACE_SEQ                                     \
    ((exposedfield, sfnode,     "metadata",         metadata))          \
    ((eventin,      mfnode,     "addChildren",      add_children_listener_)) \
    ((eventin,      mfnode,     "removeChildren",   remove_children_listener_)) \
    ((exposedfield, sfvec3f,    "center",           center_))           \
    ((exposedfield, mfnode,     "children",         children_))         \
    ((exposedfield, mfnode,     "displacers",       displacers_))       \
    ((exposedfield, sfrotation, "limitOrientation", limit_orientation_)) \
    ((exposedfield, mffloat,    "llimit",           llimit_))           \
    ((exposedfield, sfstring,   "name",             name_))             \
    ((exposedfield, sfrotation, "rotation",         rotation_))         \
    ((exposedfield, sfvec3f,    "scale",            scale_))            \
    ((exposedfield, sfrotation, "scaleOrientation", scale_orientation_)) \
    ((exposedfield, mfint32,    "skinCoordIndex",   skin_coord_index_)) \
    ((exposedfield, mffloat,    "stiffness",        stiffness_))        \
    ((exposedfield, sfvec3f,    "translation",      translation_))      \
    ((exposedfield, mffloat,    "ulimit",           ulimit_))           \
    ((field,        sfvec3f,    "bboxCenter",       bbox_center_))      \
    ((field,        sfvec3f,    "bboxSize",         bbox_size_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_h_anim,
                                              h_anim_joint_metatype,
                                              h_anim_joint_node,
                                              H_ANIM_JOINT_INTERFACE_SEQ)
