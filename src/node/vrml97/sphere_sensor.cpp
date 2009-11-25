// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2009  Braden McDaniel
// Copyright 2002  S. K. Bose
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
# include <openvrml/node_impl_util.h>
# include <private.h>
# include "sphere_sensor.h"

namespace {

    class OPENVRML_LOCAL sphere_sensor_node :
        public openvrml::node_impl_util::abstract_node<sphere_sensor_node>,
        public openvrml::pointing_device_sensor_node {

        friend class openvrml_node_vrml97::sphere_sensor_metatype;

        exposedfield<openvrml::sfbool> auto_offset_;
        exposedfield<openvrml::sfbool> enabled_;
        exposedfield<openvrml::sfrotation> offset_;
        exposedfield<openvrml::sfstring> description_;
        openvrml::sfbool is_active_;
        sfbool_emitter is_active_emitter_;
        openvrml::sfrotation rotation_changed_;
        sfrotation_emitter rotation_changed_emitter_;
        openvrml::sfvec3f track_point_changed_;
        sfvec3f_emitter track_point_changed_emitter_;
        openvrml::sfbool is_over_;
        sfbool_emitter is_over_emitter_;

        openvrml::sfvec3f activationPoint;
        openvrml::sfvec3f centerPoint;
        openvrml::mat4f modelview;

    public:
        sphere_sensor_node(const openvrml::node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~sphere_sensor_node() OPENVRML_NOTHROW;

    private:
        virtual void do_render_child(openvrml::viewer & viewer,
                                     openvrml::rendering_context context);
        virtual void do_activate(double timestamp, bool over, bool active,
                                 const double (&point)[3]);
    };

    /**
     * @class sphere_sensor_node
     *
     * @brief SphereSensor node instances.
     */

    /**
     * @var class sphere_sensor_node::sphere_sensor_metatype
     *
     * @brief Class object for SphereSensor instances.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<sphere_sensor_node>::exposedfield<openvrml::sfbool> sphere_sensor_node::auto_offset_
     *
     * @brief autoOffset exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<sphere_sensor_node>::exposedfield<openvrml::sfbool> sphere_sensor_node::enabled_
     *
     * @brief enabled exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<sphere_sensor_node>::exposedfield<openvrml::sfrotation> sphere_sensor_node::offset_
     *
     * @brief offset exposedField.
     */

    /**
     * @var openvrml::sfbool sphere_sensor_node::is_active_
     *
     * @brief isActive eventOut value.
     */

    /**
     * @var openvrml::sfbool_emitter sphere_sensor_node::is_active_emitter_
     *
     * @brief isActive eventOut emitter.
     */

    /**
     * @var openvrml::sfrotation sphere_sensor_node::rotation_changed_
     *
     * @brief rotation_changed eventOut value.
     */

    /**
     * @var openvrml::sfrotation_emitter sphere_sensor_node::rotation_changed_emitter_
     *
     * @brief rotation_changed eventOut emitter.
     */

    /**
     * @var openvrml::sfvec3f sphere_sensor_node::track_point_changed_
     *
     * @brief trackPoint_changed eventOut.
     */

    /**
     * @var openvrml::sfvec3f_emitter sphere_sensor_node::track_point_changed_emitter_
     *
     * @brief trackPoint_changed eventOut emitter.
     */

    /**
     * @var openvrml::sfvec3f sphere_sensor_node::activationPoint
     *
     * @brief The start point of a drag operation.
     */

    /**
     * @var openvrml::sfvec3f sphere_sensor_node::centerPoint
     *
     * @brief Center point.
     */

    /**
     * @var openvrml::mat4f sphere_sensor_node::modelview
     *
     * @brief Modelview matrix.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node instance.
     * @param scope     the scope to which the node belongs.
     */
    sphere_sensor_node::
    sphere_sensor_node(const openvrml::node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        child_node(type, scope),
        openvrml::node_impl_util::abstract_node<sphere_sensor_node>(type, scope),
        pointing_device_sensor_node(type, scope),
        auto_offset_(*this, true),
        enabled_(*this, true),
        offset_(*this, openvrml::make_rotation(0.0, 1.0, 0.0, 0.0)),
        description_(*this),
        is_active_(false),
        is_active_emitter_(*this, this->is_active_),
        rotation_changed_emitter_(*this, this->rotation_changed_),
        track_point_changed_emitter_(*this, this->track_point_changed_),
        is_over_emitter_(*this, this->is_over_),
        modelview(openvrml::make_mat4f())
    {
        this->node::modified(true);
    }

    /**
     * @brief Destroy.
     */
    sphere_sensor_node::~sphere_sensor_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Render the node.
     *
     * @param viewer    a viewer.
     * @param context   a rendering context.
     */
    void
    sphere_sensor_node::
    do_render_child(openvrml::viewer &, const openvrml::rendering_context context)
    {
        //
        // Store the ModelView matrix which is calculated at the time of
        // rendering in render-context. This matrix will be in use at the time
        // of activation
        //
        this->modelview = context.matrix();
    }

    /**
     * @brief Activate or deactivate the sphere_sensor_node.
     *
     * Activating a drag sensor means that the pointing device button has been
     * depressed and a drag operation has been initiated. The sensor is
     * deactivated when the button is released at the end of the operation.
     *
     * @param timestamp the current time.
     * @param over      @c true if the pointer is over affected geometry;
     *                  @c false otherwise.
     * @param active    @c true if the drag operation is in progress; @c false
     *                  otherwise.
     * @param p         the pointing device position.
     */
    void
    sphere_sensor_node::do_activate(double timestamp,
                                    bool,
                                    bool active,
                                    const double (&p)[3])
    {
        if (this->enabled_.sfbool::value()) {
            using openvrml::vec3f;
            using openvrml::make_vec3f;
            using openvrml::mat4f;
            // Become active
            if (active && !this->is_active_.value()) {
                this->is_active_.value(active);

                // set activation point in world coords
                const vec3f floatVec = make_vec3f(static_cast<float>(p[0]),
                                                  static_cast<float>(p[1]),
                                                  static_cast<float>(p[2]));
                this->activationPoint.value(floatVec);

                if (this->auto_offset_.sfbool::value()) {
                    this->rotation_changed_ = this->offset_;
                }

                // calculate the center of the object in world coords
                vec3f V;
                mat4f M = this->modelview.inverse();
                V = V * M;
                this->centerPoint.value(V);

                // send message
                node::emit_event(this->is_active_emitter_, timestamp);
            }
            // Become inactive
            else if (!active && this->is_active_.value()) {
                this->is_active_.value(active);
                node::emit_event(this->is_active_emitter_, timestamp);

                // save auto offset of rotation
                if (this->auto_offset_.sfbool::value()) {
                    this->offset_.sfrotation::value(
                        this->rotation_changed_.value());
                    node::emit_event(this->offset_, timestamp);
                }
            }
            // Tracking
            else if (active) {
                // get local coord for touch point
                vec3f V = make_vec3f(static_cast<float>(p[0]),
                                     static_cast<float>(p[1]),
                                     static_cast<float>(p[2]));
                mat4f M = this->modelview.inverse();
                V = V * M;
                this->track_point_changed_.value(V);
                node::emit_event(this->track_point_changed_emitter_,
                                 timestamp);

                vec3f V2 = make_vec3f(static_cast<float>(p[0]),
                                      static_cast<float>(p[1]),
                                      static_cast<float>(p[2]));
                vec3f tempv = V2 - this->centerPoint.value();
                vec3f dir1(tempv);

                //
                // Get the length of the pre-normalized vector.
                //
                const float dist = dir1.length();

                dir1 = dir1.normalize();
                tempv =
                    this->activationPoint.value() - this->centerPoint.value();
                vec3f dir2(tempv);
                dir2 = dir2.normalize();

                tempv = dir1 * dir2;
                vec3f cx(tempv);
                cx = cx.normalize();

                openvrml::rotation newRot =
                    make_rotation(cx, dist * float(acos(dir1.dot(dir2))));
                if (this->auto_offset_.sfbool::value()) {
                    newRot = newRot * this->offset_.sfrotation::value();
                }
                this->rotation_changed_.value(newRot);

                node::emit_event(this->rotation_changed_emitter_, timestamp);
            }
        }
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::sphere_sensor_metatype::id =
    "urn:X-openvrml:node:SphereSensor";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype.
 */
openvrml_node_vrml97::sphere_sensor_metatype::
sphere_sensor_metatype(openvrml::browser & browser):
    node_metatype(sphere_sensor_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::sphere_sensor_metatype::~sphere_sensor_metatype()
    OPENVRML_NOTHROW
{}

# define SPHERE_SENSOR_INTERFACE_SEQ \
    ((exposedfield, sfbool,     "autoOffset",         auto_offset_)) \
    ((exposedfield, sfbool,     "enabled",            enabled_)) \
    ((exposedfield, sfrotation, "offset",             offset_)) \
    ((eventout,     sfbool,     "isActive",           is_active_emitter_)) \
    ((eventout,     sfrotation, "rotation_changed",   rotation_changed_emitter_)) \
    ((eventout,     sfvec3f,    "trackPoint_changed", track_point_changed_emitter_)) \
    ((exposedfield, sfnode,     "metadata",           metadata)) \
    ((exposedfield, sfstring,   "description",        description_)) \
    ((eventout,     sfbool,     "isOver",             is_over_emitter_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              sphere_sensor_metatype,
                                              sphere_sensor_node,
                                              SPHERE_SENSOR_INTERFACE_SEQ)
