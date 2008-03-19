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
# include "sound.h"

namespace {

    class OPENVRML_LOCAL sound_node :
        public openvrml::node_impl_util::abstract_node<sound_node>,
        public openvrml::child_node {

        friend class openvrml_node_vrml97::sound_metatype;

        exposedfield<openvrml::sfvec3f> direction_;
        exposedfield<openvrml::sffloat> intensity_;
        exposedfield<openvrml::sfvec3f> location_;
        exposedfield<openvrml::sffloat> max_back_;
        exposedfield<openvrml::sffloat> max_front_;
        exposedfield<openvrml::sffloat> min_back_;
        exposedfield<openvrml::sffloat> min_front_;
        exposedfield<openvrml::sffloat> priority_;
        exposedfield<openvrml::sfnode> source_;
        openvrml::sfbool spatialize_;

    public:
        sound_node(const openvrml::node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~sound_node() OPENVRML_NOTHROW;

    private:
        virtual void do_render_child(openvrml::viewer & viewer,
                                     openvrml::rendering_context context);
    };

    /**
     * @class sound_node
     *
     * @brief Represents Sound node instances.
     */

    /**
     * @var class sound_node::sound_metatype
     *
     * @brief Class object for Sound instances.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<sound_node>::exposedfield<openvrml::sfvec3f> sound_node::direction_
     *
     * @brief direction exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<sound_node>::exposedfield<openvrml::sffloat> sound_node::intensity_
     *
     * @brief intensity exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<sound_node>::exposedfield<openvrml::sfvec3f> sound_node::location_
     *
     * @brief location exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<sound_node>::exposedfield<openvrml::sffloat> sound_node::max_back_
     *
     * @brief maxBack exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<sound_node>::exposedfield<openvrml::sffloat> sound_node::max_front_
     *
     * @brief maxFront exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<sound_node>::exposedfield<openvrml::sffloat> sound_node::min_back_
     *
     * @brief minBack exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<sound_node>::exposedfield<openvrml::sffloat> sound_node::min_front_
     *
     * @brief minFront exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<sound_node>::exposedfield<openvrml::sffloat> sound_node::priority_
     *
     * @brief priority exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<sound_node>::exposedfield<openvrml::sfnode> sound_node::source_
     *
     * @brief source exposedField.
     */

    /**
     * @var openvrml::sfbool sound_node::spatialize_
     *
     * @brief spatialize field.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the instance.
     * @param scope the scope associated with the instance.
     */
    sound_node::
    sound_node(const openvrml::node_type & type,
               const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<sound_node>(type, scope),
        child_node(type, scope),
        direction_(*this, openvrml::make_vec3f(0, 0, 1)),
        intensity_(*this, 1.0f),
        location_(*this),
        max_back_(*this, 10.0f),
        max_front_(*this, 10.0f),
        min_back_(*this, 1.0f),
        min_front_(*this, 1.0f),
        priority_(*this),
        source_(*this),
        spatialize_(true)
    {}

    /**
     * @brief Destroy.
     */
    sound_node::~sound_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Render the node.
     *
     * @param viewer    a Viewer.
     * @param context   a rendering context.
     */
    void
    sound_node::
    do_render_child(openvrml::viewer & /* viewer */,
                    const openvrml::rendering_context /* context */)
    {
        // If this clip has been modified, update the internal data
        if (this->source_.sfnode::value()
            && this->source_.sfnode::value()->modified()) {
//        this->source.value->render(viewer, context);
        }
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::sound_metatype::id =
    "urn:X-openvrml:node:Sound";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype.
 */
openvrml_node_vrml97::sound_metatype::
sound_metatype(openvrml::browser & browser):
    node_metatype(sound_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::sound_metatype::~sound_metatype() OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating Sound nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by sound_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::sound_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 11> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "direction"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "intensity"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "location"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "maxBack"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "maxFront"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "minBack"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "minFront"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "priority"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "source"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "spatialize"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata")
    };

    typedef node_impl_util::node_type_impl<sound_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & soundNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface == *++supported_interface) {
            soundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<sound_node>::exposedfield<sfvec3f> >(
                        &sound_node::direction_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<sound_node>::exposedfield<sfvec3f> >(
                        &sound_node::direction_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<sound_node>::exposedfield<sfvec3f> >(
                        &sound_node::direction_)));
        } else if (*interface == *++supported_interface) {
            soundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<sound_node>::exposedfield<sffloat> >(
                        &sound_node::intensity_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<sound_node>::exposedfield<sffloat> >(
                        &sound_node::intensity_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<sound_node>::exposedfield<sffloat> >(
                        &sound_node::intensity_)));
        } else if (*interface == *++supported_interface) {
            soundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<sound_node>::exposedfield<sfvec3f> >(
                        &sound_node::location_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<sound_node>::exposedfield<sfvec3f> >(
                        &sound_node::location_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<sound_node>::exposedfield<sfvec3f> >(
                        &sound_node::location_)));
        } else if (*interface == *++supported_interface) {
            soundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<sound_node>::exposedfield<sffloat> >(
                        &sound_node::max_back_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<sound_node>::exposedfield<sffloat> >(
                        &sound_node::max_back_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<sound_node>::exposedfield<sffloat> >(
                        &sound_node::max_back_)));
        } else if (*interface == *++supported_interface) {
            soundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<sound_node>::exposedfield<sffloat> >(
                        &sound_node::max_front_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<sound_node>::exposedfield<sffloat> >(
                        &sound_node::max_front_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<sound_node>::exposedfield<sffloat> >(
                        &sound_node::max_front_)));
        } else if (*interface == *++supported_interface) {
            soundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<sound_node>::exposedfield<sffloat> >(
                        &sound_node::min_back_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<sound_node>::exposedfield<sffloat> >(
                        &sound_node::min_back_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<sound_node>::exposedfield<sffloat> >(
                        &sound_node::min_back_)));
        } else if (*interface == *++supported_interface) {
            soundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<sound_node>::exposedfield<sffloat> >(
                        &sound_node::min_front_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<sound_node>::exposedfield<sffloat> >(
                        &sound_node::min_front_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<sound_node>::exposedfield<sffloat> >(
                        &sound_node::min_front_)));
        } else if (*interface == *++supported_interface) {
            soundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<sound_node>::exposedfield<sffloat> >(
                        &sound_node::priority_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<sound_node>::exposedfield<sffloat> >(
                        &sound_node::priority_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<sound_node>::exposedfield<sffloat> >(
                        &sound_node::priority_)));
        } else if (*interface == *++supported_interface) {
            soundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<sound_node>::exposedfield<sfnode> >(
                        &sound_node::source_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<sound_node>::exposedfield<sfnode> >(
                        &sound_node::source_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<sound_node>::exposedfield<sfnode> >(
                        &sound_node::source_)));
        } else if (*interface == *++supported_interface) {
            soundNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &sound_node::spatialize_)));
        } else if (*interface == *++supported_interface) {
            soundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<sound_node>::exposedfield<sfnode> >(
                        &sound_node::metadata)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<sound_node>::exposedfield<sfnode> >(
                        &sound_node::metadata)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<sound_node>::exposedfield<sfnode> >(
                        &sound_node::metadata)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}
