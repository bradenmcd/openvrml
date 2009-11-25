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

# include "proximity_sensor.h"
# include <openvrml/local/float.h>
# include <openvrml/node_impl_util.h>
# include <openvrml/browser.h>
# include <openvrml/viewer.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

    class OPENVRML_LOCAL proximity_sensor_node :
        public openvrml::node_impl_util::abstract_node<proximity_sensor_node>,
        public openvrml::child_node {

        friend class openvrml_node_vrml97::proximity_sensor_metatype;

        exposedfield<openvrml::sfvec3f> center_;
        exposedfield<openvrml::sfbool> enabled_;
        exposedfield<openvrml::sfvec3f> size_;
        openvrml::sfbool is_active_;
        sfbool_emitter is_active_emitter_;
        openvrml::sfvec3f position_changed_;
        sfvec3f_emitter position_changed_emitter_;
        openvrml::sfrotation orientation_changed_;
        sfrotation_emitter orientation_changed_emitter_;
        openvrml::sftime enter_time_;
        sftime_emitter enter_time_emitter_;
        openvrml::sftime exit_time_;
        sftime_emitter exit_time_emitter_;
        openvrml::sfvec3f center_of_rotation_changed_;
        sfvec3f_emitter center_of_rotation_changed_emitter_;

    public:
        proximity_sensor_node(
            const openvrml::node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~proximity_sensor_node() OPENVRML_NOTHROW;

    private:
        virtual void do_render_child(openvrml::viewer & viewer,
                                     openvrml::rendering_context context);
    };

    /**
     * @class proximity_sensor_node
     *
     * @brief Represents ProximitySensor node instances.
     */

    /**
     * @var class proximity_sensor_node::proximity_sensor_metatype
     *
     * @brief Class object for ProximitySensor instances.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<proximity_sensor_node>::exposedfield<openvrml::sfvec3f> proximity_sensor_node::center_
     *
     * @brief center exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<proximity_sensor_node>::exposedfield<openvrml::sfbool> proximity_sensor_node::enabled_
     *
     * @brief enabled exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<proximity_sensor_node>::exposedfield<openvrml::sfvec3f> proximity_sensor_node::size_
     *
     * @brief size exposedField.
     */

    /**
     * @var openvrml::sfbool proximity_sensor_node::is_active_
     *
     * @brief isActive eventOut value.
     */

    /**
     * @var openvrml::sfbool_emitter proximity_sensor_node::is_active_emitter_
     *
     * @brief isActive eventOut emitter.
     */

    /**
     * @var openvrml::sfvec3f proximity_sensor_node::position_changed_
     *
     * @brief position_changed eventOut value.
     */

    /**
     * @var openvrml::sfvec3f_emitter proximity_sensor_node::position_changed_emitter_
     *
     * @brief position_changed eventOut emitter.
     */

    /**
     * @var openvrml::sfrotation proximity_sensor_node::orientation_changed_
     *
     * @brief orientation_changed eventOut value.
     */

    /**
     * @var openvrml::sfrotation proximity_sensor_node::orientation_changed_emitter_
     *
     * @brief orientation_changed eventOut emitter.
     */

    /**
     * @var openvrml::sftime proximity_sensor_node::enter_time_
     *
     * @brief enterTime eventOut value.
     */

    /**
     * @var openvrml::sftime_emitter proximity_sensor_node::enter_time_emitter_
     *
     * @brief enterTime eventOut emitter.
     */

    /**
     * @var openvrml::sftime proximity_sensor_node::exit_time_
     *
     * @brief exitTime eventOut value.
     */

    /**
     * @var openvrml::sftime_emitter proximity_sensor_node::exit_time_emitter_
     *
     * @brief exitTime eventOut emitter.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node.
     * @param scope     the scope to which the node belongs.
     */
    proximity_sensor_node::
    proximity_sensor_node(const openvrml::node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<proximity_sensor_node>(type, scope),
        child_node(type, scope),
        center_(*this, openvrml::make_vec3f(0.0, 0.0, 0.0)),
        enabled_(*this, true),
        size_(*this, openvrml::make_vec3f(0.0, 0.0, 0.0)),
        is_active_(false),
        is_active_emitter_(*this, this->is_active_),
        position_changed_emitter_(*this, this->position_changed_),
        orientation_changed_emitter_(*this, this->orientation_changed_),
        enter_time_(0.0),
        enter_time_emitter_(*this, this->enter_time_),
        exit_time_(0.0),
        exit_time_emitter_(*this, this->exit_time_),
        center_of_rotation_changed_emitter_(*this, this->center_of_rotation_changed_)
    {
        this->node::modified(true);
    }

    /**
     * @brief Destroy.
     */
    proximity_sensor_node::~proximity_sensor_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Render the node: generate proximity events.
     *
     * If necessary, events prior to the current time are generated due to
     * interpolation of enterTimes and exitTimes. The timestamp should never be
     * increased.
     *
     * This is in a render() method since the it needs the viewer position
     * with respect to the local coordinate system.
     * Could do this with Node::inverseTransform(double [4][4]) now...
     *
     * The positions and times are not interpolated to report the exact
     * place and time of entries and exits from the sensor regions as
     * required by the spec, since that would require the last viewer position
     * to be stored in the node, which is problematic in the presence of
     * DEF/USEd nodes...
     * I suppose the scene could keep the last viewer position in the global
     * coordinate system and it could be transformed all the way down the
     * scenegraph, but that sounds painful.
     *
     * @param viewer    a viewer.
     * @param context   a rendering context.
     */
    void
    proximity_sensor_node::
    do_render_child(openvrml::viewer & viewer, const openvrml::rendering_context context)
    {
        using namespace openvrml;
        if (this->enabled_.sfbool::value()
            && this->size_.sfvec3f::value().x() > 0.0
            && this->size_.sfvec3f::value().y() > 0.0
            && this->size_.sfvec3f::value().z() > 0.0
            && viewer.mode() == viewer::draw_mode) {
            using openvrml::local::fabs;
            using openvrml::local::fless_equal;

            sftime timeNow(browser::current_time());
            float x, y, z;

            // Is viewer inside the box?
            mat4f MV = context.matrix();
            MV = MV.inverse();
            x = MV[3][0]; y = MV[3][1]; z = MV[3][2];
            bool inside =
                fless_equal(fabs(x - this->center_.sfvec3f::value().x()),
                            0.5f * this->size_.sfvec3f::value().x())
                && fless_equal(fabs(y - this->center_.sfvec3f::value().y()),
                               0.5f * this->size_.sfvec3f::value().y())
                && fless_equal(fabs(z - this->center_.sfvec3f::value().z()),
                               0.5f * this->size_.sfvec3f::value().z());
            bool wasIn = this->is_active_.value();

            // Check if viewer has entered the box
            if (inside && ! wasIn) {
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

            // Check for movement within the box
            if (wasIn || inside) {
                if (this->position_changed_.value() != make_vec3f(x, y, z)) {
                    this->position_changed_.value(make_vec3f(x, y, z));
                    node::emit_event(this->position_changed_emitter_,
                                     timeNow.value());
                }

                vec3f trans, scale, shear;
                rotation orientation;
                MV.transformation(trans, orientation, scale, shear);
                if (this->orientation_changed_.value() != orientation) {
                    this->orientation_changed_.value(orientation);
                    node::emit_event(this->orientation_changed_emitter_,
                                     timeNow.value());
                }
            }
        } else {
            this->node::modified(false);
        }
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::proximity_sensor_metatype::id =
    "urn:X-openvrml:node:ProximitySensor";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype.
 */
openvrml_node_vrml97::proximity_sensor_metatype::
proximity_sensor_metatype(openvrml::browser & browser):
    node_metatype(proximity_sensor_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::proximity_sensor_metatype::~proximity_sensor_metatype()
    OPENVRML_NOTHROW
{}

# define PROXIMITY_SENSOR_INTERFACE_SEQ                                 \
    ((exposedfield, sfvec3f,    "center",                   center_))   \
    ((exposedfield, sfvec3f,    "size",                     size_))     \
    ((exposedfield, sfbool,     "enabled",                  enabled_))  \
    ((eventout,     sfbool,     "isActive",                 is_active_emitter_)) \
    ((eventout,     sfvec3f,    "position_changed",         position_changed_emitter_)) \
    ((eventout,     sfrotation, "orientation_changed",      orientation_changed_emitter_)) \
    ((eventout,     sftime,     "enterTime",                enter_time_emitter_)) \
    ((eventout,     sftime,     "exitTime",                 exit_time_emitter_)) \
    ((exposedfield, sfnode,     "metadata",                 metadata))  \
    ((eventout,     sfvec3f,    "centerOfRotation_changed", center_of_rotation_changed_emitter_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              proximity_sensor_metatype,
                                              proximity_sensor_node,
                                              PROXIMITY_SENSOR_INTERFACE_SEQ)
