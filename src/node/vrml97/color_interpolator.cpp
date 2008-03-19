// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007  Braden McDaniel
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
# include "color_interpolator.h"

namespace {

    class OPENVRML_LOCAL color_interpolator_node :
        public openvrml::node_impl_util::abstract_node<color_interpolator_node>,
        public openvrml::child_node {

        friend class openvrml_node_vrml97::color_interpolator_metatype;

        class set_fraction_listener :
            public openvrml::node_impl_util::event_listener_base<self_t>,
            public sffloat_listener {
        public:
            explicit set_fraction_listener(color_interpolator_node & node);
            virtual ~set_fraction_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::sffloat & value,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_fraction_listener set_fraction_listener_;
        exposedfield<openvrml::mffloat> key_;
        exposedfield<openvrml::mfcolor> key_value_;
        openvrml::sfcolor value_;
        sfcolor_emitter value_changed_;

    public:
        color_interpolator_node(
            const openvrml::node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~color_interpolator_node() OPENVRML_NOTHROW;
    };

    /**
     * @class color_interpolator_node
     *
     * @brief ColorInterpolator node instances.
     */

    /**
     * @var class color_interpolator_node::color_interpolator_metatype
     *
     * @brief Class object for ColorInterpolator nodes.
     */

    /**
     * @internal
     *
     * @class color_interpolator_node::set_fraction_listener
     *
     * @brief set_fraction event handler.
     */

    /**
     * @brief Construct.
     *
     * @param node color_interpolator_node.
     */
    color_interpolator_node::set_fraction_listener::
    set_fraction_listener(color_interpolator_node & node):
        openvrml::node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<color_interpolator_node>(node),
        sffloat_listener(node)
    {}

    /**
     * @brief Destroy.
     */
    color_interpolator_node::set_fraction_listener::
    ~set_fraction_listener()
        OPENVRML_NOTHROW
    {}

    /**
     * @brief Process event.
     *
     * @param value     new fraction value.
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    color_interpolator_node::set_fraction_listener::
    do_process_event(const openvrml::sffloat & value, const double timestamp)
        OPENVRML_THROW1(std::bad_alloc)
    {
        using std::vector;

        try {
            color_interpolator_node & node =
                dynamic_cast<color_interpolator_node &>(this->node());

            float fraction = value.value();

            vector<float>::size_type n = node.key_.mffloat::value().size() - 1;
            if (fraction < node.key_.mffloat::value()[0]) {
                node.value_.value(node.key_value_.mfcolor::value()[0]);
            } else if (fraction > node.key_.mffloat::value()[n]) {
                node.value_.value(node.key_value_.mfcolor::value()[n]);
            } else {
                // convert to HSV for the interpolation...
                for (vector<float>::size_type i = 0; i < n; ++i) {
                    if (node.key_.mffloat::value()[i] <= value.value()
                        && fraction <= node.key_.mffloat::value()[i + 1]) {
                        using openvrml::color;
                        const color & rgb1 =
                            node.key_value_.mfcolor::value()[i];
                        const color & rgb2 =
                            node.key_value_.mfcolor::value()[i + 1];

                        fraction = (fraction - node.key_.mffloat::value()[i])
                            / (node.key_.mffloat::value()[i + 1]
                               - node.key_.mffloat::value()[i]);
                        float hsv1[3], hsv2[3];
                        rgb1.hsv(hsv1);
                        rgb2.hsv(hsv2);

                        // Interpolate angles via the shortest direction
                        if (fabs(hsv2[0] - hsv1[0]) > 180.0) {
                            if (hsv2[0] > hsv1[0]) {
                                hsv1[0] += 360.0;
                            } else {
                                hsv2[0] += 360.0;
                            }
                        }
                        float h = hsv1[0] + fraction * (hsv2[0] - hsv1[0]);
                        float s = hsv1[1] + fraction * (hsv2[1] - hsv1[1]);
                        float v = hsv1[2] + fraction * (hsv2[2] - hsv1[2]);
                        if (h >= 360.0) {
                            h -= 360.0;
                        } else if (h < 0.0) {
                            h += 360.0;
                        }
                        color val = node.value_.value();
                        val.hsv(h, s, v);
                        node.value_.value(val);
                        break;
                    }
                }
            }
            node.emit_event(node.value_changed_, timestamp);
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @var color_interpolator_node::set_fraction_listener color_interpolator_node::set_fraction_listener_;
     *
     * @brief set_fraction event handler.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<color_interpolator_node>::exposedfield<openvrml::mffloat> color_interpolator_node::key_
     *
     * @brief key exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<color_interpolator_node>::exposedfield<openvrml::mfcolor> color_interpolator_node::key_value_
     *
     * @brief keyValue exposedField.
     */

    /**
     * @var openvrml::sfcolor color_interpolator_node::value_
     *
     * @brief Current sfcolor value.
     */

    /**
     * @var openvrml::sfcolor_emitter color_interpolator_node::value_changed_
     *
     * @brief value_changed eventOut.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node instance.
     * @param scope the scope to which the node belongs.
     */
    color_interpolator_node::
    color_interpolator_node(const openvrml::node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<color_interpolator_node>(type, scope),
        child_node(type, scope),
        set_fraction_listener_(*this),
        key_(*this),
        key_value_(*this),
        value_changed_(*this, this->value_)
    {}

    /**
     * @brief Destroy.
     */
    color_interpolator_node::~color_interpolator_node()
        OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::color_interpolator_metatype::id =
    "urn:X-openvrml:node:ColorInterpolator";

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml_node_vrml97::color_interpolator_metatype::
color_interpolator_metatype(openvrml::browser & browser):
    node_metatype(color_interpolator_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::color_interpolator_metatype::
~color_interpolator_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating ColorInterpolator nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by
 *                                  color_interpolator_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::color_interpolator_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 5> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::eventin_id,
                       field_value::sffloat_id,
                       "set_fraction"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "key"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfcolor_id,
                       "keyValue"),
        node_interface(node_interface::eventout_id,
                       field_value::sfcolor_id,
                       "value_changed"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata")
    };

    typedef node_impl_util::node_type_impl<color_interpolator_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & colorInterpolatorNodeType =
        static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface == *++supported_interface) {
            colorInterpolatorNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    color_interpolator_node::set_fraction_listener>(
                        &color_interpolator_node::set_fraction_listener_)));
        } else if (*interface == *++supported_interface) {
            colorInterpolatorNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<color_interpolator_node>::
                    exposedfield<mffloat> >(
                        &color_interpolator_node::key_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<color_interpolator_node>::
                        exposedfield<mffloat> >(
                            &color_interpolator_node::key_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<color_interpolator_node>::
                        exposedfield<mffloat> >(
                            &color_interpolator_node::key_)));
        } else if (*interface == *++supported_interface) {
            colorInterpolatorNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<color_interpolator_node>::
                    exposedfield<mfcolor> >(
                        &color_interpolator_node::key_value_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<color_interpolator_node>::
                        exposedfield<mfcolor> >(
                            &color_interpolator_node::key_value_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<color_interpolator_node>::
                        exposedfield<mfcolor> >(
                            &color_interpolator_node::key_value_)));
        } else if (*interface == *++supported_interface) {
            colorInterpolatorNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<color_interpolator_node>::sfcolor_emitter>(
                        &color_interpolator_node::value_changed_)));
        } else if (*interface == *++supported_interface) {
            colorInterpolatorNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<color_interpolator_node>::exposedfield<sfnode> >(
                        &color_interpolator_node::metadata)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<color_interpolator_node>::exposedfield<sfnode> >(
                        &color_interpolator_node::metadata)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<color_interpolator_node>::exposedfield<sfnode> >(
                        &color_interpolator_node::metadata)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}
