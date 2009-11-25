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

# include "visibility_sensor.h"
# include <openvrml/local/float.h>
# include <openvrml/node_impl_util.h>
# include <openvrml/browser.h>
# include <openvrml/viewer.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

    class OPENVRML_LOCAL visibility_sensor_node :
        public openvrml::node_impl_util::abstract_node<visibility_sensor_node>,
        public openvrml::child_node {

        friend class openvrml_node_vrml97::visibility_sensor_metatype;

        exposedfield<openvrml::sfvec3f> center_;
        exposedfield<openvrml::sfbool> enabled_;
        exposedfield<openvrml::sfvec3f> size_;
        openvrml::sfbool is_active_;
        sfbool_emitter is_active_emitter_;
        openvrml::sftime enter_time_;
        sftime_emitter enter_time_emitter_;
        openvrml::sftime exit_time_;
        sftime_emitter exit_time_emitter_;

    public:
        visibility_sensor_node(
            const openvrml::node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~visibility_sensor_node() OPENVRML_NOTHROW;

    private:
        virtual void do_render_child(openvrml::viewer & viewer,
                                     openvrml::rendering_context context);
    };

    /**
     * @class visibility_sensor_node
     *
     * @brief VisibilitySensor node instances.
     */

    /**
     * @var class visibility_sensor_node::visibility_sensor_metatype
     *
     * @brief Class object for VisibilitySensor instances.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<visibility_sensor_node>::exposedfield<openvrml::sfvec3f> visibility_sensor_node::center_
     *
     * @brief center exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<visibility_sensor_node>::exposedfield<openvrml::sfbool> visibility_sensor_node::enabled_
     *
     * @brief enabled exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<visibility_sensor_node>::exposedfield<openvrml::sfvec3f> visibility_sensor_node::size_
     *
     * @brief size exposedField.
     */

    /**
     * @var openvrml::sfbool visibility_sensor_node::is_active_
     *
     * @brief isActive eventOut value.
     */

    /**
     * @var openvrml::sfbool_emitter visibility_sensor_node::is_active_emitter_
     *
     * @brief isActive eventOut emitter.
     */

    /**
     * @var openvrml::sftime visibility_sensor_node::enter_time_
     *
     * @brief enterTime eventOut value.
     */

    /**
     * @var openvrml::sftime_emitter visibility_sensor_node::enter_time_emitter_
     *
     * @brief enterTime eventOut emitter.
     */

    /**
     * @var openvrml::sftime visibility_sensor_node::exit_time_
     *
     * @brief exitTime eventOut value.
     */

    /**
     * @var openvrml::sftime_emitter visibility_sensor_node::exit_time_emitter_
     *
     * @brief exitTime eventOut emitter.
     */

    /**
     * @brief Construct
     *
     * @param type  the node_type associated with the instance.
     * @param scope the scope that the new node will belong to.
     */
    visibility_sensor_node::
    visibility_sensor_node(const openvrml::node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<visibility_sensor_node>(type, scope),
        child_node(type, scope),
        center_(*this, openvrml::make_vec3f(0.0, 0.0, 0.0)),
        enabled_(*this, true),
        size_(*this, openvrml::make_vec3f(0.0, 0.0, 0.0)),
        is_active_(false),
        is_active_emitter_(*this, this->is_active_),
        enter_time_(0.0),
        enter_time_emitter_(*this, this->enter_time_),
        exit_time_(0.0),
        exit_time_emitter_(*this, this->exit_time_)
    {
        this->node::modified(true);
    }

    /**
     * @brief Destroy.
     */
    visibility_sensor_node::~visibility_sensor_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Generate visibility events.
     *
     * @todo This is in a render() method since the it needs to be computed
     *       with respect to the accumulated transformations above it in the
     *       scene graph. Move to update() when xforms are accumulated in
     *       Groups...
     *
     * @param viewer    viewer.
     * @param context   rendering_context.
     */
    void
    visibility_sensor_node::do_render_child(openvrml::viewer & viewer,
                                            openvrml::rendering_context)
    {
        if (this->enabled_.sfbool::value()) {
            openvrml::sftime timeNow(openvrml::browser::current_time());
            openvrml::vec3f xyz[2] = {
                this->center_.sfvec3f::value(),
                this->center_.sfvec3f::value() + this->size_.sfvec3f::value()
            };

            // hack: enclose box in a sphere...
            viewer.transform_points(2, &xyz[0]);
            float r  = (xyz[1] - xyz[0]).length();

            // Was the sphere visible last time through? How does this work
            // for USE'd nodes? I need a way for each USE to store whether
            // it was active.
            bool wasIn = this->is_active_.value();

            // Is the sphere visible? ...
            bool inside = xyz[0].z() < 0.0; // && z > - scene->visLimit()
            if (inside) {
                using openvrml::local::fequal;

                openvrml::navigation_info_node & nav_info =
                    this->type().metatype().browser()
                    .active_navigation_info();
                if (!fequal(nav_info.visibility_limit(), 0.0f)
                    && xyz[0][2] < -(nav_info.visibility_limit())) {
                    inside = false;
                }
            }

            // This bit assumes 90degree fieldOfView to get rid of trig
            // calls...
            if (inside) {
                inside = fabs(xyz[0].x()) < -0.5 * xyz[0].z() + r
                    && fabs(xyz[0].y()) < -0.5 * xyz[0].z() + r;
            }

            // Just became visible
            if (inside && !wasIn) {
                this->is_active_.value(true);
                node::emit_event(this->is_active_emitter_, timeNow.value());

                this->enter_time_ = timeNow;
                node::emit_event(this->enter_time_emitter_, timeNow.value());
            }

            // Check if viewer has left the box
            else if (wasIn && !inside) {
                this->is_active_.value(false);
                node::emit_event(this->is_active_emitter_, timeNow.value());

                this->exit_time_ = timeNow;
                node::emit_event(this->exit_time_emitter_, timeNow.value());
            }
        } else {
            this->node::modified(false);
        }
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::visibility_sensor_metatype::id =
    "urn:X-openvrml:node:VisibilitySensor";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype.
 */
openvrml_node_vrml97::visibility_sensor_metatype::
visibility_sensor_metatype(openvrml::browser & browser):
    node_metatype(visibility_sensor_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::visibility_sensor_metatype::~visibility_sensor_metatype()
    OPENVRML_NOTHROW
{}

# define VISIBILITY_SENSOR_INTERFACE_SEQ                        \
    ((exposedfield, sfvec3f, "center",    center_))             \
    ((exposedfield, sfbool,  "enabled",   enabled_))            \
    ((exposedfield, sfvec3f, "size",      size_))               \
    ((eventout,     sftime,  "enterTime", enter_time_emitter_)) \
    ((eventout,     sftime,  "exitTime",  exit_time_emitter_))  \
    ((eventout,     sfbool,  "isActive",  is_active_emitter_))  \
    ((exposedfield, sfnode,  "metadata",  metadata))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              visibility_sensor_metatype,
                                              visibility_sensor_node,
                                              VISIBILITY_SENSOR_INTERFACE_SEQ)
