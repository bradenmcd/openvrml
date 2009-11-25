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

# include "orientation_interpolator.h"
# include <openvrml/node_impl_util.h>
# include <openvrml/local/float.h>
# include <private.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

    class OPENVRML_LOCAL orientation_interpolator_node :
        public openvrml::node_impl_util::abstract_node<orientation_interpolator_node>,
        public openvrml::child_node {

        friend class openvrml_node_vrml97::orientation_interpolator_metatype;

        class set_fraction_listener : public openvrml::node_impl_util::event_listener_base<self_t>,
                                      public sffloat_listener {
        public:
            explicit set_fraction_listener(
                orientation_interpolator_node & node);
            virtual ~set_fraction_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::sffloat & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_fraction_listener set_fraction_listener_;
        exposedfield<openvrml::mffloat> key_;
        exposedfield<openvrml::mfrotation> key_value_;
        openvrml::sfrotation value_changed_;
        sfrotation_emitter value_changed_emitter_;

    public:
        orientation_interpolator_node(
            const openvrml::node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~orientation_interpolator_node() OPENVRML_NOTHROW;
    };

    /**
     * @class orientation_interpolator_node
     *
     * @brief OrientationInterpolator node instances.
     */

    /**
     * @var class orientation_interpolator_node::orientation_interpolator_metatype
     *
     * @brief Class object for OrientationInterpolator nodes.
     */

    /**
     * @internal
     *
     * @class orientation_interpolator_node::set_fraction_listener
     *
     * @brief set_fraction eventIn handler.
     */

    /**
     * @brief Construct.
     *
     * @param node  orientation_interpolator_node.
     */
    orientation_interpolator_node::set_fraction_listener::
    set_fraction_listener(orientation_interpolator_node & node):
        openvrml::node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<orientation_interpolator_node>(node),
        sffloat_listener(node)
    {}

    /**
     * @brief Destroy.
     */
    orientation_interpolator_node::set_fraction_listener::
    ~set_fraction_listener() OPENVRML_NOTHROW
    {}

    /**
     * @brief Process event.
     *
     * @param fraction  fraction.
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    orientation_interpolator_node::set_fraction_listener::
    do_process_event(const openvrml::sffloat & fraction, const double timestamp)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            using std::vector;
            using openvrml::rotation;
            using openvrml::make_rotation;
            using openvrml::make_vec3f;

            orientation_interpolator_node & node =
                dynamic_cast<orientation_interpolator_node &>(this->node());

            const vector<float> & key = node.key_.mffloat::value();
            const vector<rotation> & key_value =
                node.key_value_.mfrotation::value();

            size_t n = key.size() - 1;
            if (fraction.value() < key[0]) {
                node.value_changed_.value(key_value[0]);
            } else if (fraction.value() > key[n]) {
                node.value_changed_.value(key_value[n]);
            } else {
                for (size_t i = 0; i < n; ++i) {
                    if (key[i] <= fraction.value()
                        && fraction.value() <= key[i + 1]) {
                        using openvrml::local::pi;

                        const rotation & v1 = key_value[i];
                        const rotation & v2 = key_value[i + 1];

                        // Interpolation factor
                        const float f =
                            (fraction.value() - key[i])
                            / (key[i + 1] - key[i]);

                        float x, y, z, r1, r2;
                        r1 = v1[3];

                        // Make sure the vectors are not pointing opposite ways
                        if (v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2] < 0.0) {
                            x = v1[0] + f * (-v2[0] - v1[0]);
                            y = v1[1] + f * (-v2[1] - v1[1]);
                            z = v1[2] + f * (-v2[2] - v1[2]);
                            r2 = -v2[3];
                        } else {
                            x = v1[0] + f * (v2[0] - v1[0]);
                            y = v1[1] + f * (v2[1] - v1[1]);
                            z = v1[2] + f * (v2[2] - v1[2]);
                            r2 = v2[3];
                        }

                        // Interpolate angles via the shortest direction
                        if (fabs(r2 - r1) > pi) {
                            if (r2 > r1) {
                                r1 += float(2.0 * pi);
                            } else {
                                r2 += float(2.0 * pi);
                            }
                        }
                        float angle = r1 + f * (r2 - r1);
                        if (angle >= 2.0 * pi) {
                            angle -= float(2.0 * pi);
                        } else if (angle < 0.0) {
                            angle += float(2.0 * pi);
                        }
                        const rotation value =
                            make_rotation(make_vec3f(x, y, z).normalize(),
                                          angle);
                        node.value_changed_.value(value);
                        break;
                    }
                }
            }

            // Send the new value
            node::emit_event(node.value_changed_emitter_, timestamp);
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @var orientation_interpolator_node::set_fraction_listener orientation_interpolator_node::set_fraction_listener_
     *
     * @brief set_fraction eventIn handler.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<orientation_interpolator_node>::exposedfield<openvrml::mffloat> orientation_interpolator_node::key_
     *
     * @brief key exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<orientation_interpolator_node>::exposedfield<openvrml::mfrotation> orientation_interpolator_node::key_value_
     *
     * @brief keyValue exposedField.
     */

    /**
     * @var openvrml::sfrotation orientation_interpolator_node::value_changed_
     *
     * @brief value_changed eventOut value.
     */

    /**
     * @var openvrml::sfrotation_emitter orientation_interpolator_node::value_changed_emitter_
     *
     * @brief value_changed eventOut emitter.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node instance.
     * @param scope the scope to which the node belongs.
     */
    orientation_interpolator_node::
    orientation_interpolator_node(
        const openvrml::node_type & type,
        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<orientation_interpolator_node>(type, scope),
        child_node(type, scope),
        set_fraction_listener_(*this),
        key_(*this),
        key_value_(*this),
        value_changed_emitter_(*this, this->value_changed_)
    {}

    /**
     * @brief Destroy.
     */
    orientation_interpolator_node::
    ~orientation_interpolator_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::orientation_interpolator_metatype::id =
    "urn:X-openvrml:node:OrientationInterpolator";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this class object.
 */
openvrml_node_vrml97::orientation_interpolator_metatype::
orientation_interpolator_metatype(openvrml::browser & browser):
    node_metatype(orientation_interpolator_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::orientation_interpolator_metatype::
~orientation_interpolator_metatype() OPENVRML_NOTHROW
{}

# define ORIENTATION_INTERPOLATOR_INTERFACE_SEQ                         \
    ((eventin,      sffloat,    "set_fraction",  set_fraction_listener_)) \
    ((exposedfield, mffloat,    "key",           key_))                 \
    ((exposedfield, mfrotation, "keyValue",      key_value_))           \
    ((eventout,     sfrotation, "value_changed", value_changed_emitter_)) \
    ((exposedfield, sfnode,     "metadata",      metadata))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(
    openvrml_node_vrml97,
    orientation_interpolator_metatype,
    orientation_interpolator_node,
    ORIENTATION_INTERPOLATOR_INTERFACE_SEQ)
