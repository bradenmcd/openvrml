// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004  Braden McDaniel
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include "private.h"
# include "script.h"
# include "scope.h"
# include "browser.h"
# include "ScriptJDK.h"

namespace openvrml {

/**
 * @class script
 *
 * @brief Abstract class implemented by scripting language bindings.
 *
 * The runtime instantiates subclasses of script for each VRML97 Script node;
 * and calls its methods appropriately to execute script code.
 */

/**
 * @var script_node & script::node
 *
 * @brief A reference to the script_node that uses this script object.
 */

/**
 * @brief Construct.
 *
 * @param node  a reference to the script_node that uses this script object.
 */
script::script(script_node & node):
    node(node)
{}

/**
 * @brief Destroy.
 */
script::~script()
{}

/**
 * @brief Initialize the Script node.
 *
 * Delegates to <code>script::do_initialize</code>.
 *
 * @param timestamp the current time.
 */
void script::initialize(double timestamp)
{
    this->do_initialize(timestamp);
    this->process_direct_output(timestamp);
}

/**
 * @fn void script::do_initialize(double timestamp)
 *
 * @brief Initialize the Script node.
 *
 * @param timestamp the current time.
 */

/**
 * @brief Process an event.
 *
 * Delegates to <code>script::do_process_event</script>.
 *
 * @param id        eventIn identifier.
 * @param value     event value.
 * @param timestamp the current time.
 */
void script::process_event(const std::string & id,
                           const field_value & value,
                           double timestamp)
{
    this->do_process_event(id, value, timestamp);
    this->process_direct_output(timestamp);
}

/**
 * @fn void script::do_process_event(const std::string & id, const field_value & value, double timestamp)
 *
 * @brief Process an event.
 *
 * @param id        eventIn identifier.
 * @param value     event value.
 * @param timestamp the current time.
 */

/**
 * @brief Execute script code after processing events.
 *
 * Delegates to <code>script::do_events_processed</code>.
 *
 * @param timestamp the current time.
 */
void script::events_processed(double timestamp)
{
    this->do_events_processed(timestamp);
}

/**
 * @fn void script::do_events_processed(double timestamp)
 *
 * @brief Execute script code after processing events.
 *
 * @param timestamp the current time.
 */

/**
 * @brief Shut down the Script node.
 *
 * Delegates to <code>script::do_shutdown</code>.
 *
 * @param timestamp the current time.
 */
void script::shutdown(double timestamp)
{
    this->do_shutdown(timestamp);
    this->process_direct_output(timestamp);
}

/**
 * @fn void script::do_shutdown(double timestamp)
 *
 * @brief Shut down the Script node.
 *
 * @param timestamp the current time.
 */

/**
 * @brief Whether direct output is enabled for the Script node.
 *
 * @return @c true if direct output is enabled for the Script node; @c false
 *         otherwise.
 */
bool script::direct_output() const throw ()
{
    return this->node.direct_output.value;
}

/**
 * @brief Whether the browser may delay sending input events to the script
 *        until its outputs are needed by the browser.
 *
 * @return @c true if the browser may delay sending input events to the script
 *         until its outputs are needed by the browser; @c false otherwise.
 */
bool script::must_evaluate() const throw ()
{
    return this->node.must_evaluate.value;
}

/**
 * @brief Set the value of a field.
 *
 * @param id    field identifier.
 * @param value new value.
 *
 * @exception unsupported_interface if the Script node has no field @p id.
 * @exception std::bad_cast         if @p value is the wrong type.
 * @exception std::bad_alloc        if memory allocation fails.
 */
void script::field(const std::string & id, const field_value & value)
    throw (unsupported_interface, std::bad_cast, std::bad_alloc)
{
    const script_node::field_value_map_t::iterator field =
        this->node.field_value_map_.find(id);
    if (field == this->node.field_value_map_.end()) {
        throw unsupported_interface(this->node.type,
                                    node_interface::field_id,
                                    id);
    }
    field->second->assign(value); // throws std::bad_cast, std::bad_alloc
}

/**
 * @brief Add an event for direct output processing at the end of script
 *        execution.
 *
 * @param listener  the <code>event_listener</code> to which the event should
 *                  be sent.
 * @param value     the value to send.
 *
 * @exception field_value_type_mismatch if @p listener is not the correct type
 *                                      to process events of @p value's type.
 * @exception std::bad_alloc            if memory allocation fails.
 */
void script::direct_output(event_listener & listener,
                           const boost::shared_ptr<field_value> & value)
    throw (field_value_type_mismatch, std::bad_alloc)
{
    assert(value);
    if (listener.type() != value->type()) {
        throw field_value_type_mismatch();
    }
    this->direct_output_map_[&listener] = value;
}

void script::process_direct_output(double timestamp)
{
    for (direct_output_map_t::const_iterator output =
             this->direct_output_map_.begin();
         output != this->direct_output_map_.end();
         ++output) {
        using boost::polymorphic_downcast;
        switch (output->first->type()) {
        case field_value::sfbool_id:
            polymorphic_downcast<sfbool_listener *>(output->first)
                ->process_event(*polymorphic_downcast<sfbool *>(
                    output->second.get()), timestamp);
            break;
        case field_value::sfcolor_id:
            polymorphic_downcast<sfcolor_listener *>(output->first)
                ->process_event(*polymorphic_downcast<sfcolor *>(
                    output->second.get()), timestamp);
            break;
        case field_value::sffloat_id:
            polymorphic_downcast<sffloat_listener *>(output->first)
                ->process_event(*polymorphic_downcast<sffloat *>(
                    output->second.get()), timestamp);
            break;
        case field_value::sfimage_id:
            polymorphic_downcast<sfimage_listener *>(output->first)
                ->process_event(*polymorphic_downcast<sfimage *>(
                    output->second.get()), timestamp);
            break;
        case field_value::sfint32_id:
            polymorphic_downcast<sfint32_listener *>(output->first)
                ->process_event(*polymorphic_downcast<sfint32 *>(
                    output->second.get()), timestamp);
            break;
        case field_value::sfnode_id:
            polymorphic_downcast<sfnode_listener *>(output->first)
                ->process_event(*polymorphic_downcast<sfnode *>(
                    output->second.get()), timestamp);
            break;
        case field_value::sfrotation_id:
            polymorphic_downcast<sfrotation_listener *>(output->first)
                ->process_event(*polymorphic_downcast<sfrotation *>(
                    output->second.get()), timestamp);
            break;
        case field_value::sfstring_id:
            polymorphic_downcast<sfstring_listener *>(output->first)
                ->process_event(*polymorphic_downcast<sfstring *>(
                    output->second.get()), timestamp);
            break;
        case field_value::sftime_id:
            polymorphic_downcast<sftime_listener *>(output->first)
                ->process_event(*polymorphic_downcast<sftime *>(
                    output->second.get()), timestamp);
            break;
        case field_value::sfvec2f_id:
            polymorphic_downcast<sfvec2f_listener *>(output->first)
                ->process_event(*polymorphic_downcast<sfvec2f *>(
                    output->second.get()), timestamp);
            break;
        case field_value::sfvec3f_id:
            polymorphic_downcast<sfvec3f_listener *>(output->first)
                ->process_event(*polymorphic_downcast<sfvec3f *>(
                    output->second.get()), timestamp);
            break;
        case field_value::mfcolor_id:
            polymorphic_downcast<mfcolor_listener *>(output->first)
                ->process_event(*polymorphic_downcast<mfcolor *>(
                    output->second.get()), timestamp);
            break;
        case field_value::mffloat_id:
            polymorphic_downcast<mffloat_listener *>(output->first)
                ->process_event(*polymorphic_downcast<mffloat *>(
                    output->second.get()), timestamp);
            break;
        case field_value::mfint32_id:
            polymorphic_downcast<mfint32_listener *>(output->first)
                ->process_event(*polymorphic_downcast<mfint32 *>(
                    output->second.get()), timestamp);
            break;
        case field_value::mfnode_id:
            polymorphic_downcast<mfnode_listener *>(output->first)
                ->process_event(*polymorphic_downcast<mfnode *>(
                    output->second.get()), timestamp);
            break;
        case field_value::mfrotation_id:
            polymorphic_downcast<mfrotation_listener *>(output->first)
                ->process_event(*polymorphic_downcast<mfrotation *>(
                    output->second.get()), timestamp);
            break;
        case field_value::mfstring_id:
            polymorphic_downcast<mfstring_listener *>(output->first)
                ->process_event(*polymorphic_downcast<mfstring *>(
                    output->second.get()), timestamp);
            break;
        case field_value::mftime_id:
            polymorphic_downcast<mftime_listener *>(output->first)
                ->process_event(*polymorphic_downcast<mftime *>(
                    output->second.get()), timestamp);
            break;
        case field_value::mfvec2f_id:
            polymorphic_downcast<mfvec2f_listener *>(output->first)
                ->process_event(*polymorphic_downcast<mfvec2f *>(
                    output->second.get()), timestamp);
            break;
        case field_value::mfvec3f_id:
            polymorphic_downcast<mfvec3f_listener *>(output->first)
                ->process_event(*polymorphic_downcast<mfvec3f *>(
                    output->second.get()), timestamp);
            break;
        }
    }
    this->direct_output_map_.clear();
}


/**
 * @class script_node_class
 *
 * @brief Class object for @link script_node script_nodes@endlink.
 *
 * There is one script_node_class per browser instance.
 *
 * @see browser::scriptNodeClass
 */

/**
 * @brief Construct.
 *
 * @param browser   the browser to be associated with the script_node_class.
 */
script_node_class::script_node_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
script_node_class::~script_node_class() throw ()
{}

/**
 * @brief Not implemented.
 *
 * This method is not implemented because the Script node implementation,
 * unlike other node implementations, cannot provide the implementation of
 * an @c EXTERNPROTO. It is an error to call this method.
 */
const node_type_ptr
script_node_class::do_create_type(const std::string &,
                                  const node_interface_set &) const
    throw ()
{
    assert(false);
    return node_type_ptr();
}


/**
 * @class script_node
 *
 * @brief Represents a VRML Script node.
 */

/**
 * @typedef script_node::field_value_map_t
 *
 * @brief A std::map that keys field values on their field name.
 */

/**
 * @class script_node::eventout
 *
 * @brief An event_emitter along with the emitted value.
 */

/**
 * @var script_node & script_node::eventout::node_
 *
 * @brief The containing script_node.
 */

/**
 * @var boost::scoped_ptr<field_value> script_node::eventout::value_
 *
 * @brief The value.
 */

/**
 * @var bool script_node::eventout::modified_
 *
 * @brief Flag to indicate whether @a value_ has been modified.
 */

/**
 * @var boost::scoped_ptr<openvrml::event_emitter> script_node::eventout::emitter_
 *
 * @brief Event emitter.
 */

/**
 * @brief Construct.
 *
 * @param type  field value type identifier.
 * @param node  script_node.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
script_node::eventout::eventout(const field_value::type_id type,
                                script_node & node)
    throw (std::bad_alloc):
    node_(node),
    value_(field_value::create(type)),
    modified_(false),
    emitter_(openvrml::event_emitter::create(*this->value_))
{}

/**
 * @brief The value that will be sent from the eventOut.
 *
 * @return the value that will be sent from the eventOut.
 */
const field_value & script_node::eventout::value() const throw ()
{
    return *this->value_;
}

/**
 * @brief Set the value that will be sent from the eventOut.
 *
 * After calling this function, modified will return @c true until emit_event
 * is called.
 *
 * @param val   field value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 * @exception std::bad_cast     if @p val is not the correct type.
 */
void script_node::eventout::value(const field_value & val)
    throw (std::bad_alloc, std::bad_cast)
{
    if (this->value_->type() == field_value::sfnode_id) {
        this->node_
            .assign_with_self_ref_check(dynamic_cast<const sfnode &>(val),
                                        static_cast<sfnode &>(*this->value_));
    } else if (this->value_->type() == field_value::mfnode_id) {
        this->node_
            .assign_with_self_ref_check(dynamic_cast<const mfnode &>(val),
                                        static_cast<mfnode &>(*this->value_));
    } else {
        this->value_->assign(val); // Throws std::bad_alloc, std::bad_cast.
    }
    this->modified_ = true;
}

/**
 * @brief Whether the value has been modified.
 *
 * @return @c true if the value has been changed since emit_event was last
 *         called; @c false otherwise.
 */
bool script_node::eventout::modified() const throw ()
{
    return this->modified_;
}

/**
 * @brief The event_emitter associated with the eventout.
 *
 * @return the event_emitter associated with the eventout.
 */
event_emitter & script_node::eventout::emitter() throw ()
{
    return *this->emitter_;
}

/**
 * @brief Cause the contained event_emitter to emit an event.
 *
 * Events should be emitted from Script nodes by calling this function instead
 * of passing the event_emitter directly to node::emit_event.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void script_node::eventout::emit_event(const double timestamp)
    throw (std::bad_alloc)
{
    node::emit_event(*this->emitter_, timestamp);
    this->modified_ = false;
}

/**
 * @typedef script_node::eventout_ptr
 *
 * @brief Reference-counted smart pointer to an eventout.
 */

/**
 * @typedef script_node::eventout_map_t
 *
 * @brief Map of eventout instances.
 */

/**
 * @internal
 *
 * @class script_node::script_node_type
 *
 * @brief Type objects for @link script_node script_nodes@endlink.
 *
 * @see script_node::type
 */

/**
 * @var node_interface_set script_node::script_node_type::interfaces_
 *
 * @brief Node interfaces.
 */

namespace {

    //
    // The order of the node_interfaces in this array is significant. This
    // range is used with std::set_difference, so the elements *must* be in
    // lexicographically increasing order according to their "id" member.
    //
    const node_interface built_in_script_interfaces_[] = {
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "directOutput"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "mustEvaluate"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "url")
    };
}

/**
 * @brief Construct.
 *
 * @param class_    the node_class for @link script_node script_nodes@endlink.
 */
script_node::script_node_type::script_node_type(script_node_class & class_):
    node_type(class_, "Script")
{
    for (size_t i = 0; i < 3; ++i) {
        bool succeeded =
            this->interfaces_.insert(built_in_script_interfaces_[i]).second;
        assert(succeeded);
    }
}

/**
 * @brief Destroy.
 */
script_node::script_node_type::~script_node_type() throw ()
{}

/**
 * @brief Add an interface.
 *
 * @param interface
 *
 * @exception std::bad_alloc        if memory allocation fails.
 * @exception std::invalid_argument if the script_node_type already has an
 *                                  interface that conflicts with @p interface.
 */
void
script_node::script_node_type::add_interface(const node_interface & interface)
    throw (std::bad_alloc, std::invalid_argument)
{
    bool succeeded  = this->interfaces_.insert(interface).second;
    if (!succeeded) {
        throw std::invalid_argument("Interface conflicts with an interface "
                                    "already in this set.");
    }
}

/**
 * @brief Get the interfaces for the node.
 *
 * @return the interfaces for the node.
 */
const node_interface_set & script_node::script_node_type::do_interfaces() const
    throw ()
{
    return this->interfaces_;
}

/**
 * @brief Clone the Script node that has this node_type.
 *
 * Since the node_type for a script_node is only available once a script_node
 * is instantiated, you have to have a script_node instance in order to be able
 * to use this method. The "primordial" script_node instance must be created
 * with a call to the script_node constructor.
 *
 * @param scope             the scope to which the node should belong.
 * @param initial_values    a map of initial values for the node's fields and
 *                          exposedFields.
 *
 * @exception unsupported_interface if @p initial_values specifies a field
 *                                  name that is not supported by the node
 *                                  type.
 * @exception std::bad_cast         if a value in @p initial_values is the
 *                                  wrong type.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const node_ptr
script_node::script_node_type::
do_create_node(const boost::shared_ptr<openvrml::scope> & scope,
               const initial_value_map & initial_values) const
    throw (unsupported_interface, std::bad_cast, std::bad_alloc)
{
    using std::insert_iterator;
    using std::set_difference;

    //
    // The const_cast here is a bit messy; the alternative would be to make
    // script_node_type a friend of node_type. That's not straightforward
    // since script_node_type is an inner class of script_node.
    //
    script_node_class & scriptNodeClass =
        const_cast<script_node_class &>(
            static_cast<const script_node_class &>(this->node_class()));

    //
    // First, we need the set of node_interfaces *without* the built-in
    // Script node interfaces (url, directOutput, mustEvaluate).
    //
    node_interface_set interfaces;
    insert_iterator<node_interface_set> interface_inserter(interfaces,
                                                           interfaces.begin());
    set_difference(this->interfaces_.begin(),
                   this->interfaces_.end(),
                   built_in_script_interfaces_,
                   built_in_script_interfaces_ + 3,
                   interface_inserter,
                   node_interface_set::key_compare());

    node_ptr node;
    try {
        node.reset(new script_node(scriptNodeClass,
                                   scope,
                                   interfaces,
                                   initial_values));
    } catch (std::invalid_argument & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        assert(false);
    }
    return node;
}

/**
 * @internal
 *
 * @class script_node::script_event_listener
 *
 * @brief Event listener.
 */

/**
 * @var const std::string script_node::script_event_listener::id
 *
 * @brief eventIn identifier.
 */

/**
 * @fn script_node::script_event_listener::script_event_listener(const std::string & id, script_node & node)
 *
 * @brief Construct.
 *
 * @param id    eventIn identifier.
 * @param node  script_node.
 */

/**
 * @fn script_node::script_event_listener::~script_event_listener() throw ()
 *
 * @brief Destroy.
 */

/**
 * @fn script_node::script_event_listener::do_process_event(const FieldValue & value, double timestamp) throw (std::bad_alloc)
 *
 * @brief Process an event.
 *
 * @param value     event value.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @internal
 *
 * @typedef script_node::sfbool_listener
 *
 * @brief sfbool event listener.
 */

/**
 * @internal
 *
 * @typedef script_node::sfcolor_listener
 *
 * @brief sfcolor event listener.
 */

/**
 * @internal
 *
 * @typedef script_node::sffloat_listener
 *
 * @brief sffloat event listener.
 */

/**
 * @internal
 *
 * @typedef script_node::sfimage_listener
 *
 * @brief sfimage event listener.
 */

/**
 * @internal
 *
 * @typedef script_node::sfint32_listener
 *
 * @brief sfint32 event listener.
 */

/**
 * @internal
 *
 * @typedef script_node::sfnode_listener
 *
 * @brief sfnode event listener.
 */

/**
 * @internal
 *
 * @typedef script_node::sfrotation_listener
 *
 * @brief sfrotation event listener.
 */

/**
 * @internal
 *
 * @typedef script_node::sfstring_listener
 *
 * @brief sfstring event listener.
 */

/**
 * @internal
 *
 * @typedef script_node::sftime_listener
 *
 * @brief sftime event listener.
 */

/**
 * @internal
 *
 * @typedef script_node::sfvec2f_listener
 *
 * @brief sfvec2f event listener.
 */

/**
 * @internal
 *
 * @typedef script_node::sfvec3f_listener
 *
 * @brief sfvec3f event listener.
 */

/**
 * @internal
 *
 * @typedef script_node::mfcolor_listener
 *
 * @brief mfcolor event listener.
 */

/**
 * @internal
 *
 * @typedef script_node::mffloat_listener
 *
 * @brief mffloat event listener.
 */

/**
 * @internal
 *
 * @typedef script_node::mfint32_listener
 *
 * @brief mfint32 event listener.
 */

/**
 * @internal
 *
 * @typedef script_node::mfnode_listener
 *
 * @brief mfnode event listener.
 */

/**
 * @internal
 *
 * @typedef script_node::mfrotation_listener
 *
 * @brief mfrotation event listener.
 */

/**
 * @internal
 *
 * @typedef script_node::mfstring_listener
 *
 * @brief mfstring event listener.
 */

/**
 * @internal
 *
 * @typedef script_node::mftime_listener
 *
 * @brief mftime event listener.
 */

/**
 * @internal
 *
 * @typedef script_node::mfvec2f_listener
 *
 * @brief mfvec2f event listener.
 */

/**
 * @internal
 *
 * @typedef script_node::mfvec3f_listener
 *
 * @brief mfvec3f event listener.
 */

/**
 * @internal
 *
 * @brief Create a Script node event listener.
 *
 * @param type  the type of listener to create.
 * @param id    eventIn identifier.
 * @param node  the containing script_node.
 *
 * @return a Script node event listener.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
const boost::shared_ptr<event_listener>
script_node::create_listener(const field_value::type_id type,
                             const std::string & id,
                             script_node & node)
    throw (std::bad_alloc)
{
    boost::shared_ptr<openvrml::event_listener> listener;
    switch (type) {
    case field_value::sfbool_id:
        listener.reset(new sfbool_listener(id, node));
        break;
    case field_value::sfcolor_id:
        listener.reset(new sfcolor_listener(id, node));
        break;
    case field_value::sffloat_id:
        listener.reset(new sffloat_listener(id, node));
        break;
    case field_value::sfimage_id:
        listener.reset(new sfimage_listener(id, node));
        break;
    case field_value::sfint32_id:
        listener.reset(new sfint32_listener(id, node));
        break;
    case field_value::sfnode_id:
        listener.reset(new sfnode_listener(id, node));
        break;
    case field_value::sfstring_id:
        listener.reset(new sfstring_listener(id, node));
        break;
    case field_value::sfrotation_id:
        listener.reset(new sfrotation_listener(id, node));
        break;
    case field_value::sftime_id:
        listener.reset(new sftime_listener(id, node));
        break;
    case field_value::sfvec2f_id:
        listener.reset(new sfvec2f_listener(id, node));
        break;
    case field_value::sfvec3f_id:
        listener.reset(new sfvec3f_listener(id, node));
        break;
    case field_value::mfcolor_id:
        listener.reset(new mfcolor_listener(id, node));
        break;
    case field_value::mffloat_id:
        listener.reset(new mffloat_listener(id, node));
        break;
    case field_value::mfint32_id:
        listener.reset(new mfint32_listener(id, node));
        break;
    case field_value::mfnode_id:
        listener.reset(new mfnode_listener(id, node));
        break;
    case field_value::mfstring_id:
        listener.reset(new mfstring_listener(id, node));
        break;
    case field_value::mfrotation_id:
        listener.reset(new mfrotation_listener(id, node));
        break;
    case field_value::mftime_id:
        listener.reset(new mftime_listener(id, node));
        break;
    case field_value::mfvec2f_id:
        listener.reset(new mfvec2f_listener(id, node));
        break;
    case field_value::mfvec3f_id:
        listener.reset(new mfvec3f_listener(id, node));
        break;
    default:
        assert(false);
    }
    return listener;
}

/**
 * @internal
 *
 * @class script_node::set_url_listener_t
 *
 * @brief set_url event listener.
 */

/**
 * @brief Construct.
 *
 * @param node  a reference to the containing script_node.
 */
script_node::set_url_listener_t::set_url_listener_t(script_node & node):
    openvrml::mfstring_listener(node)
{}

/**
 * @brief Destroy.
 */
script_node::set_url_listener_t::~set_url_listener_t() throw ()
{}

/**
 * @brief Process an event.
 *
 * @param value     new url value.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void script_node::set_url_listener_t::do_process_event(const mfstring & value,
                                                       const double timestamp)
    throw (std::bad_alloc)
{
    assert(dynamic_cast<openvrml::script_node *>(&this->node()));
    openvrml::script_node & script_node =
        dynamic_cast<openvrml::script_node &>(this->node());
    delete script_node.script_;
    script_node.script_ = 0;
    script_node.url_ = value;
    script_node.do_initialize(timestamp);

    //
    // url is an exposedField.
    //
    node::emit_event(script_node.url_changed_emitter, timestamp);
}

/**
 * @internal
 *
 * @var script_node::script_node_type script_node::type
 *
 * @brief Type object for the script_node instance.
 *
 * Script node @link openvrml::node_type node_types@endlink are significantly
 * different from other @link openvrml::node_type node_types@endlink. While
 * most @link openvrml::node_type node_types@endlink are shared by the node
 * instances they spawn, the script_node_type is unique to a script_node
 * instance, and it shares the script_node's lifetime. This reflects the fact
 * that Script nodes in VRML get their functionality by the addition of fields,
 * eventIns, and eventOuts on a per-instance basis.
 *
 * For script_node instances, node::node_type is an alias for the
 * script_node_type object.
 */

/**
 * @internal
 *
 * @var sfbool script_node::direct_output
 *
 * @brief directOutput field.
 */

/**
 * @internal
 *
 * @var sfbool script_node::must_evaluate
 *
 * @brief mustEvaluate field.
 */

/**
 * @internal
 *
 * @var script_node::set_url_listener_t script_node::set_url_listener
 *
 * @brief set_url eventIn handler.
 */

/**
 * @internal
 *
 * @var mfstring script_node::url_
 *
 * @brief url exposedField.
 */

/**
 * @internal
 *
 * @var mfstring_emitter script_node::url_changed_emitter
 *
 * @brief url_changed eventOut emitter.
 */

/**
 * @internal
 *
 * @var script_node::field_value_map_t script_node::field_value_map_
 *
 * @brief Maps user-defined field names to their values.
 */

/**
 * @internal
 *
 * @typedef script_node::event_listener_ptr
 *
 * @brief Reference-counted smart pointer to an event_listener.
 */

/**
 * @internal
 *
 * @typedef script_node::event_listener_map_t
 *
 * @brief Map of event listeners.
 */

/**
 * @internal
 *
 * @var script_node::event_listener_map_t script_node::event_listener_map
 *
 * @brief Map of event listeners.
 */

/**
 * @internal
 *
 * @var script_node::eventout_map_t script_node::eventout_map_
 *
 * @brief Map of eventout instances.
 */

/**
 * @internal
 *
 * @var script * script_node::script_
 *
 * @brief A pointer to a script object.
 */

/**
 * @internal
 *
 * @var int script_node::events_received
 *
 * @brief A count of the number of events received since script_node::update
 *      was called.
 */

/**
 * @brief Construct.
 *
 * Unlike other concrete node types, which are always instantiated via
 * <code>node_type::create_node</code>, the <code>script_node</code>
 * constructor is called directly when creating a new <code>script_node</code>
 * from scratch.  However, a <code>script_node</code> can be duplicated (or
 * "cloned") by calling <code>node_type::create_node</code> on
 * <code>script_node::type</code> of a <code>script_node</code> instance.  This
 * provides a consistent interface for cloning any node, regardless of its
 * type.  OpenVRML uses this internally when instantiating <code>PROTO</code>s.
 *
 * @param class_            the <code>script_node_class</code>.  Typically
 *                          there is one <code>script_node_class</code> per
 *                          browser instance.
 * @param scope             the <code>scope</code> to which the node should
 *                          belong.
 * @param interfaces        a <code>node_interface_set</code> containing
 *                          specifications of user-defined fields, eventIns,
 *                          and eventOuts particular to the
 *                          <code>script_node</code> instance.
 * @param initial_values    a map of initial values for fields of the
 *                          <code>script_node</code>.
 *
 * @exception unsupported_interface if @p initial_values specifies a field that
 *                                  is not supported by the
 *                                  <code>script_node</code>.
 * @exception std::bad_cast         if @p initial_values includes a field value
 *                                  that is the wrong type for the specified
 *                                  field.
 * @exception std::bad_alloc        if memory allocation fails.
 * @exception std::invalid_argument if:
 *                                  - @p interfaces includes an exposedField
 *                                    specification.
 *                                  - @p interfaces includes an interface
 *                                    specification that duplicates an existing
 *                                    Script node interface.
 *                                  - @p initial_values is missing an initial
 *                                    value for a user-defined field in
 *                                    @p interfaces.
 */
script_node::script_node(script_node_class & class_,
                         const boost::shared_ptr<openvrml::scope> & scope,
                         const node_interface_set & interfaces,
                         const initial_value_map & initial_values)
    throw (unsupported_interface, std::bad_cast, std::bad_alloc,
           std::invalid_argument):
    node(this->type, scope),
    child_node(this->type, scope),
    type(class_),
    direct_output(false),
    must_evaluate(false),
    set_url_listener(*this),
    url_changed_emitter(this->url_),
    script_(0),
    events_received(0)
{
    for (node_interface_set::const_iterator interface = interfaces.begin();
         interface != interfaces.end();
         ++interface) {
        using std::invalid_argument;
        using std::auto_ptr;
        using std::pair;
        using std::string;
        using boost::shared_ptr;
        using boost::polymorphic_downcast;
        shared_ptr<openvrml::event_listener> listener;
        shared_ptr<eventout> eventout;
        initial_value_map::const_iterator initial_value;
        pair<string, field_value_ptr> field_value;
        auto_ptr<openvrml::field_value> cloned_field_value;
        bool succeeded;
        switch (interface->type) {
        case node_interface::eventin_id:
            listener = create_listener(interface->field_type,
                                       interface->id,
                                       *this);
            succeeded = this->event_listener_map
                .insert(make_pair(interface->id, listener)).second;
            break;
        case node_interface::eventout_id:
            eventout.reset(new script_node::eventout(interface->field_type,
                                                     *this));
            succeeded = this->eventout_map_
                .insert(make_pair(interface->id, eventout)).second;
            break;
        case node_interface::field_id:
            initial_value = initial_values.find(interface->id);
            if (initial_value == initial_values.end()) {
                throw invalid_argument("Missing initial value for field \""
                                       + interface->id + "\".");
            }
            cloned_field_value = initial_value->second->clone();
            field_value = make_pair(initial_value->first,
                                    field_value_ptr(cloned_field_value));
            //
            // Account for self-references.
            //
            // Replace instances of node_ptr::self with a real reference to
            // the current script_node.
            //
            // After making the assignment, we must decrement the node_ptr's
            // count because a Script node self-reference must not be an owning
            // pointer. See the assign_with_self_ref_check functions for more
            // information.
            //
            if (interface->field_type == openvrml::field_value::sfnode_id) {
                sfnode & sfnode_field_value =
                    *polymorphic_downcast<sfnode *>(field_value.second.get());
                if (sfnode_field_value.value == node_ptr::self) {
                    sfnode_field_value.value.reset(this);
                    assert(sfnode_field_value.value.count_ptr->second > 0);
                    --sfnode_field_value.value.count_ptr->second;
                }
            } else if (interface->field_type
                       == openvrml::field_value::mfnode_id) {
                mfnode & mfnode_field_value =
                    *polymorphic_downcast<mfnode *>(field_value.second.get());
                using std::vector;
                for (vector<node_ptr>::iterator node =
                         mfnode_field_value.value.begin();
                     node != mfnode_field_value.value.end();
                     ++node) {
                    if (*node == node_ptr::self) {
                        node->reset(this);
                        assert(node->count_ptr->second > 0);
                        --node->count_ptr->second;
                    }
                }
            }

            succeeded = this->field_value_map_.insert(field_value).second;
            break;
        case node_interface::exposedfield_id:
            throw invalid_argument("Script cannot support user-defined "
                                   "exposedFields.");
        default:
            assert(false);
        }
        assert(succeeded);
        this->type.add_interface(*interface); // Throws std::invalid_argument.
    }

    for (initial_value_map::const_iterator initial_value =
             initial_values.begin();
         initial_value != initial_values.end();
         ++initial_value) {
        if (initial_value->first == "url") {
            this->url_.assign(*initial_value->second);
        } else if (initial_value->first == "directOutput") {
            this->direct_output.assign(*initial_value->second);
        } else if (initial_value->first == "mustEvaluate") {
            this->must_evaluate.assign(*initial_value->second);
        }
    }
}

/**
 * @brief Destroy.
 */
script_node::~script_node() throw ()
{
    delete this->script_;
}

/**
 * @internal
 *
 * @brief Return a pointer to this script_node.
 *
 * @return a pointer to this script_node.
 */
script_node * script_node::to_script() throw ()
{
    return this;
}

/**
 * @brief Update the script_node for the current time.
 *
 * @param current_time  the current time.
 */
void script_node::update(const double current_time)
{
    if (this->events_received > 0) {
        this->events_received = 0;
        if (this->script_) {
            this->script_->events_processed(current_time);
        }
    }

    //
    // For each modified eventOut, send an event.
    //
    for (eventout_map_t::iterator eventout = this->eventout_map_.begin();
         eventout != this->eventout_map_.end();
         ++eventout) {
        if (eventout->second->modified()) {
            eventout->second->emit_event(current_time);
        }
    }
}

/**
 * @fn const script_node::field_value_map_t & script_node::field_value_map() const throw ()
 *
 * @brief Field value map accessor.
 *
 * @return the field value map.
 */

/**
 * @fn const script_node::eventout_map_t & script_node::eventout_map() const throw ()
 *
 * @brief eventOut map accessor.
 *
 * @return the eventOut map.
 */

/**
 * @internal
 *
 * @brief Special assignment function to take into account the fact that
 *        Script nodes can be self referential.
 *
 * "Undo" the refcounting appropriately: decrement the refcount on any
 * self-references we acquire ownership of, and increment the refcount on any
 * self-references for which ownership is relinquished.
 *
 * @param inval     input sfnode.
 * @retval retval   output sfnode.
 */
void script_node::assign_with_self_ref_check(const sfnode & inval,
                                             sfnode & retval) const
    throw ()
{
    const node_ptr & oldNode = retval.value;

    //
    // About to relinquish ownership of a sfnode value. If the
    // sfnode value is this Script node, then we need to
    // *increment* its refcount, since we previously
    // *decremented* it to accommodate creating a cycle between
    // refcounted objects.
    //
    if (oldNode
        && (dynamic_cast<script_node *>(oldNode.count_ptr->first) == this)) {
        ++oldNode.count_ptr->second;
    }

    retval = inval;

    //
    // Now, check to see if the new sfnode value is a self-
    // reference. If it is, we need to *decrement* the refcount.
    // A self-reference creates a cycle. If a Script node with
    // a self-reference were completely removed from the world,
    // it still wouldn't be deleted (if we didn't do this)
    // because the reference it held to itself would prevent the
    // refcount from ever dropping to zero.
    //
    const node_ptr & newNode = retval.value;
    if (dynamic_cast<script_node *>(newNode.count_ptr->first) == this) {
        --(newNode.count_ptr->second);
    }
}

/**
 * @internal
 *
 * @brief Special assignment function to take into account the fact that
 *        Script nodes can be self referential.
 *
 * "Undo" the refcounting appropriately: decrement the refcount on any
 * self-references we acquire ownership of, and increment the refcount on any
 * self-references for which ownership is relinquished.
 *
 * @param inval     input mfnode.
 * @retval retval   output mfnode.
 */
void script_node::assign_with_self_ref_check(const mfnode & inval,
                                             mfnode & retval) const
    throw ()
{
    std::vector<node_ptr>::size_type i;
    for (i = 0; i < retval.value.size(); ++i) {
        const node_ptr & oldNode = retval.value[i];
        if (oldNode
            && (dynamic_cast<script_node *>(oldNode.count_ptr->first)
                == this)) {
            ++oldNode.count_ptr->second;
        }
    }

    retval = inval;

    for (i = 0; i < retval.value.size(); ++i) {
        const node_ptr & newNode = retval.value[i];
        if (newNode
            && (dynamic_cast<script_node *>(newNode.count_ptr->first)
                == this)) {
            --(newNode.count_ptr->second);
        }
    }
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void script_node::do_initialize(const double timestamp) throw (std::bad_alloc)
{
    assert(this->scene());
    this->scene()->browser.add_script(*this);

    this->events_received = 0;
    this->script_ = this->create_script();
    if (this->script_) { this->script_->initialize(timestamp); }

    //
    // For each modified eventOut, send an event.
    //
    for (eventout_map_t::iterator eventout = this->eventout_map_.begin();
         eventout != this->eventout_map_.end();
         ++eventout) {
        if (eventout->second->modified()) {
            eventout->second->emit_event(timestamp);
        }
    }
}

/**
 * @brief Get the value of a field.
 *
 * @param id    the name of the field to get.
 *
 * @return the value for field @p id.
 *
 * @exception unsupported_interface  if the node has no field @p id.
 */
const field_value & script_node::do_field(const std::string & id) const
    throw (unsupported_interface)
{
    field_value_map_t::const_iterator itr;
    if (id == "url") {
        return this->url_;
    } else if (id == "directOutput") {
        return this->direct_output;
    } else if (id == "mustEvaluate") {
        return this->must_evaluate;
    } else if ((itr = this->field_value_map_.find(id))
            != this->field_value_map_.end()) {
        return *itr->second;
    }
    throw unsupported_interface(this->type, node_interface::field_id, id);
}

/**
 * @brief Get an event listener.
 *
 * This method is called by node::event_listener. Subclasses must implement
 * this method.
 *
 * @param id    eventIn identifier.
 *
 * @return the event listener.
 *
 * @exception unsupported_interface if the node has no eventIn @p id.
 */
event_listener & script_node::do_event_listener(const std::string & id)
    throw (unsupported_interface)
{
    if (id == "url" || id == "set_url") {
        return this->set_url_listener;
    } else {
        event_listener_map_t::iterator pos;
        const event_listener_map_t::iterator end =
            this->event_listener_map.end();
        if ((pos = this->event_listener_map.find(id)) != end) {
            return *pos->second;
        } else if ((pos = this->event_listener_map.find("set_" + id)) != end) {
            return *pos->second;
        }
    }
    throw unsupported_interface(this->type, node_interface::eventin_id, id);
}

/**
 * @brief Get an event emitter.
 *
 * This method is called by node::event_emitter.
 *
 * @param id    eventOut identifier.
 *
 * @return the event emitter.
 *
 * @exception unsupported_interface if the node has no eventOut @p id.
 */
event_emitter & script_node::do_event_emitter(const std::string & id)
    throw (unsupported_interface)
{
    openvrml::event_emitter * result = 0;
    if (id == "url" || id == "url_changed") {
        result = &this->url_changed_emitter;
    } else {
        eventout_map_t::iterator pos;
        const eventout_map_t::iterator end = this->eventout_map_.end();
        if ((pos = this->eventout_map_.find(id)) != end) {
            result = &pos->second->emitter();
        } else if ((pos = this->eventout_map_.find(id + "_changed")) != end) {
            result = &pos->second->emitter();
        } else {
            throw unsupported_interface(this->type,
                                        node_interface::eventout_id,
                                        id);
        }
    }
    assert(result);
    return *result;
}

/**
 * @fn const script_node::field_value_map_t & script_node::field_value_map() const throw ()
 *
 * @brief field value map.
 *
 * @return the field value map.
 */

/**
 * @fn const script_node::eventout_map_t & script_node::eventout_map() const throw ()
 *
 * @brief eventOut map.
 *
 * @return the eventOut map.
 */

/**
 * @brief Called by node::shutdown.
 *
 * @param timestamp the current time.
 */
void script_node::do_shutdown(const double timestamp) throw ()
{
    if (this->script_) { this->script_->shutdown(timestamp); }
    this->scene()->browser.remove_script(*this);
}

/**
 * @brief render_child implementation.
 *
 * @param v         viewer implementation responsible for actually doing the
 *                  drawing.
 * @param context   generic context argument; holds things like the accumulated
 *                  modelview transform.
 */
void script_node::do_render_child(viewer & v, rendering_context context)
{}

} // namespace openvrml

# if OPENVRML_ENABLE_SCRIPT_NODE_JAVASCRIPT
#   include <algorithm>
#   include <iostream>
#   include <memory>
#   include <sstream>
#   include <vector>
#   include <jsapi.h>
# endif
namespace openvrml {

# if OPENVRML_ENABLE_SCRIPT_NODE_JAVASCRIPT
namespace {

namespace js_ {

class SFNode;
class MFNode;

namespace Browser {
    JSBool addRoute(JSContext * cx, JSObject * obj,
                    uintN argc, jsval * argv, jsval * rval) throw ();
    JSBool deleteRoute(JSContext * cx, JSObject * obj,
                       uintN argc, jsval * argv, jsval * rval) throw ();
}

class script : public openvrml::script {

    friend class SFNode;
    friend class MFNode;
    friend JSBool Browser::addRoute(JSContext * cx, JSObject * obj,
                                    uintN argc, jsval * argv, jsval * rval)
        throw ();
    friend JSBool Browser::deleteRoute(JSContext * cx, JSObject * obj,
                                       uintN argc, jsval * argv, jsval * rval)
        throw ();

    static JSRuntime * rt;
    static size_t nInstances;

    double d_timeStamp;

    JSContext * cx;
    JSClass & sfnode_class;

public:
    script(openvrml::script_node & node, const std::string & source)
        throw (std::bad_alloc);
    virtual ~script();

    openvrml::script_node & script_node();

    using openvrml::script::direct_output;

    jsval vrmlFieldToJSVal(const openvrml::field_value & value) throw ();

private:
    static JSBool field_setProperty(JSContext * cx, JSObject * obj,
                                    jsval id, jsval * val)
        throw ();

    virtual void do_initialize(double timeStamp);
    virtual void do_process_event(const std::string & id,
                                  const openvrml::field_value & value,
                                  double timestamp);
    virtual void do_events_processed(double timeStamp);
    virtual void do_shutdown(double timeStamp);

    void initVrmlClasses() throw (std::bad_alloc);
    void defineBrowserObject() throw (std::bad_alloc);
    void defineFields() throw (std::bad_alloc);
    void activate(double timeStamp, const std::string & fname,
                  size_t argc, const openvrml::field_value * const argv[]);
};

} // namespace js_

} // namespace
# endif // OPENVRML_SCRIPT_NODE_JAVASCRIPT

/**
 * @internal
 *
 * @brief Create a script object.
 *
 * @return a new script object.
 */
script * script_node::create_script()
{
    // Try each url until we find one we like
    for (size_t i = 0; i < this->url_.value.size(); ++i) {
        if (this->url_.value[i].empty()) { continue; }

        // Get the protocol & mimetype...
# if OPENVRML_ENABLE_SCRIPT_NODE_JAVASCRIPT
        // Need to handle external .js files too...
        const char * javascriptScheme = "javascript:";
        const char * vrmlscriptScheme = "vrmlscript:";
        if (std::equal(javascriptScheme, javascriptScheme + 11,
                       this->url_.value[i].begin())
                || std::equal(vrmlscriptScheme, vrmlscriptScheme + 11,
                              this->url_.value[i].begin())) {
            return new js_::script(*this,
                                   this->url_.value[i].substr(11));
        }
# endif

#if OPENVRML_ENABLE_SCRIPT_NODE_JAVA
        const char javaExtension1[] = ".class";
        const char javaExtension2[] = ".CLASS";

        int slen = this->url_.value[i].length();

        if (slen > 6
                && (std::equal(javaExtension1, javaExtension1 + 6,
                               this->url_.value[i].end() - 6)
                    || std::equal(javaExtension2, javaExtension2 + 6,
                                  this->url_.value[i].end() - 6))) {
            doc2 base(this->type.node_class().browser().world_url());
            doc2 doc(this->url_.value[i], &base);
            if (doc.local_name()) {
                return new ScriptJDK(*this,
                                     doc.url_base().c_str(),
                                     doc.local_path());
            }
        }
#endif
    }

    return 0;
}

# if OPENVRML_ENABLE_SCRIPT_NODE_JAVASCRIPT
namespace {

namespace js_ {

const long MAX_HEAP_BYTES = 4L * 1024L * 1024L;
const long STACK_CHUNK_BYTES = 4024L;

# if 0
enum err_num {
# define MSG_DEF(name, number, count, exception, format) name = number,
# include "js.msg"
# undef MSG_DEF
    err_limit
};

JSErrorFormatString error_format_string[err_limit] = {
# if JS_HAS_DFLT_MSG_STRINGS
#   define MSG_DEF(name, number, count, exception, format) { format, count },
# else
#   define MSG_DEF(name, number, count, exception, format) { 0, count },
# endif
# include "js.msg"
# undef MSG_DEF
};

const JSErrorFormatString * get_error_message(void * user_ref,
                                              const char * locale,
                                              const uintN error_number)
{
    if (error_number > 0 && error_number < err_limit) {
        return &error_format_string[error_number];
    }
    return 0;
}
# endif

class bad_conversion : public std::runtime_error {
public:
    bad_conversion(const std::string & msg): runtime_error(msg) {}
    virtual ~bad_conversion() throw () {}
};

JSBool floatsToJSArray(size_t numFloats, const float * floats,
                       JSContext * cx, jsval * rval);
std::auto_ptr<field_value>
createFieldValueFromJsval(JSContext * cx, jsval val,
                          field_value::type_id fieldType)
    throw (bad_conversion, std::bad_alloc);

namespace Global {
    JSClass jsclass = {
        "global", 0,
        JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
        JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub
    };
    JSBool print(JSContext *, JSObject *, uintN, jsval *, jsval *);

}

namespace Browser {
    JSClass jsclass = {
        "Browser", 0,
        JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
        JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub
    };
    JSBool getName(JSContext * cx, JSObject * obj,
                   uintN argc, jsval * argv, jsval * rval) throw ();
    JSBool getVersion(JSContext * cx, JSObject * obj,
                      uintN argc, jsval * argv, jsval * rval) throw ();
    JSBool getCurrentSpeed(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval)
            throw ();
    JSBool getCurrentFrameRate(JSContext * cx, JSObject * obj,
                               uintN argc, jsval * argv, jsval * rval)
            throw ();
    JSBool getWorldURL(JSContext * cx, JSObject * obj,
                       uintN argc, jsval * argv, jsval * rval) throw ();
    JSBool replaceWorld(JSContext * cx, JSObject * obj,
                        uintN argc, jsval * argv, jsval * rval)
            throw ();
    JSBool createVrmlFromString(JSContext * cx, JSObject * obj,
                                uintN argc, jsval * argv, jsval * rval)
            throw ();
    JSBool createVrmlFromURL(JSContext * cx, JSObject * obj,
                             uintN argc, jsval * argv, jsval * rval)
            throw ();
    JSBool addRoute(JSContext * cx, JSObject * obj,
                    uintN argc, jsval * argv, jsval * rval) throw ();
    JSBool deleteRoute(JSContext * cx, JSObject * obj,
                       uintN argc, jsval * argv, jsval * rval) throw ();
    JSBool loadURL(JSContext * cx, JSObject * obj,
                   uintN argc, jsval * argv, jsval * rval) throw ();
    JSBool setDescription(JSContext * cx, JSObject * obj,
                          uintN argc, jsval * argv, jsval * rval)
            throw ();
}

//
// A base class for a field value "holder" that gets owned by the
// JavaScript objects corresponding to VRML data values. The holder
// includes essential metadata: the corresponding interface ID for
// the object, if any, and whether or not the object corresponds to
// an eventOut.
//
class field_data : boost::noncopyable {
public:
    bool changed;

    virtual ~field_data() = 0;

protected:
    field_data();
};

class sfield {
public:
    /**
     * The sfdata class takes ownership of the field_value and deletes it upon
     * destruction.
     */
    class sfdata : public field_data {
        openvrml::field_value * const field_value_;

    public:
        explicit sfdata(openvrml::field_value * value);
        virtual ~sfdata();

        openvrml::field_value & field_value() const;
    };

protected:
    static void finalize(JSContext * cx, JSObject * obj) throw ();
    static JSBool toString(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval) throw ();
private:
    sfield();
};

class SFColor : public sfield {
public:
    static JSClass jsclass;

    static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
    static JSBool toJsval(const color & sfcolor,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<openvrml::sfcolor>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (bad_conversion, std::bad_alloc);

private:
    static JSBool construct(JSContext * cx, JSObject * obj,
                            uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool initObject(JSContext * cx, JSObject * obj,
                             uintN argc, jsval * argv) throw ();
    static JSBool getProperty(JSContext * cx, JSObject * obj, jsval id,
                              jsval * vp) throw ();
    static JSBool setProperty(JSContext * cx, JSObject * obj, jsval id,
                              jsval * vp) throw ();
    static JSBool setHSV(JSContext * cx, JSObject * obj,
                         uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool getHSV(JSContext * cx, JSObject * obj,
                         uintN argc, jsval * argv, jsval * rval) throw ();
};

class SFImage : public sfield {
public:
    static JSClass jsclass;

    static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
    static JSBool toJsval(const openvrml::sfimage & sfcolor,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<openvrml::sfimage>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (bad_conversion, std::bad_alloc);

private:
    static JSBool construct(JSContext * cx, JSObject * obj,
	                    uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool initObject(JSContext * cx, JSObject * obj,
                             uintN argc, jsval * argv) throw ();
    static JSBool getProperty(JSContext * cx, JSObject * obj, jsval id,
                              jsval * vp) throw ();
    static JSBool setProperty(JSContext * cx, JSObject * obj, jsval id,
                              jsval * vp) throw ();
};

class SFNode : public sfield {
public:
    static JSClass jsclass;
    static JSClass direct_output_jsclass;

    static JSObject * initClass(JSContext * cx, JSObject * obj)
        throw ();
    static JSBool toJsval(const node_ptr & node,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<openvrml::sfnode>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (bad_conversion, std::bad_alloc);
    static JSBool construct(JSContext * cx, JSObject * obj,
                            uintN argc, jsval * argv, jsval *)
        throw ();
    static JSBool initObject(JSContext * cx, JSObject * obj,
                             uintN argc, jsval * argv)
        throw ();
    static JSBool getProperty(JSContext * cx, JSObject * obj,
                              jsval id, jsval * vp)
        throw ();
    static JSBool setProperty(JSContext * cx, JSObject * obj,
                              jsval id, jsval * vp)
        throw ();
};

class SFRotation : public sfield {
public:
    static JSClass jsclass;

    static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
    static JSBool toJsval(const rotation & rotation,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<openvrml::sfrotation>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (bad_conversion, std::bad_alloc);

private:
    static JSBool construct(JSContext * cx, JSObject * obj,
	                    uintN argc, jsval * argv, jsval *) throw ();
    static JSBool initObject(JSContext * cx, JSObject * obj,
                             uintN argc, jsval * argv) throw ();
    static JSBool getProperty(JSContext * cx, JSObject * obj, jsval id,
                              jsval * vp) throw ();
    static JSBool setProperty(JSContext * cx, JSObject * obj, jsval id,
                              jsval * vp) throw ();
    static JSBool getAxis(JSContext * cx, JSObject * obj,
                          uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool inverse(JSContext * cx, JSObject * obj,
                          uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool multiply(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool multVec(JSContext * cx, JSObject * obj,
                          uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool setAxis(JSContext * cx, JSObject * obj,
                          uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool slerp(JSContext * cx, JSObject * obj,
                        uintN argc, jsval * argv, jsval * rval) throw ();
};

class SFVec2f : public sfield {
public:
    static JSClass jsclass;

    static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
    static JSBool toJsval(const vec2f & vec2f,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<openvrml::sfvec2f>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (bad_conversion, std::bad_alloc);

private:
    static JSBool constructor(JSContext * cx, JSObject * obj,
	                      uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool initObject(JSContext * cx, JSObject * obj,
                             uintN argc, jsval * argv) throw ();
    static JSBool getProperty(JSContext * cx, JSObject * obj, jsval id,
                             jsval * rval) throw ();
    static JSBool setProperty(JSContext * cx, JSObject * obj, jsval id,
                              jsval * vp) throw ();
    static JSBool add(JSContext * cx, JSObject * obj,
                      uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool divide(JSContext * cx, JSObject * obj,
                         uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool dot(JSContext * cx, JSObject * obj,
                      uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool length(JSContext * cx, JSObject * obj,
                         uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool multiply(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool negate(JSContext * cx, JSObject * obj,
                         uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool normalize(JSContext * cx, JSObject * obj,
                            uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool subtract(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval) throw ();
};

class SFVec3f : public sfield {
public:
    static JSClass jsclass;

    static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
    static JSBool toJsval(const vec3f & vec3f,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<openvrml::sfvec3f>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (bad_conversion, std::bad_alloc);

private:
    static JSBool constructor(JSContext * cx, JSObject * obj,
                              uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool initObject(JSContext * cx, JSObject * obj,
                             uintN argc, jsval * argv) throw ();
    static JSBool getProperty(JSContext * cx, JSObject * obj, jsval id,
                              jsval * vp) throw ();
    static JSBool setProperty(JSContext * cx, JSObject * obj, jsval id,
                              jsval * vp) throw ();
    static JSBool add(JSContext * cx, JSObject * obj,
                      uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool cross(JSContext * cx, JSObject * obj,
                        uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool divide(JSContext * cx, JSObject * obj,
                         uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool dot(JSContext * cx, JSObject * obj,
                      uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool length(JSContext * cx, JSObject * obj,
                         uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool multiply(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool negate(JSContext * cx, JSObject * obj,
                         uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool normalize(JSContext * cx, JSObject * obj,
                            uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool subtract(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval) throw ();
};


class MField {
public:
    typedef std::vector<jsval> JsvalArray;

    class MFData : public field_data {
    public:
        JsvalArray array;

        explicit MFData(JsvalArray::size_type size);
        virtual ~MFData();
    };

protected:
    static void AddRoots(JSContext * cx, JsvalArray & jsvalArray)
        throw (std::bad_alloc);
    static void RemoveRoots(JSContext * cx, JsvalArray & jsvalArray) throw ();

    static JSBool getElement(JSContext * cx, JSObject * obj,
                             jsval id, jsval * vp) throw ();
    static JSBool getLength(JSContext * cx, JSObject * obj,
                            jsval id, jsval * vp) throw ();

private:
    MField();
};

/**
 * @class Base class template for SFColor, SFNode, SFRotation,
 *      SFVec2f, SFVec3f.
 */
template <typename Subclass>
class MFJSObject : public MField {
public:
    static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();

protected:
    static JSBool construct(JSContext * cx, JSObject * obj,
	                    uintN argc, jsval * argv, jsval * vp) throw ();
    static JSBool setElement(JSContext * cx, JSObject * obj,
                             jsval id, jsval * vp) throw ();
    static JSBool setLength(JSContext * cx, JSObject * obj,
                            jsval id, jsval * vp) throw ();
    static JSBool toString(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval) throw ();
    static void finalize(JSContext * cx, JSObject * obj) throw ();

private:
    static JSBool initObject(JSContext * cx, JSObject * obj,
                             uintN argc, jsval * argv) throw ();
};

/**
 * @class Base class template for MFFloat, MFTime.
 *
 * In JavaScript, all floating point values are treated as doubles.
 */
template <typename Subclass>
class MFJSDouble : public MField {
public:
    static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();

protected:
    static JSBool construct(JSContext * cx, JSObject * obj,
	                    uintN argc, jsval * argv, jsval * vp) throw ();
    static JSBool setElement(JSContext * cx, JSObject * obj,
                             jsval id, jsval * vp) throw ();
    static JSBool setLength(JSContext * cx, JSObject * obj,
                            jsval id, jsval * vp) throw ();
    static JSBool toString(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval) throw ();
    static void finalize(JSContext * cx, JSObject * obj) throw ();

private:
    static JSBool initObject(JSContext * cx, JSObject * obj,
                             uintN argc, jsval * argv) throw ();
};

class MFColor : public MFJSObject<MFColor> {
public:
    static JSClass jsclass;
    static JSClass & sfjsclass;

    static JSBool toJsval(const std::vector<color> & colors,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<openvrml::mfcolor>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (bad_conversion, std::bad_alloc);
};

class MFFloat : public MFJSDouble<MFFloat> {
public:
    static JSClass jsclass;

    static JSBool toJsval(const std::vector<float> & floats,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<openvrml::mffloat>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (bad_conversion, std::bad_alloc);
};

class MFInt32 : public MField {
public:
    static JSClass jsclass;

    static JSObject * initClass(JSContext * cx, JSObject * obj);
    static JSBool toJsval(const std::vector<openvrml::int32> & int32s,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<openvrml::mfint32>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (bad_conversion, std::bad_alloc);

private:
    static JSBool construct(JSContext * cx, JSObject * obj,
	                    uintN argc, jsval * argv,
                            jsval * vp);
    static JSBool initObject(JSContext * cx, JSObject * obj,
                             uintN argc, jsval * argv);
    static JSBool setElement(JSContext * cx, JSObject * obj,
                             jsval id, jsval * vp);
    static JSBool setLength(JSContext * cx, JSObject * obj,
                            jsval id, jsval * vp);
    static JSBool toString(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval);
    static void finalize(JSContext * cx, JSObject * obj);
};

//
// We can't use the MFJSObject<> template to implement MFNode, since
// the SFNode constructor requires an argument.
//
class MFNode : public MField {
public:
    static JSClass jsclass;

    static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
    static JSBool toJsval(const std::vector<node_ptr> & nodes,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<openvrml::mfnode>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (bad_conversion, std::bad_alloc);

private:
    static JSBool construct(JSContext * cx, JSObject * obj,
	                    uintN argc, jsval * argv,
                            jsval * vp) throw ();
    static JSBool initObject(JSContext * cx, JSObject * obj,
                             uintN argc, jsval * argv) throw ();
    static JSBool setElement(JSContext * cx, JSObject * obj,
                             jsval id, jsval * vp) throw ();
    static JSBool setLength(JSContext * cx, JSObject * obj,
                            jsval id, jsval * vp) throw ();
    static JSBool toString(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval) throw ();
    static void finalize(JSContext * cx, JSObject * obj) throw ();
};

class MFRotation : public MFJSObject<MFRotation> {
public:
    static JSClass jsclass;
    static JSClass & sfjsclass;

    static JSBool toJsval(const std::vector<rotation> & rotations,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<openvrml::mfrotation>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (bad_conversion, std::bad_alloc);
};

class MFString : public MField {
public:
    static JSClass jsclass;

    static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
    static JSBool toJsval(const std::vector<std::string> & strings,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<openvrml::mfstring>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (bad_conversion, std::bad_alloc);

private:
    static JSBool construct(JSContext * cx, JSObject * obj,
	                    uintN argc, jsval * argv,
                            jsval * vp) throw ();
    static JSBool initObject(JSContext * cx, JSObject * obj,
                             uintN argc, jsval * argv) throw ();
    static JSBool setElement(JSContext * cx, JSObject * obj,
                             jsval id, jsval * vp) throw ();
    static JSBool setLength(JSContext * cx, JSObject * obj,
                            jsval id, jsval * vp) throw ();
    static JSBool toString(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval) throw ();
    static void finalize(JSContext * cx, JSObject * obj) throw ();
};

class MFTime : public MFJSDouble<MFTime> {
public:
    static JSClass jsclass;

    static JSBool toJsval(const std::vector<double> & times,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<openvrml::mftime>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (bad_conversion, std::bad_alloc);
};

class MFVec2f : public MFJSObject<MFVec2f> {
public:
    static JSClass jsclass;
    static JSClass & sfjsclass;

    static JSBool toJsval(const std::vector<vec2f> & vec2fs,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<openvrml::mfvec2f>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (bad_conversion, std::bad_alloc);
};

class MFVec3f : public MFJSObject<MFVec3f> {
public:
    static JSClass jsclass;
    static JSClass & sfjsclass;

    static JSBool toJsval(const std::vector<vec3f> & vec3fs,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<openvrml::mfvec3f>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (bad_conversion, std::bad_alloc);
};

class VrmlMatrix {
public:
    //
    // The VrmlMatrixRow_ class is returned when using the [] operator on
    // a VrmlMatrix in order to facilitate matrix[i][j] notation. A
    // VrmlMatrixRow_ object holds a pointer to a row of the openvrml::mat4f
    // associated with the JavaScript VrmlMatrix that spawned it. The only
    // useful thing you can do with a VrmlMatrixRow_ object is use the []
    // operator to dereference a row element.
    //
    class Row {
    public:
        static JSClass jsclass;

        static JSObject * initClass(JSContext * cx, JSObject * obj)
            throw ();
        static JSBool construct(JSContext * cx, JSObject * obj,
	                        uintN argc, jsval * argv,
                                jsval * vp)
            throw ();
        static JSBool getElement(JSContext * cx, JSObject * obj,
                                 jsval id, jsval * vp)
            throw ();
        static JSBool setElement(JSContext * cx, JSObject * obj,
                                 jsval id, jsval * vp)
            throw ();
    };

    static JSClass jsclass;

    static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
    static JSBool construct(JSContext * cx, JSObject * obj,
	                    uintN argc, jsval * argv,
                            jsval * vp)
        throw ();
    static JSBool initObject(JSContext * cx, JSObject * obj,
                             uintN argc, jsval * argv)
        throw ();
    static JSBool getElement(JSContext * cx, JSObject * obj,
                             jsval id, jsval * vp)
        throw ();
    static JSBool setElement(JSContext * cx, JSObject * obj,
                             jsval id, jsval * vp)
        throw ();
    static JSBool setTransform(JSContext * cx, JSObject * obj,
                               uintN argc, jsval * argv, jsval * rval)
        throw ();
    static JSBool getTransform(JSContext * cx, JSObject * obj,
                               uintN argc, jsval * argv, jsval * rval)
        throw ();
    static JSBool inverse(JSContext * cx, JSObject * obj,
                          uintN argc, jsval * argv, jsval * rval)
        throw ();
    static JSBool transpose(JSContext * cx, JSObject * obj,
                            uintN argc, jsval * argv, jsval * rval)
        throw ();
    static JSBool multLeft(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval)
        throw ();
    static JSBool multRight(JSContext * cx, JSObject * obj,
                            uintN argc, jsval * argv, jsval * rval)
        throw ();
    static JSBool multVecMatrix(JSContext * cx, JSObject * obj,
                                uintN argc, jsval * argv, jsval * rval)
        throw ();
    static JSBool multMatrixVec(JSContext * cx, JSObject * obj,
                                uintN argc, jsval * argv, jsval * rval)
        throw ();
    static JSBool toString(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval)
        throw ();
    static void finalize(JSContext * cx, JSObject * obj) throw ();

private:
    VrmlMatrix();
};

JSRuntime * script::rt = 0; // Javascript runtime singleton object
size_t script::nInstances = 0; // Number of distinct script objects

JSBool eventOut_setProperty(JSContext * cx, JSObject * obj,
                            jsval id, jsval * val) throw ();

void errorReporter(JSContext * cx, const char * message,
                   JSErrorReport * report);


// Construct from inline script

script::script(openvrml::script_node & node, const std::string & source)
    throw (std::bad_alloc):
    openvrml::script(node),
    cx(0),
    sfnode_class(this->direct_output()
                 ? SFNode::direct_output_jsclass
                 : SFNode::jsclass)
{
    //
    // Initialize the runtime.
    //
    if (!rt) {
        if (!(rt = JS_NewRuntime(MAX_HEAP_BYTES))) { throw std::bad_alloc(); }
    }

    //
    // Initialize the context for this script object.
    //
    if (!(this->cx = JS_NewContext(rt, STACK_CHUNK_BYTES))) {
        throw std::bad_alloc();
    }

    //
    // Store a pointer to this script object in the context.
    //
    JS_SetContextPrivate(this->cx, this);

    JS_SetErrorReporter(cx, errorReporter);

    //
    // Define the global objects (builtins, Browser, SF*, MF*) ...
    //

    JSObject * const globalObj =
        JS_NewObject(this->cx, &Global::jsclass, 0, 0);
    if (!globalObj) { throw std::bad_alloc(); }

    if (!JS_InitStandardClasses(this->cx, globalObj)) {
        throw std::bad_alloc();
    }

    //
    // "print" function; non-standard, but widely used.
    //
    static JSFunctionSpec globalFunctions[] = {
        { "print", Global::print, 0 },
        { 0, 0, 0 }
    };
    JSBool ok = JS_DefineFunctions(cx, globalObj, globalFunctions);
    assert(ok);

    //
    // VRML-like TRUE, FALSE syntax
    //
    if (!(JS_DefineProperty(cx, globalObj, "FALSE",
                           BOOLEAN_TO_JSVAL(false), 0, 0,
                           JSPROP_READONLY | JSPROP_PERMANENT)
            && JS_DefineProperty(cx, globalObj, "TRUE",
                                 BOOLEAN_TO_JSVAL(true), 0, 0,
                                 JSPROP_READONLY | JSPROP_PERMANENT))) {
        throw std::bad_alloc();
    }

    //
    // Browser object.
    //
    this->defineBrowserObject();

    //
    // Define SF*/MF* classes.
    //
    this->initVrmlClasses();

    //
    // Define field/eventOut vars for this script.
    //
    this->defineFields();

    /* These should indicate source location for diagnostics. */
    char *filename = 0;
    uintN lineno = 0;

    jsval rval;
    ok = JS_EvaluateScript(cx, globalObj, source.c_str(), source.length(),
                           filename, lineno, &rval);
    OPENVRML_VERIFY_(ok);

    ++nInstances;
}

script::~script()
{
    JS_DestroyContext(this->cx);
    if (--nInstances == 0) {
        JS_DestroyRuntime(rt);
        rt = 0;
    }
}

void script::do_initialize(const double timestamp)
{
    const sftime arg(timestamp);
    const field_value * argv[] = { &arg };
    this->activate(timestamp, "initialize", 1, argv);
}

void script::do_process_event(const std::string & id,
                              const field_value & value,
                              const double timestamp)
{
    const sftime timestampArg(timestamp);
    const field_value * argv[] = { &value, &timestampArg };
    this->activate(timestamp, id, 2, argv);
}

void script::do_events_processed(const double timestamp)
{
    const sftime arg(timestamp);
    const field_value * argv[] = { &arg };
    this->activate(timestamp, "eventsProcessed", 1, argv);
}

void script::do_shutdown(const double timestamp)
{
    const sftime arg(timestamp);
    const field_value * argv[] = { &arg };
    this->activate(timestamp, "shutdown", 1, argv);
}

double s_timeStamp; // XXX go away...

/**
 * @brief Run a specified script.
 *
 * @todo Decide what to do if a call to a JavaScript function fails
 *      (probably due to an error in the script).
 */
void script::activate(const double timeStamp, const std::string & fname,
                      const size_t argc,
                      const field_value * const argv[])
{
    assert(this->cx);

    jsval fval, rval;
    JSObject * const globalObj = JS_GetGlobalObject(this->cx);
    assert(globalObj);

    try {
        if (!JS_LookupProperty(this->cx, globalObj,
                               fname.c_str(), &fval)) {
            throw std::bad_alloc();
        }

        //
        // The function may not be defined, in which case we should do
        // nothing.
        //
        if (JSVAL_IS_VOID(fval)) { return; }

        d_timeStamp = timeStamp;
        s_timeStamp = timeStamp; // XXX this won't work for long...

        // convert FieldValue*'s to (gc-protected) jsvals
        std::vector<jsval> jsargv(argc);
        size_t i;
        for (i = 0; i < argc; ++i) {
            assert(argv[i]);
            jsargv[i] = vrmlFieldToJSVal(*argv[i]);
            if (JSVAL_IS_GCTHING(jsargv[i])) {
                if (!JS_AddRoot(this->cx, &jsargv[i])) {
                    throw std::bad_alloc();
                }
            }
        }

        JSBool ok = JS_CallFunctionValue(this->cx, globalObj,
                                         fval, argc, &jsargv[0], &rval);
        // XXX
        // XXX What should we do at this point if a function call fails?
        // XXX For now, just print a message for a debug build.
        // XXX
        OPENVRML_VERIFY_(ok);

        // Free up args
        for (i = 0; i < argc; ++i) {
            assert(jsargv[i] != JSVAL_NULL);
            if (JSVAL_IS_GCTHING(jsargv[i])) {
                ok = JS_RemoveRoot(cx, &jsargv[i]);
                assert(ok);
            }
        }

        //
        // Check to see if any eventOuts need to be sent.
        //
        for (script_node::eventout_map_t::const_iterator eventout =
                 this->node.eventout_map().begin();
             eventout != this->node.eventout_map().end();
             ++eventout) {
            assert(eventout->second);
            jsval val;
            if (!JS_LookupProperty(this->cx, globalObj,
                                   eventout->first.c_str(), &val)) {
                throw std::bad_alloc();
            }
            assert(val != JSVAL_VOID);
            if (JSVAL_IS_OBJECT(val)) {
                field_data * fieldData =
                    static_cast<field_data *>
                        (JS_GetPrivate(this->cx, JSVAL_TO_OBJECT(val)));
                if (fieldData->changed) {
                    using std::auto_ptr;
                    auto_ptr<field_value> fieldValue =
                        createFieldValueFromJsval(
                            this->cx,
                            val,
                            eventout->second->value().type());
                    eventout->second->value(*fieldValue);
                    fieldData->changed = false;
                }
            }
        }
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    } catch (...) {
        assert(false);
    }
}

script_node & script::script_node()
{
    return this->node;
}

/**
 * @brief Create a jsval from an openvrml::field_value.
 */
jsval script::vrmlFieldToJSVal(const field_value & fieldValue) throw ()
{
    jsval rval;
    JSObject * const globalObj = JS_GetGlobalObject(this->cx);
    assert(globalObj);

    switch (fieldValue.type()) {
    case field_value::sfbool_id:
        {
            using openvrml::sfbool;
            const sfbool & b = static_cast<const sfbool &>(fieldValue);
	    rval = BOOLEAN_TO_JSVAL(b.value);
        }
        break;

    case field_value::sfcolor_id:
        {
            using openvrml::sfcolor;
            const sfcolor & c = static_cast<const sfcolor &>(fieldValue);
            if (!SFColor::toJsval(c.value, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::sffloat_id:
        {
            using openvrml::sffloat;
	    const sffloat & f = static_cast<const sffloat &>(fieldValue);
            if (!JS_NewDoubleValue(cx, f.value, &rval)) { rval = JSVAL_NULL; }
        }
        break;

    case field_value::sfimage_id:
        {
            const openvrml::sfimage & sfimage =
                    static_cast<const openvrml::sfimage &>(fieldValue);
            if (!SFImage::toJsval(sfimage, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::sfint32_id:
        {
            const openvrml::sfint32 & sfint32 =
                    static_cast<const openvrml::sfint32 &>(fieldValue);
            if (!JS_NewNumberValue(cx, jsdouble(sfint32.value), &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::sfnode_id:
        {
            const openvrml::sfnode & sfnode =
                    static_cast<const openvrml::sfnode &>(fieldValue);
            if (!SFNode::toJsval(sfnode.value, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::sfrotation_id:
        {
            const openvrml::sfrotation & sfrotation =
                static_cast<const openvrml::sfrotation &>(fieldValue);
            if (!SFRotation::toJsval(sfrotation.value, this->cx, globalObj,
                                     &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::sfstring_id:
        {
            const openvrml::sfstring & sfstring =
                    static_cast<const openvrml::sfstring &>(fieldValue);
            rval = STRING_TO_JSVAL(
                        JS_NewStringCopyZ(cx, sfstring.value.c_str()));
        }
        break;

    case field_value::sftime_id:
        {
	    const openvrml::sftime & sftime =
                    static_cast<const openvrml::sftime &>(fieldValue);
            if (!JS_NewDoubleValue(cx, sftime.value, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::sfvec2f_id:
        {
            const openvrml::sfvec2f & sfvec2f =
                    static_cast<const openvrml::sfvec2f &>(fieldValue);
            if (!SFVec2f::toJsval(sfvec2f.value, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::sfvec3f_id:
        {
            const openvrml::sfvec3f & sfvec3f =
                    static_cast<const openvrml::sfvec3f &>(fieldValue);
            if (!SFVec3f::toJsval(sfvec3f.value, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::mfcolor_id:
        {
            const openvrml::mfcolor & mfcolor =
                    static_cast<const openvrml::mfcolor &>(fieldValue);
            if (!MFColor::toJsval(mfcolor.value, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::mffloat_id:
        {
            const openvrml::mffloat & mffloat =
                    static_cast<const openvrml::mffloat &>(fieldValue);
            if (!MFFloat::toJsval(mffloat.value, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::mfint32_id:
        {
            const openvrml::mfint32 & mfint32 =
                    static_cast<const openvrml::mfint32 &>(fieldValue);
            if (!MFInt32::toJsval(mfint32.value, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::mfnode_id:
        {
            const openvrml::mfnode & mfnode =
                    static_cast<const openvrml::mfnode &>(fieldValue);
            if (!MFNode::toJsval(mfnode.value, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::mfrotation_id:
        {
            const openvrml::mfrotation & mfrotation =
                static_cast<const openvrml::mfrotation &>(fieldValue);
            if (!MFRotation::toJsval(mfrotation.value, this->cx, globalObj,
                                     &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::mfstring_id:
        {
            const openvrml::mfstring & mfstring =
                    static_cast<const openvrml::mfstring &>(fieldValue);
            if (!MFString::toJsval(mfstring.value, this->cx, globalObj,
                                   &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::mftime_id:
        {
            const openvrml::mftime & mftime =
                    static_cast<const openvrml::mftime &>(fieldValue);
            if (!MFTime::toJsval(mftime.value, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::mfvec2f_id:
        {
            const openvrml::mfvec2f & mfvec2f =
                    static_cast<const openvrml::mfvec2f &>(fieldValue);
            if (!MFVec2f::toJsval(mfvec2f.value, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::mfvec3f_id:
        {
            const openvrml::mfvec3f & mfvec3f =
                    static_cast<const openvrml::mfvec3f &>(fieldValue);
            if (!MFVec3f::toJsval(mfvec3f.value, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    default:
        assert(false);
    }

    return rval;
}


// Must assign the proper type to eventOuts

JSBool eventOut_setProperty(JSContext * const cx,
                            JSObject * const obj,
                            const jsval id,
                            jsval * const val)
    throw ()
{
    JSString * const str = JS_ValueToString(cx, id);
    if (!str) { return JS_FALSE; }
    const char * const eventId = JS_GetStringBytes(str);

    //
    // The script object pointer is stored as private data in the
    // context.
    //
    js_::script * const script =
            static_cast<js_::script *>(JS_GetContextPrivate(cx));
    assert(script);

    script_node & scriptNode = script->script_node();

    const node_interface_set & interfaces =
        scriptNode.node::type().interfaces();
    const node_interface_set::const_iterator interface =
        find_if(interfaces.begin(), interfaces.end(),
                bind2nd(node_interface_matches_eventout(), eventId));
    //
    // If this assertion is false, then we accidentally gave this
    // setter to an object that doesn't correspond to an eventOut!
    //
    assert(interface != interfaces.end());

    const field_value::type_id field_type_id = interface->field_type;

    //
    // Convert to an openvrml::field_value and set the eventOut value.
    //
    try {
        using std::auto_ptr;

        auto_ptr<field_value> fieldValue =
            createFieldValueFromJsval(cx, *val, field_type_id);
        const script_node::eventout_map_t::const_iterator eventout =
            scriptNode.eventout_map().find(eventId);
        assert(eventout != scriptNode.eventout_map().end());
        eventout->second->value(*fieldValue);
    } catch (bad_conversion & ex) {
        JS_ReportError(cx, ex.what());
    } catch (std::bad_alloc &) {
        JS_ReportOutOfMemory(cx);
        return JS_FALSE;
    }
    return JS_TRUE;
}

JSBool script::field_setProperty(JSContext * const cx,
                                 JSObject * const obj,
                                 const jsval id,
                                 jsval * const val)
    throw ()
{
    JSString * const str = JS_ValueToString(cx, id);
    if (!str) { return JS_FALSE; }
    const char * const fieldId = JS_GetStringBytes(str);

    //
    // The script object pointer is stored as private data in the
    // context.
    //
    js_::script * const script =
        static_cast<js_::script *>(JS_GetContextPrivate(cx));
    assert(script);

    openvrml::script_node & scriptNode = script->script_node();

    const node_interface_set & interfaces =
        scriptNode.node::type().interfaces();
    const node_interface_set::const_iterator interface =
        find_if(interfaces.begin(), interfaces.end(),
                bind2nd(node_interface_matches_field(), fieldId));
    //
    // If this assertion is false, then we accidentally gave this
    // setter to an object that doesn't correspond to a field!
    //
    assert(interface != interfaces.end());

    const field_value::type_id field_type_id = interface->field_type;

    //
    // Convert to an openvrml::FieldValue and set the field value.
    //
    try {
        using std::auto_ptr;
        auto_ptr<field_value> fieldValue =
            createFieldValueFromJsval(cx, *val, field_type_id);
        script->field(fieldId, *fieldValue);
    } catch (bad_conversion & ex) {
        JS_ReportError(cx, ex.what());
        return JS_FALSE;
    } catch (unsupported_interface & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        assert(false);
    } catch (std::bad_cast & ex) {
        JS_ReportError(cx, ex.what());
        return JS_FALSE;
    } catch (std::bad_alloc &) {
        JS_ReportOutOfMemory(cx);
        return JS_FALSE;
    }

    return JS_TRUE;
}

//
// Initialize SF*/MF* types.
//
void script::initVrmlClasses() throw (std::bad_alloc)
{
    JSObject * const globalObj = JS_GetGlobalObject(this->cx);
    assert(globalObj);
    if (!(SFColor::initClass(this->cx, globalObj)
            && SFImage::initClass(this->cx, globalObj)
            && SFNode::initClass(this->cx, globalObj)
            && SFRotation::initClass(this->cx, globalObj)
            && SFVec2f::initClass(this->cx, globalObj)
            && SFVec3f::initClass(this->cx, globalObj)
            && MFColor::initClass(this->cx, globalObj)
            && MFFloat::initClass(this->cx, globalObj)
            && MFInt32::initClass(this->cx, globalObj)
            && MFNode::initClass(this->cx, globalObj)
            && MFRotation::initClass(this->cx, globalObj)
            && MFString::initClass(this->cx, globalObj)
            && MFTime::initClass(this->cx, globalObj)
            && MFVec2f::initClass(this->cx, globalObj)
            && MFVec3f::initClass(this->cx, globalObj)
            && VrmlMatrix::initClass(this->cx, globalObj)
            && VrmlMatrix::Row::initClass(this->cx, globalObj))) {
        throw std::bad_alloc();
    }
}

//
// Define the Browser object.
//
void script::defineBrowserObject() throw (std::bad_alloc)
{
    JSObject * const globalObj = JS_GetGlobalObject(this->cx);
    assert(globalObj);

    static JSFunctionSpec methods[] =
            { { "getName", Browser::getName, 0 },
              { "getVersion", Browser::getVersion, 0 },
              { "getCurrentSpeed", Browser::getCurrentSpeed, 0 },
              { "getCurrentFrameRate", Browser::getCurrentFrameRate, 0 },
              { "getWorldURL", Browser::getWorldURL, 0 },
              { "replaceWorld", Browser::replaceWorld, 1 },
              { "createVrmlFromString", Browser::createVrmlFromString, 1 },
              { "createVrmlFromURL", Browser::createVrmlFromURL, 3 },
              { "addRoute", Browser::addRoute, 4 },
              { "deleteRoute", Browser::deleteRoute, 4 },
              { "loadURL", Browser::loadURL, 2 },
              { "setDescription", Browser::setDescription, 1 },
              { 0 } };

    JSObject * const browserObj =
            JS_DefineObject(this->cx, globalObj,
                            "Browser", &Browser::jsclass, 0, 0);
    if (!browserObj) { throw std::bad_alloc(); }

    if (!JS_DefineFunctions(this->cx, browserObj, methods)) {
        throw std::bad_alloc();
    }
}

//
// Define objects corresponding to fields/eventOuts
//
void script::defineFields() throw (std::bad_alloc)
{
    JSObject * const globalObj = JS_GetGlobalObject(this->cx);
    assert(globalObj);

    {
        script_node::field_value_map_t::const_iterator
            itr(this->node.field_value_map().begin());
        for (; itr != this->node.field_value_map().end(); ++itr) {
            assert(itr->second);
            jsval val = vrmlFieldToJSVal(*itr->second);
            if (!JS_DefineProperty(this->cx, globalObj,
                                   itr->first.c_str(), val,
                                   0, field_setProperty, // getter, setter
                                   JSPROP_PERMANENT)) {
                throw std::bad_alloc();
            }
        }
    }

    for (script_node::eventout_map_t::const_iterator eventout =
             this->node.eventout_map().begin();
         eventout != this->node.eventout_map().end();
         ++eventout) {
        assert(eventout->second);
        jsval val = vrmlFieldToJSVal(eventout->second->value());
        if (!JS_DefineProperty(this->cx,
                               globalObj,
                               eventout->first.c_str(),
                               val,
                               0,                    // getter
                               eventOut_setProperty, // setter
                               JSPROP_PERMANENT)) {
            throw std::bad_alloc();
        }
    }
}


void errorReporter(JSContext * const cx,
                   const char * const message,
                   JSErrorReport * const errorReport)
{
    using std::endl;
    using std::string;

    js_::script * const script =
        static_cast<js_::script *>(JS_GetContextPrivate(cx));
    assert(script);

    openvrml::browser & browser = script->script_node().scene()->browser;

    string nodeId = script->script_node().id();
    if (!nodeId.empty()) {
        browser.err << nodeId << ": ";
    }

    if (errorReport) {
        if (errorReport->filename) {
            browser.err << errorReport->filename << ": ";
        }

        browser.err << errorReport->lineno << ": ";
    }

    browser.err << message << endl;
}

JSBool floatsToJSArray(const size_t numFloats,
                       const float * floats,
                       JSContext * const cx,
                       jsval * const rval)
{
    std::vector<jsval> jsvec(numFloats);

    size_t i;
    for (i = 0; i < numFloats; ++i) {
        if (!JS_NewDoubleValue(cx, floats[i], &jsvec[i])) {
            return JS_FALSE;
        }
        if (!JS_AddRoot(cx, &jsvec[i])) { return JS_FALSE; }
    }

    JSObject * arr = 0;
    if (i == numFloats) {
        arr = JS_NewArrayObject(cx, numFloats, &jsvec[0]);
        if (arr) { *rval = OBJECT_TO_JSVAL(arr); }
    }

    for (size_t j = 0; j < i; ++j) { JS_RemoveRoot(cx, &jsvec[j]); }

    if (!arr) { return JS_FALSE; }

    *rval = OBJECT_TO_JSVAL(arr);
    return JS_TRUE;
}

/**
 * @brief Convert a jsval to a (new) field_value.
 */
std::auto_ptr<field_value>
createFieldValueFromJsval(JSContext * const cx,
                          const jsval v,
                          const field_value::type_id expectType)
    throw (bad_conversion, std::bad_alloc)
{
    using std::auto_ptr;

    switch (expectType) {
    case field_value::sfbool_id:
        {
            if (!JSVAL_IS_BOOLEAN(v)) {
                throw bad_conversion("Boolean value expected.");
            }
            return auto_ptr<field_value>(new sfbool(JSVAL_TO_BOOLEAN(v)));
        }

    case field_value::sfcolor_id:
        if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
        return auto_ptr<field_value>
            (SFColor::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::sffloat_id:
        {
            jsdouble d;
            if (!JS_ValueToNumber(cx, v, &d)) {
                throw bad_conversion("Numeric value expected.");
            }
            return auto_ptr<field_value>(new sffloat(d));
        }

    case field_value::sfint32_id:
        {
            ::int32 i;
            if (!JS_ValueToECMAInt32(cx, v, &i)) {
                throw bad_conversion("Numeric value expected.");
            }
            return auto_ptr<field_value>(new sfint32(i));
        }

    case field_value::sfimage_id:
        if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
        return auto_ptr<field_value>
            (SFImage::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::sfnode_id:
        if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
        return auto_ptr<field_value>
            (SFNode::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::sfrotation_id:
        if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
        return auto_ptr<field_value>
            (SFRotation::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::sfstring_id:
        {
            if (!JSVAL_IS_STRING(v)) {
                throw bad_conversion("String value expected.");
            }
            JSString * const jsstring = JSVAL_TO_STRING(v);
            //
            // Is the null check here really necessary? Perhaps this
            // should be an assertion?
            //
            return auto_ptr<field_value>
                (new sfstring(jsstring
                              ? JS_GetStringBytes(jsstring)
                              : ""));
        }

    case field_value::sftime_id:
        {
            jsdouble d;
            if (!JS_ValueToNumber(cx, v, &d)) {
                throw bad_conversion("Numeric value expected.");
            }
            return auto_ptr<field_value>(new sftime(d));
        }

    case field_value::sfvec2f_id:
        if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
        return auto_ptr<field_value>
            (SFVec2f::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::sfvec3f_id:
        if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
        return auto_ptr<field_value>
            (SFVec3f::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::mfcolor_id:
        if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
        return auto_ptr<field_value>
            (MFColor::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::mffloat_id:
        if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
        return auto_ptr<field_value>
            (MFFloat::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::mfint32_id:
        if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
        return auto_ptr<field_value>
            (MFInt32::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::mfnode_id:
        if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
        return auto_ptr<field_value>
            (MFNode::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::mfrotation_id:
        if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
        return auto_ptr<field_value>
            (MFRotation::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::mfstring_id:
        if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
        return auto_ptr<field_value>
            (MFString::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::mftime_id:
        if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
        return auto_ptr<field_value>
            (MFTime::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::mfvec2f_id:
        if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
        return auto_ptr<field_value>
            (MFVec2f::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::mfvec3f_id:
        if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
        return auto_ptr<field_value>
            (MFVec3f::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    default:
        assert(false);
        return auto_ptr<field_value>();
    }
}

namespace Global {

JSBool print(JSContext * const cx, JSObject *,
             const uintN argc, jsval * const argv, jsval *)
{
    using std::cout;
    using std::flush;

    for (uintN i = 0; i < argc; i++) {
        JSString * const str = JS_ValueToString(cx, argv[i]);
        if (!str) {
            return JS_FALSE;
        }
        cout << JS_GetStringBytes(str) << "\n";
    }
    cout << flush;
    return JS_TRUE;
}

}

namespace Browser {

JSBool getName(JSContext * const cx, JSObject *,
	       uintN, jsval *, jsval * const rval)
    throw ()
{
    js_::script * const script =
        static_cast<js_::script *>(JS_GetContextPrivate(cx));
    assert(script);

    const char * const name =
        script->script_node().node::type().node_class().browser().name();
    *rval = STRING_TO_JSVAL(JS_InternString(cx, name));
    return JS_TRUE;
}

JSBool getVersion(JSContext * const cx, JSObject *,
		  uintN, jsval *, jsval * const rval)
    throw ()
{
    js_::script * const script =
        static_cast<js_::script *>(JS_GetContextPrivate(cx));
    assert(script);

    const char * const version =
        script->script_node().node::type().node_class().browser().version();
    *rval = STRING_TO_JSVAL(JS_InternString(cx, version));
    return JS_TRUE;
}

JSBool getCurrentSpeed(JSContext * const cx, JSObject *,
                       uintN, jsval *, jsval * const rval)
    throw ()
{
    js_::script * const script =
        static_cast<js_::script *>(JS_GetContextPrivate(cx));
    assert(script);

    float speed = script->script_node().node::type().node_class()
                  .browser().current_speed();
    *rval = DOUBLE_TO_JSVAL(JS_NewDouble( cx, speed ));
    return JS_TRUE;
}

JSBool getCurrentFrameRate(JSContext * const cx,
                           JSObject *,
                           uintN,
                           jsval *,
                           jsval * const rval)
    throw ()
{
    js_::script * const script =
        static_cast<js_::script *>(JS_GetContextPrivate(cx));
    assert(script);

    jsdouble * d = JS_NewDouble(cx,
                                script->script_node().node::type().node_class()
                                .browser().frame_rate());
    *rval = DOUBLE_TO_JSVAL(d);
    return JS_TRUE;
}

JSBool getWorldURL(JSContext * const cx,
                   JSObject *,
                   uintN,
                   jsval *,
                   jsval * const rval)
    throw ()
{
    js_::script * const script =
        static_cast<js_::script *>(JS_GetContextPrivate(cx));
    assert(script);

    const std::string url =
        script->script_node().node::type().node_class().browser().world_url();
    *rval = STRING_TO_JSVAL(JS_InternString(cx, url.c_str()));
    return JS_TRUE;
}


// No events will be processed after loadURL.

JSBool loadURL(JSContext * const cx,
               JSObject *,
               const uintN argc,
               jsval * const argv,
               jsval * const rval)
    throw ()
{
    using std::auto_ptr;

    assert(JS_GetContextPrivate(cx));
    js_::script & script =
        *static_cast<js_::script *>(JS_GetContextPrivate(cx));

    //
    // Make sure our first argument (the URL) is an MFString.
    //
    JSObject * arg0_obj;
    if (!JS_ValueToObject(cx, argv[0], &arg0_obj)) { return JS_FALSE; }
    if (!JS_InstanceOf(cx, arg0_obj, &MFString::jsclass, argv)) {
        return JS_FALSE;
    }

    auto_ptr<openvrml::mfstring> url =
        MFString::createFromJSObject(cx, arg0_obj);
    assert(url.get());

    //
    // Make sure our second argument is an MFString
    //
    JSObject * arg1_obj;
    if (!JS_ValueToObject(cx, argv[1], &arg1_obj)) { return JS_FALSE; }
    if (!JS_InstanceOf(cx, arg1_obj, &MFString::jsclass, argv)) {
        return JS_FALSE;
    }

    auto_ptr<openvrml::mfstring> parameters =
        MFString::createFromJSObject(cx, arg1_obj);
    assert(parameters.get());

    script.script_node().scene()->browser.load_url(url->value,
                                                   parameters->value);
    return JS_TRUE;
}


// This does return, but no events will be processed after it is called.

JSBool replaceWorld(JSContext * const cx,
                    JSObject *,
                    const uintN argc,
                    jsval * const argv,
                    jsval * const rval)
    throw ()
{
    using std::auto_ptr;

    assert(JS_GetContextPrivate(cx));
    js_::script & script =
        *static_cast<js_::script *>(JS_GetContextPrivate(cx));

    //
    // Make sure our argument is an MFNode.
    //
    JSObject * arg_obj;
    if (!JS_ValueToObject(cx, argv[0], &arg_obj)) { return JS_FALSE; }
    if (!JS_InstanceOf(cx, arg_obj, &MFNode::jsclass, argv)) {
        return JS_FALSE;
    }

    auto_ptr<openvrml::mfnode> nodes =
        MFNode::createFromJSObject(cx, arg_obj);
    assert(nodes.get());

    script.script_node().scene()->browser.replace_world(nodes->value);

    *rval = JSVAL_VOID;
    return JS_TRUE;
}

JSBool createVrmlFromString(JSContext * const cx,
                            JSObject * const obj,
			    const uintN argc,
                            jsval * const argv,
                            jsval * const rval)
    throw ()
{
    assert(JS_GetContextPrivate(cx));
    js_::script & script =
        *static_cast<js_::script *>(JS_GetContextPrivate(cx));

    //
    // Make sure our argument is a string.
    //
    JSString * str = JS_ValueToString(cx, argv[0]);
    if (!str) { return JS_FALSE; }

    try {
        std::istringstream in(JS_GetStringBytes(str));

        assert(script.script_node().scene());
        openvrml::browser & browser =
            script.script_node().scene()->browser;
        const std::vector<node_ptr> nodes =
            browser.create_vrml_from_stream(in);

        if (nodes.empty()) {
            *rval = JSVAL_NULL;
        } else {
            if (!MFNode::toJsval(nodes, cx, obj, rval)) { return JS_FALSE; }
        }
    } catch (...) {
        return JS_FALSE;
    }
    return JS_TRUE;
}


// createVrmlFromURL( MFString url, SFNode node, SFString event )

JSBool createVrmlFromURL(JSContext * const cx,
                         JSObject *,
			 const uintN argc,
                         jsval * const argv,
                         jsval * const rval)
    throw ()
{
# if 0
    using std::auto_ptr;

    assert(JS_GetContextPrivate(cx));
    js_::script & script =
        *static_cast<js_::script *>(JS_GetContextPrivate(cx));

    Doc2 * relative = script.script_node().type.node_class.browser.urlDoc();

    //
    // Make sure our first argument (the URL) is an MFString.
    //
    JSObject * arg0_obj;
    if (!JS_ValueToObject(cx, argv[0], &arg0_obj)) { return JS_FALSE; }
    if (!JS_InstanceOf(cx, arg0_obj, &MFString::jsclass, argv)) {
        return JS_FALSE;
    }

    auto_ptr<openvrml::mfstring>
        url(MFString::createFromJSObject(cx, arg0_obj));
    assert(url.get());

    //
    // Make sure our second arument is an SFNode.
    //
    js_::script & script =
        *static_cast<js_::script *>(JS_GetContextPrivate(cx));
    JSClass & sfnode_jsclass = script.sfnode_class;

    JSObject * arg1_obj;
    if (!JS_ValueToObject(cx, argv[1], &arg1_obj)) { return JS_FALSE; }
    if (!JS_InstanceOf(cx, arg1_obj, &sfnode_jsclass, argv)) {
        return JS_FALSE;
    }

    auto_ptr<openvrml::sfnode>
        sfnode(SFNode::createFromJSObject(cx, arg1_obj));
    assert(sfnode.get());
    node_ptr node(sfnode->get());
    if (!node) { return JS_FALSE; }

    //
    // Make sure our third argument is a string.
    //
    JSString * arg2_str = JS_ValueToString(cx, argv[2]);
    if (!arg2_str) { return JS_FALSE; }

    const char * const eventInId = JS_GetStringBytes(arg2_str);
    openvrml::browser & browser =
        script.script_node().type.node_class.browser;
    auto_ptr<openvrml::mfnode> kids(browser.readWrl(*url, relative));

    if (!kids.get()) { return JS_FALSE; }

    node->processEvent(eventInId, *kids, s_timeStamp); // fix me...

    *rval = JSVAL_VOID;
# endif
    return JS_TRUE;
}

// addRoute(SFNode fromNode, String fromEventOut, SFNode toNode, String toEvent)

JSBool addRoute(JSContext * const cx,
                JSObject *,
                const uintN argc,
                jsval * const argv,
                jsval * const rval)
    throw ()
{
    using std::auto_ptr;

    //
    // Make sure our first argument (fromNode) is a SFNode.
    //
    JSObject * arg0_obj;
    if (!JS_ValueToObject(cx, argv[0], &arg0_obj)) { return JS_FALSE; }
    js_::script & script =
        *static_cast<js_::script *>(JS_GetContextPrivate(cx));
    JSClass & sfnode_jsclass = script.sfnode_class;
    if (!JS_InstanceOf(cx, arg0_obj, &sfnode_jsclass, argv)) {
        return JS_FALSE;
    }
    auto_ptr<openvrml::sfnode> fromNode =
        SFNode::createFromJSObject(cx, arg0_obj);
    //
    // Make sure the node is not null.
    //
    if (!fromNode.get()) {
        JS_ReportError(cx, "NULL destination node.");
        return JS_FALSE;
    }

    //
    // Makes sure our second argument (fromEventOut) is a string.
    //
    JSString * arg1_str = JS_ValueToString(cx, argv[1]);
    if (!arg1_str) { return JS_FALSE; }
    const char * const fromEventOut = JS_GetStringBytes(arg1_str);

    //
    // Make sure our third argument (toNode) is a SFNode.
    //
    JSObject * arg2_obj;
    if (!JS_ValueToObject(cx, argv[2], &arg2_obj)) { return JS_FALSE; }
    if (!JS_InstanceOf(cx, arg2_obj, &sfnode_jsclass, argv)) {
        return JS_FALSE;
    }
    auto_ptr<openvrml::sfnode> toNode =
        SFNode::createFromJSObject(cx, arg2_obj);
    //
    // Make sure the node is not null.
    //
    if (!toNode.get()) {
        JS_ReportError(cx, "NULL destination node.");
        return JS_FALSE;
    }

    //
    // Makes sure our fourth argument (toEventIn) is a string.
    //
    JSString * arg3_str = JS_ValueToString(cx, argv[3]);
    if (!arg3_str) { return JS_FALSE; }
    const char * const toEventIn = JS_GetStringBytes(arg3_str);

    try {
        add_route(*fromNode->value, fromEventOut, *toNode->value, toEventIn);
    } catch (std::runtime_error & ex) {
        JS_ReportError(cx, ex.what());
        return JS_FALSE;
    }

    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// deleteRoute(SFNode fromNode, String fromEventOut, SFNode toNode, String toEvent)

JSBool deleteRoute(JSContext * const cx,
                   JSObject *,
                   const uintN argc,
                   jsval * const argv,
                   jsval * const rval)
    throw ()
{
    using std::auto_ptr;

    //
    // Make sure our first argument (fromNode) is a SFNode.
    //
    JSObject * arg0_obj;
    if (!JS_ValueToObject(cx, argv[0], &arg0_obj)) { return JS_FALSE; }
    js_::script & script =
        *static_cast<js_::script *>(JS_GetContextPrivate(cx));
    JSClass & sfnode_jsclass = script.sfnode_class;
    if (!JS_InstanceOf(cx, arg0_obj, &sfnode_jsclass, argv)) {
        return JS_FALSE;
    }
    auto_ptr<openvrml::sfnode> fromNode =
        SFNode::createFromJSObject(cx, arg0_obj);

    //
    // Makes sure our second argument (fromEventOut) is a string.
    //
    JSString * arg1_str = JS_ValueToString(cx, argv[1]);
    if (!arg1_str) { return JS_FALSE; }
    const char * const fromEventOut = JS_GetStringBytes(arg1_str);

    //
    // Make sure our third argument (toNode) is a SFNode.
    //
    JSObject * arg2_obj;
    if (!JS_ValueToObject(cx, argv[2], &arg2_obj)) { return JS_FALSE; }
    if (!JS_InstanceOf(cx, arg2_obj, &sfnode_jsclass, argv)) {
        return JS_FALSE;
    }
    auto_ptr<openvrml::sfnode> toNode =
        SFNode::createFromJSObject(cx, arg2_obj);

    //
    // Makes sure our fourth argument (toEventIn) is a string.
    //
    JSString * arg3_str = JS_ValueToString(cx, argv[3]);
    if (!arg3_str) { return JS_FALSE; }
    const char * const toEventIn = JS_GetStringBytes(arg3_str);

    delete_route(*fromNode->value, fromEventOut, *toNode->value, toEventIn);

    *rval = JSVAL_VOID;
    return JS_TRUE;
}

JSBool setDescription(JSContext * const cx,
                      JSObject *,
		      const uintN argc,
                      jsval * const argv,
                      jsval * const rval)
    throw ()
{
    JSString * str = JS_ValueToString(cx, argv[0]);
    if (!str) { return JS_FALSE; }
    assert(JS_GetContextPrivate(cx));
    js_::script & script =
        *static_cast<js_::script *>(JS_GetContextPrivate(cx));
    openvrml::browser & browser = script.script_node().scene()->browser;
    browser.description(JS_GetStringBytes(str));
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

} // namespace Browser


//
// field_data
//

field_data::field_data():
    changed(false)
{}

field_data::~field_data()
{}


//
// sfield
//

sfield::sfdata::sfdata(openvrml::field_value * const field_value):
    field_value_(field_value)
{
    assert(field_value);
}

sfield::sfdata::~sfdata()
{
    delete this->field_value_;
}

field_value & sfield::sfdata::field_value() const
{
    return *this->field_value_;
}

void sfield::finalize(JSContext * const cx, JSObject * const obj)
    throw ()
{
    delete static_cast<field_data *>(JS_GetPrivate(cx, obj));
    JS_SetPrivate(cx, obj, 0);
}

JSBool sfield::toString(JSContext * const cx, JSObject * const obj,
                        uintN, jsval *, jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));

    std::ostringstream out;
    out << sfdata.field_value();
    JSString * const jsstr = JS_NewStringCopyZ(cx, out.str().c_str());
    if (!jsstr) { return JS_FALSE; }
    *rval = STRING_TO_JSVAL(jsstr);
    return JS_TRUE;
}

JSClass SFColor::jsclass = {
    "SFColor",
    JSCLASS_HAS_PRIVATE,
    JS_PropertyStub,
    JS_PropertyStub,
    getProperty,
    setProperty,
    JS_EnumerateStub,
    JS_ResolveStub,
    JS_ConvertStub,
    finalize
};

JSObject * SFColor::initClass(JSContext * const cx, JSObject * const obj)
    throw ()
{
    static JSPropertySpec properties[] =
            { { "r", 0, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { "g", 1, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { "b", 2, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { 0, 0, 0, 0, 0 } };

    static JSFunctionSpec methods[] =
            // name, func ptr, argc
            { { "setHSV", setHSV, 3, 0, 0 },
              { "getHSV", getHSV, 0, 0, 0 },
              { "toString", toString, 0, 0, 0 },
              { 0, 0, 0, 0, 0 } };

    JSObject * const proto =
        JS_InitClass(cx, obj, 0, &jsclass,
                     construct, 0, // constructor function, min arg count
                     properties, methods,
                     0, 0); // static properties and methods
    if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
    return proto;
}

JSBool SFColor::toJsval(const color & color,
                        JSContext * const cx,
                        JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    JSObject * const sfcolorObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!sfcolorObj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, sfcolorObj));
    const sfield::sfdata & sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, sfcolorObj));

    assert(dynamic_cast<openvrml::sfcolor *>(&sfdata.field_value()));

    //
    // Assignment does not throw.
    //
    static_cast<openvrml::sfcolor &>(sfdata.field_value()).value = color;

    *rval = OBJECT_TO_JSVAL(sfcolorObj);
    return JS_TRUE;
}

std::auto_ptr<openvrml::sfcolor>
SFColor::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (bad_conversion, std::bad_alloc)
{
    using std::auto_ptr;

    assert(cx);
    if (!obj || !JS_InstanceOf(cx, obj, &SFColor::jsclass, 0)) {
        throw bad_conversion("SFColor object expected.");
    }
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));

    assert(dynamic_cast<openvrml::sfcolor *>(&sfdata.field_value()));

    return auto_ptr<openvrml::sfcolor>(
        static_cast<openvrml::sfcolor *>(
            sfdata.field_value().clone().release()));
}

JSBool SFColor::construct(JSContext * const cx,
                          JSObject * obj,
	                  const uintN argc,
                          jsval * const argv,
                          jsval * const rval)
    throw ()
{
    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return initObject(cx, obj, argc, argv);
}

JSBool SFColor::initObject(JSContext * const cx,
                           JSObject * const obj,
                           const uintN argc,
                           jsval * const argv)
    throw ()
{
    jsdouble rgb[] = { 0.0, 0.0, 0.0 };
    for (uintN i = 0; i < ((argc < 3) ? argc : 3); ++i) {
        if (!JSVAL_IS_NUMBER(argv[i])
                || !JS_ValueToNumber(cx, argv[i], rgb + i)) {
            JS_ReportError(cx, "Numeric value expected");
            return JS_FALSE;
        }
    }

    try {
        using std::auto_ptr;

        auto_ptr<openvrml::sfcolor>
            sfcolor(new openvrml::sfcolor(color(rgb[0], rgb[1], rgb[2])));
        auto_ptr<sfield::sfdata> sfdata(new sfield::sfdata(sfcolor.get()));
        sfcolor.release();
        if (!JS_SetPrivate(cx, obj, sfdata.get())) { return JS_FALSE; }
        sfdata.release();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        return JS_FALSE;
    } catch (...) {
        assert(false);
        return JS_FALSE;
    }
    return JS_TRUE;
}

JSBool SFColor::getProperty(JSContext * const cx,
                            JSObject * const obj,
                            const jsval id,
                            jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfcolor *>(&sfdata.field_value()));
    const openvrml::sfcolor & thisColor =
        static_cast<openvrml::sfcolor &>(sfdata.field_value());

    if (JSVAL_IS_INT(id)
        && JSVAL_TO_INT(id) >= 0
        && JSVAL_TO_INT(id) < 3) {
        if (!JS_NewDoubleValue(cx, thisColor.value[JSVAL_TO_INT(id)], rval)) {
            return JS_FALSE;
        }
    }
    return JS_TRUE;
}

JSBool SFColor::setProperty(JSContext * const cx,
                            JSObject * const obj,
                            const jsval id,
                            jsval * const vp)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    sfield::sfdata & sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfcolor *>(&sfdata.field_value()));
    openvrml::sfcolor & thisColor =
        static_cast<openvrml::sfcolor &>(sfdata.field_value());

    if (!JSVAL_IS_INT(id)
            || JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 2) {
        return JS_FALSE;
    }

    jsdouble d;
    if (!JS_ValueToNumber(cx, *vp, &d)) { return JS_FALSE; }

    switch (JSVAL_TO_INT(id)) {
    case 0:
        thisColor.value.r(d);
        break;

    case 1:
        thisColor.value.g(d);
        break;

    case 2:
        thisColor.value.b(d);
        break;

    default:
        assert(false);
    }
    sfdata.changed = true;
    return JS_TRUE;
}

JSBool SFColor::setHSV(JSContext * const cx,
                       JSObject * const obj,
                       const uintN argc,
                       jsval * const argv,
                       jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    sfield::sfdata & sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfcolor *>(&sfdata.field_value()));
    openvrml::sfcolor & thisColor =
        static_cast<openvrml::sfcolor &>(sfdata.field_value());

    jsdouble h, s, v;

    if (!JSVAL_IS_NUMBER(argv[0])
            || !JS_ValueToNumber(cx, argv[0], &h)
            || !JSVAL_IS_NUMBER(argv[1])
            || !JS_ValueToNumber(cx, argv[1], &s)
            || !JSVAL_IS_NUMBER(argv[2])
            || !JS_ValueToNumber(cx, argv[2], &v)) {
        return JS_FALSE;
    }

    thisColor.value.hsv(h, s, v);
    *rval = JSVAL_VOID;
    sfdata.changed = true;
    return JS_TRUE;
}

JSBool SFColor::getHSV(JSContext * const cx,
                       JSObject * const obj,
                       uintN,
                       jsval *,
                       jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfcolor *>(&sfdata.field_value()));
    const openvrml::sfcolor & thisColor =
        static_cast<openvrml::sfcolor &>(sfdata.field_value());

    float hsv[3];
    thisColor.value.hsv(hsv);

    return floatsToJSArray(3, hsv, cx, rval);
}


JSClass SFImage::jsclass =
        { "SFImage", JSCLASS_HAS_PRIVATE,
          JS_PropertyStub, JS_PropertyStub, getProperty, setProperty,
          JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize };

JSObject * SFImage::initClass(JSContext * const cx, JSObject * const obj)
    throw ()
{
    //
    // The properties are read-only for now; this can be made smarter
    // once the underlying openvrml SFImage type uses an integer array
    // to represent the pixel data (rather than a char array).
    //
    static JSPropertySpec properties[] =
            { { "x", 0, JSPROP_PERMANENT | JSPROP_READONLY, 0, 0 },
              { "y", 1, JSPROP_PERMANENT | JSPROP_READONLY, 0, 0 },
              { "comp", 2, JSPROP_PERMANENT | JSPROP_READONLY, 0, 0 },
              { "array", 3, JSPROP_PERMANENT |JSPROP_READONLY, 0, 0 },
              { 0, 0, 0, 0, 0 } };

    static JSFunctionSpec methods[] =
            // name, func ptr, argc
            { { "toString", toString, 0, 0, 0 },
              { 0, 0, 0, 0, 0 } };

    JSObject * const proto = JS_InitClass(cx, obj, 0, &jsclass,
                                          construct, 3, // constructor function, min arg count
                                          properties, methods,
                                          0, 0); // static properties and methods
    jsval args[] = { INT_TO_JSVAL(0), INT_TO_JSVAL(0), INT_TO_JSVAL(0) };
    if (!proto || !initObject(cx, proto, 3, args)) { return 0; }
    return proto;
}

JSBool SFImage::toJsval(const openvrml::sfimage & sfimage,
                        JSContext * const cx, JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    //
    // Can't call JS_ConstructObject() here since SFImage's ctor
    // requires 4 arguments.
    //
    JSObject * const sfimageObj = JS_NewObject(cx, &jsclass, 0, obj);
    if (!sfimageObj) { return JS_FALSE; }

    try {
        std::auto_ptr<openvrml::sfimage>
                sfimageClone(static_cast<openvrml::sfimage *>
                    (sfimage.clone().release()));
        std::auto_ptr<sfield::sfdata>
            sfdata(new sfield::sfdata(sfimageClone.get()));
        sfimageClone.release();
        if (!JS_SetPrivate(cx, sfimageObj, sfdata.get())) { return JS_FALSE; }
        sfdata.release();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        return JS_FALSE;
    } catch (...) {
        assert(false);
        return JS_FALSE;
    }
    *rval = OBJECT_TO_JSVAL(sfimageObj);
    return JS_TRUE;
}

std::auto_ptr<openvrml::sfimage>
SFImage::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (bad_conversion, std::bad_alloc)
{
    using std::auto_ptr;

    if (!JS_InstanceOf(cx, obj, &SFImage::jsclass, 0)) {
        throw bad_conversion("SFImage object expected.");
    }
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfimage *>(&sfdata.field_value()));
    return auto_ptr<openvrml::sfimage>(
        static_cast<openvrml::sfimage *>(
            sfdata.field_value().clone().release()));
}

JSBool SFImage::construct(JSContext * const cx,
                          JSObject * obj,
	                  const uintN argc,
                          jsval * const argv,
                          jsval * const rval)
    throw ()
{
    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return initObject(cx, obj, argc, argv);
}

//
// This is ugly because the spec requires an MFInt array for the pixel data
// and because giving write access to the image size parameters can cause
// the library code to crash unless they are validated somehow...
//
JSBool SFImage::initObject(JSContext * const cx,
                           JSObject * const obj,
                           const uintN argc,
                           jsval * const argv)
    throw ()
{
    //
    // x dimension.
    //
    uint32 x;
    if (!JS_ValueToECMAUint32(cx, argv[0], &x)) { return JS_FALSE; }

    //
    // y dimension.
    //
    uint32 y;
    if (!JS_ValueToECMAUint32(cx, argv[1], &y)) { return JS_FALSE; }

    //
    // components
    //
    uint32 comp;
    if (!JS_ValueToECMAUint32(cx, argv[2], &comp)) { return JS_FALSE; }

    try {
        using std::auto_ptr;

        //
        // pixel data array
        //
        typedef std::vector<unsigned char> pixels_t;

        pixels_t pixels(x * y * comp, 0); // Throws std::bad_alloc.

        if (argc > 3) {
            JSObject * arg3_obj;
            if (!JS_ValueToObject(cx, argv[3], &arg3_obj)) { return JS_FALSE; }
            if (!JS_InstanceOf(cx, arg3_obj, &MFInt32::jsclass, argv)) {
                return JS_FALSE;
            }

            MField::MFData * const mfdata =
                static_cast<MField::MFData *>(JS_GetPrivate(cx, arg3_obj));

            pixels_t::iterator pixelPtr = pixels.begin();
            for (MField::JsvalArray::size_type i(0);
                 i < mfdata->array.size() && i < x * y;
                 ++i, pixelPtr += comp) {
                assert(JSVAL_IS_INT(mfdata->array[i]));
                static const long byteMask[] =
                        { 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000 };
                ::int32 pixel;
                if (!JS_ValueToInt32(cx, mfdata->array[i], &pixel)) {
                    return JS_FALSE;
                }
                for (size_t j(0); j < comp; ++j) {
                    *(pixelPtr + j) = (pixel & byteMask[j]) >> (8 * j);
                }
            }
        }

        auto_ptr<openvrml::sfimage>
            sfimage(new openvrml::sfimage(image(x, y, comp, pixels)));
        auto_ptr<sfield::sfdata> sfdata(new sfield::sfdata(sfimage.get()));
        sfimage.release();
        if (!JS_SetPrivate(cx, obj, sfdata.get())) { return JS_FALSE; }
        sfdata.release();
    } catch (std::bad_alloc & ex) {
        JS_ReportOutOfMemory(cx);
        return JS_FALSE;
    }

    return JS_TRUE;
}

/**
 * @brief JavaScript SFImage getter
 *
 * @todo Return the pixel array as an MFInt32.
 */
JSBool SFImage::getProperty(JSContext * const cx,
                            JSObject * const obj,
                            const jsval id,
                            jsval * const vp)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfimage *>(&sfdata.field_value()));
    const openvrml::sfimage & thisImage =
            static_cast<openvrml::sfimage &>(sfdata.field_value());

    if (JSVAL_IS_INT(id)) {
        switch (JSVAL_TO_INT(id)) {
        case 0:
            *vp = INT_TO_JSVAL(thisImage.value.x());
            break;
        case 1:
            *vp = INT_TO_JSVAL(thisImage.value.y());
            break;
        case 2:
            *vp = INT_TO_JSVAL(thisImage.value.comp());
            break;
        case 3: // *vp = convert pixels to MFInt...
        default: return JS_FALSE;
        }
    }
    return JS_TRUE;
}


/**
 * @brief JavaScript SFImage getter
 *
 * @todo Implement me!
 */
JSBool SFImage::setProperty(JSContext *, JSObject *, jsval , jsval *)
    throw ()
{
  // ...
  return JS_FALSE;
}

JSClass SFNode::jsclass = {
    "SFNode",
    JSCLASS_HAS_PRIVATE,
    JS_PropertyStub,
    JS_PropertyStub,
    getProperty,
    JS_PropertyStub,
    JS_EnumerateStub,
    JS_ResolveStub,
    JS_ConvertStub,
    finalize
};

JSClass SFNode::direct_output_jsclass = {
    "SFNode",
    JSCLASS_HAS_PRIVATE,
    JS_PropertyStub,
    JS_PropertyStub,
    getProperty,
    setProperty,
    JS_EnumerateStub,
    JS_ResolveStub,
    JS_ConvertStub,
    finalize
};

JSObject * SFNode::initClass(JSContext * const cx,
                             JSObject * const obj)
    throw ()
{
    static JSFunctionSpec methods[] =
            { { "toString", SFNode::toString, 0, 0, 0 },
              { 0, 0, 0, 0, 0 } };

    js_::script & script =
        *static_cast<js_::script *>(JS_GetContextPrivate(cx));
    JSClass & jsclass = script.sfnode_class;

    jsval arg = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, "Group {}"));
    JSObject * const proto =
        JS_InitClass(cx, obj, 0, &jsclass,
                     SFNode::construct, 1, // constructor function, min arg count
                     0, methods, // instance properties, methods
                     0, 0); // static properties and methods
    if (!proto || !initObject(cx, proto, 1, &arg)) { return 0; }
    return proto;
}

JSBool SFNode::toJsval(const node_ptr & node,
                       JSContext * const cx,
                       JSObject * const obj,
                       jsval * const rval)
    throw ()
{
    js_::script & script =
        *static_cast<js_::script *>(JS_GetContextPrivate(cx));
    JSClass & jsclass = script.sfnode_class;
    //
    // The SFNode constructor requires one arg (a vrmlstring),
    // so we can't use JS_ConstructObject here.
    //
    JSObject * const sfnodeObj = JS_NewObject(cx, &jsclass, 0, obj);
    if (!sfnodeObj) { return JS_FALSE; }

    try {
        using std::auto_ptr;

        auto_ptr<openvrml::sfnode> sfnodeClone(new openvrml::sfnode(node));
        auto_ptr<sfield::sfdata> sfdata(new sfield::sfdata(sfnodeClone.get()));
        sfnodeClone.release();
        if (!JS_SetPrivate(cx, sfnodeObj, sfdata.get())) { return JS_FALSE; }
        sfdata.release();
    } catch (std::bad_alloc &) {
        JS_ReportOutOfMemory(cx);
        return JS_FALSE;
    }
    *rval = OBJECT_TO_JSVAL(sfnodeObj);
    return JS_TRUE;
}

std::auto_ptr<openvrml::sfnode>
SFNode::createFromJSObject(JSContext * const cx,
                           JSObject * const obj)
    throw (bad_conversion, std::bad_alloc)
{
    using std::auto_ptr;
    js_::script & script =
        *static_cast<js_::script *>(JS_GetContextPrivate(cx));
    JSClass & sfnode_jsclass = script.sfnode_class;

    if (!JS_InstanceOf(cx, obj, &sfnode_jsclass, 0)) {
        throw bad_conversion("SFNode object expected.");
    }
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfnode *>(&sfdata.field_value()));
    return auto_ptr<openvrml::sfnode>
            (static_cast<openvrml::sfnode *>
                (sfdata.field_value().clone().release()));
}

JSBool SFNode::construct(JSContext * const cx,
                         JSObject * obj,
                         const uintN argc,
                         jsval * const argv,
                         jsval * rval)
    throw ()
{
    js_::script & script =
        *static_cast<js_::script *>(JS_GetContextPrivate(cx));
    JSClass & jsclass = script.sfnode_class;
    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return initObject(cx, obj, argc, argv);
}

JSBool SFNode::initObject(JSContext * const cx,
                          JSObject * const obj,
                          const uintN argc,
                          jsval * const argv)
    throw ()
{
    using std::istringstream;
    assert(argc >= 1);

    assert(JS_GetContextPrivate(cx));
    js_::script & script =
        *static_cast<js_::script *>(JS_GetContextPrivate(cx));

    //
    // Make sure our argument is a string.
    //
    JSString * const str = JS_ValueToString(cx, argv[0]);
    if (!str) { return JS_FALSE; }

    istringstream in(JS_GetStringBytes(str));

    assert(script.script_node().scene());
    openvrml::browser & browser = script.script_node().scene()->browser;
    std::vector<node_ptr> nodes;
    try {
        nodes = browser.create_vrml_from_stream(in);
    } catch (invalid_vrml & ex) {
        JS_ReportError(cx, ex.what());
        return JS_FALSE;
    } catch (std::bad_alloc &) {
        JS_ReportOutOfMemory(cx);
        return JS_FALSE;
    }

    //
    // Fail if the string does not produce exactly one node.
    //
    if (nodes.size() != 1) { return JS_FALSE; }

    try {
        using std::auto_ptr;
        auto_ptr<openvrml::sfnode> sfnode(new openvrml::sfnode(nodes[0]));
        auto_ptr<sfield::sfdata> sfdata(new sfield::sfdata(sfnode.get()));
        sfnode.release();
        if (!JS_SetPrivate(cx, obj, sfdata.get())) { return JS_FALSE; }
        sfdata.release();
    } catch (std::bad_alloc &) {
        JS_ReportOutOfMemory(cx);
        return JS_FALSE;
    }
    return JS_TRUE;
}

//
// SFNode getProperty reads eventOut values, setProperty sends eventIns.
//
JSBool SFNode::getProperty(JSContext * const cx,
                           JSObject * const obj,
                           const jsval id,
                           jsval * const vp)
    throw ()
{
    if (JSVAL_IS_STRING(id)) {
        assert(JS_GetPrivate(cx, obj));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
        assert(dynamic_cast<openvrml::sfnode *>(&sfdata.field_value()));
        const openvrml::sfnode & thisNode =
            static_cast<openvrml::sfnode &>(sfdata.field_value());

        if (!thisNode.value) { return JS_TRUE; }

        assert(JS_GetContextPrivate(cx));
        js_::script & script =
            *static_cast<js_::script *>(JS_GetContextPrivate(cx));

        try {
            const char * eventOut = JS_GetStringBytes(JSVAL_TO_STRING(id));
            event_emitter & emitter = thisNode.value->event_emitter(eventOut);
            *vp = script.vrmlFieldToJSVal(emitter.value());
        } catch (unsupported_interface & ex) {}
    }
    return JS_TRUE;
}

// This should only happen if directOutput is set...

JSBool SFNode::setProperty(JSContext * const cx,
                           JSObject * const obj,
                           const jsval id,
                           jsval * const vp)
    throw ()
{
    if (JSVAL_IS_STRING(id)) {
        using std::auto_ptr;
        using std::string;

        assert(JS_GetPrivate(cx, obj));
        sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
        assert(dynamic_cast<openvrml::sfnode *>(&sfdata.field_value()));
        openvrml::sfnode & thisNode =
            static_cast<openvrml::sfnode &>(sfdata.field_value());

        if (!thisNode.value) { return JS_TRUE; }

        node_ptr nodePtr = thisNode.value;

        const char * const eventInId = JS_GetStringBytes(JSVAL_TO_STRING(id));

        try {
            //
            // Get the event_listener.
            //
            event_listener & listener = nodePtr->event_listener(eventInId);

            // convert vp to field, send eventIn to node
            field_value::type_id expectType = listener.type();
            auto_ptr<field_value> fieldValue;
            fieldValue = createFieldValueFromJsval(cx, *vp, expectType);
            
            assert(JS_GetContextPrivate(cx));
            js_::script & script =
                *static_cast<js_::script *>(JS_GetContextPrivate(cx));
            script.direct_output(listener,
                                 boost::shared_ptr<field_value>(fieldValue));
        } catch (unsupported_interface & ex) {
            // We can't handle the property here, so get out of the way.
            return JS_TRUE;
        } catch (bad_conversion & ex) {
            JS_ReportError(cx, ex.what());
            return JS_FALSE;
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }
    }
    return JS_TRUE;
}


JSClass SFRotation::jsclass = {
    "SFRotation",
    JSCLASS_HAS_PRIVATE,
    JS_PropertyStub,
    JS_PropertyStub,
    getProperty,
    setProperty,
    JS_EnumerateStub,
    JS_ResolveStub,
    JS_ConvertStub,
    finalize
};

JSObject * SFRotation::initClass(JSContext * const cx,
                                 JSObject * const obj)
    throw ()
{
    static JSPropertySpec properties[] =
            { { "x", 0, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { "y", 1, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { "z", 2, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { "angle", 3, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { 0, 0, 0, 0, 0 } };

    static JSFunctionSpec methods[] =
            // name, func ptr, argc
            { { "getAxis", getAxis, 0, 0, 0 },
              { "inverse", inverse, 0, 0, 0 },
              { "multiply", multiply, 1, 0, 0 },
              { "multVec", multVec, 1, 0, 0 },
              { "setAxis", setAxis, 1, 0, 0 },
              { "slerp", slerp, 2, 0, 0 },
              { "toString", toString, 0, 0, 0 },
              { 0, 0, 0, 0, 0 } };

    JSObject * const proto =
        JS_InitClass(cx, obj, 0, &jsclass,
                     construct, 0, // constructor function, min arg count
                     properties, methods, // instance properties, methods
                     0, 0); // static properties and methods
    if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
    return proto;
}

JSBool SFRotation::toJsval(const rotation & rotation,
                           JSContext * const cx,
                           JSObject * const obj,
                           jsval * const rval)
    throw ()
{
    JSObject * const sfrotationObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!sfrotationObj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, sfrotationObj));
    const sfield::sfdata & sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, sfrotationObj));
    assert(dynamic_cast<openvrml::sfrotation *>(&sfdata.field_value()));

    //
    // Assignment does not throw.
    //
    static_cast<openvrml::sfrotation &>(sfdata.field_value()).value = rotation;
    *rval = OBJECT_TO_JSVAL(sfrotationObj);
    return JS_TRUE;
}

std::auto_ptr<openvrml::sfrotation>
SFRotation::createFromJSObject(JSContext * const cx,
                               JSObject * const obj)
    throw (bad_conversion, std::bad_alloc)
{
    using std::auto_ptr;

    if (!JS_InstanceOf(cx, obj, &SFRotation::jsclass, 0)) {
        throw bad_conversion("SFRotation object expected.");
    }
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfrotation *>(&sfdata.field_value()));
    return auto_ptr<openvrml::sfrotation>(
        static_cast<openvrml::sfrotation *>(
            sfdata.field_value().clone().release()));
}

JSBool SFRotation::construct(JSContext * const cx,
                             JSObject * obj,
                             const uintN argc,
                             jsval * const argv,
                             jsval * const rval)
    throw ()
{
    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return initObject(cx, obj, argc, argv);
}

JSBool SFRotation::initObject(JSContext * const cx,
                              JSObject * const obj,
                              const uintN argc,
                              jsval * const argv)
    throw ()
{
    using std::auto_ptr;

    jsdouble x = 0.0, y = 1.0, z = 0.0, angle = 0.0;
    if (argc > 0) {
        if (JSVAL_IS_NUMBER(argv[0])) {
	    if (!JS_ValueToNumber(cx, argv[0], &x)) { return JS_FALSE; }
	    if (argc > 1) {
                if (!JSVAL_IS_NUMBER(argv[1])
                        || !JS_ValueToNumber(cx, argv[1], &y)) {
                    return JS_FALSE;
                }

	        if (argc > 2) {
                    if (!JSVAL_IS_NUMBER(argv[2])
                            || !JS_ValueToNumber(cx, argv[2], &z)) {
                        return JS_FALSE;
                    }
                    if (argc > 3) {
                        if (!JSVAL_IS_NUMBER(argv[3])
                                || !JS_ValueToNumber(cx, argv[3], &angle)) {
                            return JS_FALSE;
                        }
                    }
                }
            }
        } else if (JSVAL_IS_OBJECT(argv[0])) {
            if (!JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                               &js_::SFVec3f::jsclass, argv)) {
                return JS_FALSE;
            }
            assert(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
            const sfield::sfdata & sfdata =
                *static_cast<sfield::sfdata *>(
                    JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
            assert(dynamic_cast<openvrml::sfvec3f *>(&sfdata.field_value()));
            const openvrml::sfvec3f & argVec1 =
                static_cast<openvrml::sfvec3f &>(sfdata.field_value());

            if (argc > 1) {
                if (JSVAL_IS_OBJECT(argv[1])) {
                    if (!JS_InstanceOf(cx,
                                       JSVAL_TO_OBJECT(argv[1]),
                                       &js_::SFVec3f::jsclass,
                                       argv)) {
                        return JS_FALSE;
                    }
                    assert(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[1])));
                    const sfield::sfdata & sfdata =
                        *static_cast<sfield::sfdata *>(
                            JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[1])));
                    assert(dynamic_cast<openvrml::sfvec3f *>(
                               &sfdata.field_value()));
                    const openvrml::sfvec3f & argVec2 =
                        static_cast<openvrml::sfvec3f &>(sfdata.field_value());

                    vec3f axisVec =
                        (argVec1.value * argVec2.value).normalize();
                    x = axisVec.x();
                    y = axisVec.y();
                    z = axisVec.z();
                    angle = acos(argVec1.value.dot(argVec2.value)
                                 / (argVec1.value.length()
                                    * argVec2.value.length()));

                } else if (JSVAL_IS_NUMBER(argv[1])) {
                    if (!JS_ValueToNumber(cx, argv[1], &angle)) {
                        return JS_FALSE;
                    }
                    x = argVec1.value.x();
                    y = argVec1.value.y();
                    z = argVec1.value.z();
                } else {
                    return JS_FALSE;
                }
            }
        } else {
            return JS_FALSE;
        }
    }

    try {
        auto_ptr<openvrml::sfrotation>
            sfrotation(new openvrml::sfrotation(rotation(x, y, z, angle)));
        auto_ptr<sfield::sfdata> sfdata(new sfield::sfdata(sfrotation.get()));
        sfrotation.release();
        if (!JS_SetPrivate(cx, obj, sfdata.get())) { return JS_FALSE; }
        sfdata.release();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        return JS_FALSE;
    } catch (...) {
        assert(false);
        return JS_FALSE;
    }
    return JS_TRUE;
}

JSBool SFRotation::getProperty(JSContext * const cx,
                               JSObject * const obj,
                               const jsval id,
                               jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfrotation *>(&sfdata.field_value()));
    const openvrml::sfrotation & thisRot =
        static_cast<openvrml::sfrotation &>(sfdata.field_value());

    if (JSVAL_IS_INT(id)
        && JSVAL_TO_INT(id) >= 0 && JSVAL_TO_INT(id) < 4) {
        if (!JS_NewDoubleValue(cx, thisRot.value[JSVAL_TO_INT(id)], rval)) {
            return JS_FALSE;
        }
    }
    return JS_TRUE;
}

JSBool SFRotation::setProperty(JSContext * const cx,
                               JSObject * const obj,
                               const jsval id,
                               jsval * const vp)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    sfield::sfdata & sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfrotation *>(&sfdata.field_value()));
    openvrml::sfrotation & thisRot =
        static_cast<openvrml::sfrotation &>(sfdata.field_value());

    if (JSVAL_IS_INT(id)
        && JSVAL_TO_INT(id) >= 0 && JSVAL_TO_INT(id) < 4) {

        jsdouble d;
        if (!JS_ValueToNumber(cx, *vp, &d)) { return JS_FALSE; }

        switch (JSVAL_TO_INT(id)) {
        case 0:
            thisRot.value.x(d);
            break;

        case 1:
            thisRot.value.y(d);
            break;

        case 2:
            thisRot.value.z(d);
            break;

        case 3:
            thisRot.value.angle(d);
            break;

        default:
            assert(false);
        }
        sfdata.changed = true;
    }
    return JS_TRUE;
}

JSBool SFRotation::getAxis(JSContext * const cx,
                           JSObject * const obj,
                           uintN,
                           jsval *,
                           jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & obj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfrotation *>(&obj_sfdata.field_value()));
    const openvrml::sfrotation & thisRot =
        static_cast<openvrml::sfrotation &>(obj_sfdata.field_value());

    JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0, obj);
    if (!robj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, robj));
    const sfield::sfdata & robj_sfdata =
        *static_cast<sfdata *>(JS_GetPrivate(cx, robj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&robj_sfdata.field_value()));
    openvrml::sfvec3f & resultVec =
        static_cast<openvrml::sfvec3f &>(robj_sfdata.field_value());

    resultVec.value = thisRot.value.axis(); // Does not throw.
    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFRotation::inverse(JSContext * const cx,
                           JSObject * const obj,
                           uintN,
                           jsval *,
                           jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & obj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfrotation *>(&obj_sfdata.field_value()));
    const openvrml::sfrotation & thisRot =
            static_cast<openvrml::sfrotation &>(obj_sfdata.field_value());

    static JSObject * const proto = 0;
    JSObject * const parent = JS_GetParent(cx, obj);
    JSObject * const robj =
        JS_ConstructObject(cx, &SFRotation::jsclass, proto, parent);
    if (!robj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, robj));
    const sfield::sfdata & robj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
    assert(dynamic_cast<openvrml::sfrotation *>(&robj_sfdata.field_value()));
    openvrml::sfrotation & resultRot =
        static_cast<openvrml::sfrotation &>(robj_sfdata.field_value());

    resultRot.value = thisRot.value.inverse(); // Does not throw.
    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFRotation::multiply(JSContext * const cx,
                            JSObject * const obj,
                            const uintN argc,
                            jsval * const argv,
                            jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & obj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfrotation *>(&obj_sfdata.field_value()));
    const openvrml::sfrotation & thisRot =
            static_cast<openvrml::sfrotation &>(obj_sfdata.field_value());

    //
    // Make sure our argument is an SFRotation.
    //
    if (!JSVAL_IS_OBJECT(argv[0])
            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                              &SFRotation::jsclass, argv)) {
        return JS_FALSE;
    }

    assert(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
    const sfield::sfdata & arg_sfdata =
        *static_cast<sfdata *>(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
    assert(dynamic_cast<openvrml::sfrotation *>(&arg_sfdata.field_value()));
    const openvrml::sfrotation & argRot =
            static_cast<openvrml::sfrotation &>(arg_sfdata.field_value());

    //
    // Construct the result object.
    //
    static JSObject * const proto = 0;
    JSObject * const parent = JS_GetParent(cx, obj);
    JSObject * const robj =
        JS_ConstructObject(cx, &SFRotation::jsclass, proto, parent);
    if (!robj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, robj));
    const sfield::sfdata & robj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
    assert(dynamic_cast<openvrml::sfrotation *>(&robj_sfdata.field_value()));
    openvrml::sfrotation & resultRot =
        static_cast<openvrml::sfrotation &>(robj_sfdata.field_value());

    resultRot.value = thisRot.value * argRot.value; // Does not throw.
    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFRotation::multVec(JSContext * const cx,
                           JSObject * const obj,
                           const uintN argc,
                           jsval * const argv,
	                   jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & obj_sfdata =
        *static_cast<sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfrotation *>(&obj_sfdata.field_value()));
    const openvrml::sfrotation & thisRot =
        static_cast<openvrml::sfrotation &>(obj_sfdata.field_value());

    //
    // Make sure our argument is an SFVec3f.
    //
    if (!JSVAL_IS_OBJECT(argv[0])
            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                              &SFVec3f::jsclass, argv)) {
        return JS_FALSE;
    }

    assert(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
    const sfield::sfdata & arg_sfdata =
        *static_cast<sfdata *>(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
    assert(dynamic_cast<openvrml::sfvec3f *>(&arg_sfdata.field_value()));
    const openvrml::sfvec3f & argVec =
        static_cast<openvrml::sfvec3f &>(arg_sfdata.field_value());

    //
    // Construct the result object.
    //
    static JSObject * const proto = 0;
    JSObject * const parent = JS_GetParent(cx, obj);
    JSObject * const robj =
        JS_ConstructObject(cx, &SFVec3f::jsclass, proto, parent);
    if (!robj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, robj));
    sfield::sfdata & robj_sfdata =
        *static_cast<sfdata *>(JS_GetPrivate(cx, robj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&robj_sfdata.field_value()));
    openvrml::sfvec3f & resultVec =
        static_cast<openvrml::sfvec3f &>(robj_sfdata.field_value());

    //
    // Does not throw.
    //
    resultVec.value = argVec.value * mat4f::rotation(thisRot.value);

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFRotation::setAxis(JSContext * const cx,
                           JSObject * const obj,
                           const uintN argc,
                           jsval * const argv,
                           jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    sfield::sfdata & obj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfrotation *>(&obj_sfdata.field_value()));
    openvrml::sfrotation & thisRot =
        static_cast<openvrml::sfrotation &>(obj_sfdata.field_value());

    //
    // Make sure our argument is an SFVec3f.
    //
    JSObject * arg_obj;
    if (!JS_ValueToObject(cx, argv[0], &arg_obj)) { return JS_FALSE; }
    if (!JS_InstanceOf(cx, arg_obj, &SFVec3f::jsclass, argv)) {
        return JS_FALSE;
    }

    assert(JS_GetPrivate(cx, arg_obj));
    const sfield::sfdata & arg_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, arg_obj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&arg_sfdata.field_value()));
    const openvrml::sfvec3f & argVec =
        static_cast<openvrml::sfvec3f &>(arg_sfdata.field_value());

    thisRot.value.axis(argVec.value); // Does not throw.
    obj_sfdata.changed = true;
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

JSBool SFRotation::slerp(JSContext * const cx,
                         JSObject * const obj,
                         const uintN argc,
                         jsval * const argv,
                         jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & obj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfrotation *>(&obj_sfdata.field_value()));
    const openvrml::sfrotation & thisRot =
        static_cast<openvrml::sfrotation &>(obj_sfdata.field_value());

    //
    // Make sure our first argument is an SFRotation.
    //
    JSObject * arg0_obj;
    if (!JS_ValueToObject(cx, argv[0], &arg0_obj)) { return JS_FALSE; }
    if (!JS_InstanceOf(cx, arg0_obj, &SFRotation::jsclass, argv)) {
        return JS_FALSE;
    }

    assert(JS_GetPrivate(cx, arg0_obj));
    const sfield::sfdata & arg0_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, arg0_obj));
    assert(dynamic_cast<openvrml::sfrotation *>(&arg0_sfdata.field_value()));
    const openvrml::sfrotation & argRot =
        static_cast<openvrml::sfrotation &>(arg0_sfdata.field_value());

    //
    // Make sure our second argument is a number.
    //
    jsdouble factor;
    if (!JS_ValueToNumber(cx, argv[1], &factor)) { return JS_FALSE; }

    //
    // Construct the result object.
    //
    static JSObject * const proto = 0;
    JSObject * const parent = JS_GetParent(cx, obj);
    JSObject * const robj =
        JS_ConstructObject(cx, &SFRotation::jsclass, proto, parent);
    if (!robj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, obj));
    sfield::sfdata & robj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
    assert(dynamic_cast<openvrml::sfrotation *>(&robj_sfdata.field_value()));
    openvrml::sfrotation & resultRot =
            static_cast<openvrml::sfrotation &>(robj_sfdata.field_value());

    resultRot.value = thisRot.value.slerp(argRot.value, factor);

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}


JSClass SFVec2f::jsclass = {
    "SFVec2f",
    JSCLASS_HAS_PRIVATE,
    JS_PropertyStub,
    JS_PropertyStub,
    getProperty,
    setProperty,
    JS_EnumerateStub,
    JS_ResolveStub,
    JS_ConvertStub,
    finalize
};

JSObject * SFVec2f::initClass(JSContext * const cx, JSObject * const obj)
    throw ()
{
    static JSPropertySpec properties[] =
            { { "x", 0, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { "y", 1, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { 0, 0, 0, 0, 0 } };

    static JSFunctionSpec methods[] =
      /*    name          native          nargs    */
            { { "add", add, 1, 0, 0 },
              { "divide", divide, 1, 0, 0 },
              { "dot", dot, 1, 0, 0 },
              { "length", length, 0, 0, 0 },
              { "multiply", multiply, 1, 0, 0 },
              { "negate", negate, 0, 0, 0 },
              { "normalize", normalize, 0, 0, 0 },
              { "subtract", subtract, 1, 0, 0 },
              { "toString", toString, 0, 0, 0 },
              { 0, 0, 0, 0, 0 } };

    JSObject * const proto =
        JS_InitClass(cx, obj, 0, &jsclass,
                     constructor, 0, // constructor function, min arg count
                     properties, methods,
                     0, 0); // static properties and methods
    if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
    return proto;
}

JSBool SFVec2f::toJsval(const vec2f & vec2f,
                        JSContext * const cx,
                        JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    JSObject * const sfvec2fObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!sfvec2fObj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, sfvec2fObj));
    const sfield::sfdata & sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, sfvec2fObj));
    assert(dynamic_cast<openvrml::sfvec2f *>(&sfdata.field_value()));

    //
    // Assignment should not throw.
    //
    static_cast<openvrml::sfvec2f &>(sfdata.field_value()).value = vec2f;
    *rval = OBJECT_TO_JSVAL(sfvec2fObj);
    return JS_TRUE;
}

std::auto_ptr<openvrml::sfvec2f>
SFVec2f::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (bad_conversion, std::bad_alloc)
{
    using std::auto_ptr;

    if (!JS_InstanceOf(cx, obj, &SFVec2f::jsclass, 0)) {
        throw bad_conversion("SFVec2f object expected.");
    }
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfvec2f *>(&sfdata.field_value()));
    return auto_ptr<openvrml::sfvec2f>(
        static_cast<openvrml::sfvec2f *>(
            sfdata.field_value().clone().release()));
}

JSBool SFVec2f::constructor(JSContext * const cx,
                            JSObject * obj,
                            const uintN argc,
                            jsval * const argv,
                            jsval * const rval)
    throw ()
{
    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return initObject(cx, obj, argc, argv);
}

JSBool SFVec2f::initObject(JSContext * const cx,
                           JSObject * const obj,
                           const uintN argc,
                           jsval * const argv)
    throw ()
{
    jsdouble vec[] = { 0.0, 0.0 };
    for (uintN i(0); i < ((argc < 2) ? argc : 2); ++i) {
        if (!JS_ValueToNumber(cx, argv[i], vec + i)) { return JS_FALSE; }
        if (vec[i] != vec[i]) {
            JS_ReportError(cx, "Argument %d of SFVec2f constructor is NaN.",
                           i + 1);
            return JS_FALSE;
        }
    }

    try {
        using std::auto_ptr;

        auto_ptr<openvrml::sfvec2f>
            sfvec2f(new openvrml::sfvec2f(vec2f(vec[0], vec[1])));
        auto_ptr<sfield::sfdata> sfdata(new sfield::sfdata(sfvec2f.get()));
        sfvec2f.release();
        if (!JS_SetPrivate(cx, obj, sfdata.get())) { return JS_FALSE; }
        sfdata.release();
    } catch (std::bad_alloc &) {
        JS_ReportOutOfMemory(cx);
        return JS_FALSE;
    }

    return JS_TRUE;
}

JSBool SFVec2f::getProperty(JSContext * const cx,
                            JSObject * const obj,
                            const jsval id,
                            jsval * const rval)
    throw ()
{
    if (JSVAL_IS_INT(id) && JSVAL_TO_INT(id) >= 0 && JSVAL_TO_INT(id) < 2) {
        assert(JS_GetPrivate(cx, obj));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
        assert(dynamic_cast<openvrml::sfvec2f *>(&sfdata.field_value()));
        const openvrml::sfvec2f & thisVec =
            static_cast<openvrml::sfvec2f &>(sfdata.field_value());

        if (!JS_NewDoubleValue(cx, thisVec.value[JSVAL_TO_INT(id)], rval)) {
            return JS_FALSE;
        }
    }
    return JS_TRUE;
}

JSBool SFVec2f::setProperty(JSContext * const cx,
                            JSObject * const obj,
                            const jsval id,
                            jsval * const vp)
    throw ()
{
    if (JSVAL_IS_INT(id) && JSVAL_TO_INT(id) >= 0 && JSVAL_TO_INT(id) < 2) {
        assert(JS_GetPrivate(cx, obj));
        sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
        assert(dynamic_cast<openvrml::sfvec2f *>(&sfdata.field_value()));
        openvrml::sfvec2f & thisVec =
            static_cast<openvrml::sfvec2f &>(sfdata.field_value());

        jsdouble d;
        if (!JS_ValueToNumber(cx, *vp, &d)) { return JS_FALSE; }
        if (d != d) {
            JS_ReportError(cx, "Cannot set SFVec2f component to NaN.");
            return JS_FALSE;
        }

        switch (JSVAL_TO_INT(id)) {
        case 0:
            thisVec.value.x(d);
            break;
        case 1:
            thisVec.value.y(d);
            break;
        default:
            assert(false);
        }
        sfdata.changed = true;
    }
    return JS_TRUE;
}

JSBool SFVec2f::add(JSContext * const cx,
                    JSObject * const obj,
                    const uintN argc,
                    jsval * const argv,
                    jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & obj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfvec2f *>(&obj_sfdata.field_value()));
    const openvrml::sfvec2f & thisVec =
        static_cast<openvrml::sfvec2f &>(obj_sfdata.field_value());

    //
    // Make sure our argument is an SFVec2f.
    //
    JSObject * arg_obj;
    if (!JS_ValueToObject(cx, argv[0], &arg_obj)) { return JS_FALSE; }
    if (!JS_InstanceOf(cx, arg_obj, &SFVec2f::jsclass, argv)) {
        return JS_FALSE;
    }

    assert(JS_GetPrivate(cx, arg_obj));
    const sfield::sfdata & arg_sfdata=
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, arg_obj));
    assert(dynamic_cast<openvrml::sfvec2f *>(&arg_sfdata.field_value()));
    const openvrml::sfvec2f & argVec =
            static_cast<openvrml::sfvec2f &>(arg_sfdata.field_value());

    //
    // Construct the result object.
    //
    static JSObject * const proto = 0;
    JSObject * const parent = JS_GetParent(cx, obj);
    JSObject * const robj =
        JS_ConstructObject(cx, &SFVec2f::jsclass, proto, parent);
    if (!robj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, robj));
    sfield::sfdata & robj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
    assert(dynamic_cast<openvrml::sfvec2f *>(&robj_sfdata.field_value()));
    openvrml::sfvec2f & resultVec =
        static_cast<openvrml::sfvec2f &>(robj_sfdata.field_value());

    resultVec.value = thisVec.value + argVec.value;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec2f::divide(JSContext * const cx,
                       JSObject * const obj,
                       const uintN argc,
                       jsval * const argv,
                       jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & obj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfvec2f *>(&obj_sfdata.field_value()));
    const openvrml::sfvec2f & thisVec =
        static_cast<openvrml::sfvec2f &>(obj_sfdata.field_value());

    //
    // Make sure our argument is a number.
    //
    jsdouble divisor;
    if (!JS_ValueToNumber(cx, argv[0], &divisor)) { return JS_FALSE; }

    //
    // Construct the result object.
    //
    static JSObject * const proto = 0;
    JSObject * const parent = JS_GetParent(cx, obj);
    JSObject * const robj =
        JS_ConstructObject(cx, &SFVec2f::jsclass, proto, parent);
    if (!robj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, robj));
    sfield::sfdata & robj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
    assert(dynamic_cast<openvrml::sfvec2f *>(&robj_sfdata.field_value()));
    openvrml::sfvec2f & resultVec =
        static_cast<openvrml::sfvec2f &>(robj_sfdata.field_value());

    resultVec.value = thisVec.value / divisor;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec2f::dot(JSContext * const cx,
                    JSObject * const obj,
                    const uintN argc,
                    jsval * const argv,
                    jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfvec2f *>(&sfdata.field_value()));
    const openvrml::sfvec2f & thisVec =
            static_cast<openvrml::sfvec2f &>(sfdata.field_value());

    //
    // Make sure our argument is an SFVec2f.
    //
    JSObject * arg_obj;
    if (!JS_ValueToObject(cx, argv[0], &arg_obj)) { return JS_FALSE; }
    if (!JS_InstanceOf(cx, arg_obj, &SFVec2f::jsclass, argv)) {
        return JS_FALSE;
    }

    assert(JS_GetPrivate(cx, arg_obj));
    const sfield::sfdata & arg_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, arg_obj));
    assert(dynamic_cast<openvrml::sfvec2f *>(&arg_sfdata.field_value()));
    const openvrml::sfvec2f & argVec =
            static_cast<openvrml::sfvec2f &>(arg_sfdata.field_value());

    const jsdouble result = thisVec.value.dot(argVec.value);
    if (!JS_NewDoubleValue(cx, result, rval)) { return JS_FALSE; }
    return JS_TRUE;
}

JSBool SFVec2f::length(JSContext * const cx,
                       JSObject * const obj,
                       uintN,
                       jsval *,
                       jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfvec2f *>(&sfdata.field_value()));
    const openvrml::sfvec2f & thisVec =
            static_cast<openvrml::sfvec2f &>(sfdata.field_value());

    if (!JS_NewDoubleValue(cx, thisVec.value.length(), rval)) {
        return JS_FALSE;
    }
    return JS_TRUE;
}

JSBool SFVec2f::multiply(JSContext * const cx,
                         JSObject * const obj,
                         const uintN argc,
                         jsval * const argv,
                         jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & obj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfvec2f *>(&obj_sfdata.field_value()));
    const openvrml::sfvec2f & thisVec =
            static_cast<openvrml::sfvec2f &>(obj_sfdata.field_value());

    //
    // Make sure our argument is a number.
    //
    jsdouble factor;
    if (!JS_ValueToNumber(cx, argv[0], &factor)) { return JS_FALSE; }

    //
    // Construct the result object.
    //
    static JSObject * const proto = 0;
    JSObject * const parent = JS_GetParent(cx, obj);
    JSObject * const robj =
        JS_ConstructObject(cx, &SFVec2f::jsclass, proto, parent);
    if (!robj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, robj));
    sfield::sfdata & robj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
    assert(dynamic_cast<openvrml::sfvec2f *>(&robj_sfdata.field_value()));
    openvrml::sfvec2f & resultVec =
        static_cast<openvrml::sfvec2f &>(robj_sfdata.field_value());

    resultVec.value = thisVec.value * factor;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec2f::negate(JSContext * const cx,
                       JSObject * const obj,
                       uintN,
                       jsval *,
                       jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & obj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfvec2f *>(&obj_sfdata.field_value()));
    const openvrml::sfvec2f & thisVec =
            static_cast<openvrml::sfvec2f &>(obj_sfdata.field_value());

    //
    // Construct the result object.
    //
    static JSObject * const proto = 0;
    JSObject * const parent = JS_GetParent(cx, obj);
    JSObject * const robj =
        JS_ConstructObject(cx, &SFVec2f::jsclass, proto, parent);
    if (!robj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, robj));
    sfield::sfdata & robj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
    assert(dynamic_cast<openvrml::sfvec2f *>(&robj_sfdata.field_value()));
    openvrml::sfvec2f & resultVec =
            static_cast<openvrml::sfvec2f &>(robj_sfdata.field_value());

    resultVec.value = -thisVec.value;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec2f::normalize(JSContext * const cx,
                          JSObject * const obj,
                          uintN,
                          jsval *,
                          jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & obj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfvec2f *>(&obj_sfdata.field_value()));
    const openvrml::sfvec2f & thisVec =
            static_cast<openvrml::sfvec2f &>(obj_sfdata.field_value());

    //
    // Construct the result object.
    //
    static JSObject * const proto = 0;
    JSObject * const parent = JS_GetParent(cx, obj);
    JSObject * const robj =
        JS_ConstructObject(cx, &SFVec2f::jsclass, proto, parent);
    if (!robj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, robj));
    sfield::sfdata & robj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
    assert(dynamic_cast<openvrml::sfvec2f *>(&robj_sfdata.field_value()));
    openvrml::sfvec2f & resultVec =
            static_cast<openvrml::sfvec2f &>(robj_sfdata.field_value());

    resultVec.value = thisVec.value.normalize();

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec2f::subtract(JSContext * const cx,
                         JSObject * const obj,
                         const uintN argc,
                         jsval * const argv,
                         jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & obj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfvec2f *>(&obj_sfdata.field_value()));
    const openvrml::sfvec2f & thisVec =
            static_cast<openvrml::sfvec2f &>(obj_sfdata.field_value());

    //
    // Make sure our argument is an SFVec2f.
    //
    JSObject * arg_obj;
    if (!JS_ValueToObject(cx, argv[0], &arg_obj)) { return JS_FALSE; }
    if (!JS_InstanceOf(cx, arg_obj, &SFVec2f::jsclass, argv)) {
        return JS_FALSE;
    }

    assert(JS_GetPrivate(cx, arg_obj));
    const sfield::sfdata & arg_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, arg_obj));
    assert(dynamic_cast<openvrml::sfvec2f *>(&arg_sfdata.field_value()));
    const openvrml::sfvec2f & argVec =
        static_cast<openvrml::sfvec2f &>(arg_sfdata.field_value());

    //
    // Construct the result object.
    //
    static JSObject * const proto = 0;
    JSObject * const parent = JS_GetParent(cx, obj);
    JSObject * const robj =
        JS_ConstructObject(cx, &SFVec2f::jsclass, proto, parent);
    if (!robj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, robj));
    sfield::sfdata & robj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
    assert(dynamic_cast<openvrml::sfvec2f *>(&robj_sfdata.field_value()));
    openvrml::sfvec2f & resultVec =
        static_cast<openvrml::sfvec2f &>(robj_sfdata.field_value());

    resultVec.value = thisVec.value - argVec.value;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSClass SFVec3f::jsclass = {
    "SFVec3f",
    JSCLASS_HAS_PRIVATE,
    JS_PropertyStub,
    JS_PropertyStub,
    getProperty,
    setProperty,
    JS_EnumerateStub,
    JS_ResolveStub,
    JS_ConvertStub,
    finalize
};

JSObject * SFVec3f::initClass(JSContext * const cx, JSObject * const obj)
    throw ()
{
    static JSPropertySpec properties[] =
            { { "x", 0, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { "y", 1, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { "z", 2, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { 0, 0, 0, 0, 0 } };

    static JSFunctionSpec methods[] =
      /*    name          native          nargs    */
            { { "add", add, 1, 0, 0 },
              { "cross", cross, 1, 0, 0 },
              { "divide", divide, 1, 0, 0 },
              { "dot", dot, 1, 0, 0 },
              { "length", length, 0, 0, 0 },
              { "multiply", multiply, 1, 0, 0 },
              { "negate", negate, 0, 0, 0 },
              { "normalize", normalize, 0, 0, 0 },
              { "subtract", subtract, 1, 0, 0 },
              { "toString", toString, 0, 0, 0 },
              { 0, 0, 0, 0, 0 } };

    JSObject * const proto =
        JS_InitClass(cx, obj, 0, &jsclass,
                     constructor, 0, // constructor function, min arg count
                     properties, methods,
                     0, 0); // static properties and methods
    if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
    return proto;
}

JSBool SFVec3f::toJsval(const vec3f & vec3f,
                        JSContext * const cx,
                        JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    JSObject * const sfvec3fObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!sfvec3fObj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, sfvec3fObj));
    const sfield::sfdata & sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, sfvec3fObj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&sfdata.field_value()));

    //
    // Assignment should not throw.
    //
    static_cast<openvrml::sfvec3f &>(sfdata.field_value()).value = vec3f;

    *rval = OBJECT_TO_JSVAL(sfvec3fObj);
    return JS_TRUE;
}

std::auto_ptr<openvrml::sfvec3f>
SFVec3f::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (bad_conversion, std::bad_alloc)
{
    using std::auto_ptr;

    if (!JS_InstanceOf(cx, obj, &SFVec3f::jsclass, 0)) {
        throw bad_conversion("SFVec3f object expected.");
    }
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&sfdata.field_value()));
    return auto_ptr<openvrml::sfvec3f>(
        static_cast<openvrml::sfvec3f *>(
            sfdata.field_value().clone().release()));
}

JSBool SFVec3f::constructor(JSContext * const cx,
                            JSObject * obj,
                            const uintN argc,
                            jsval * const argv,
                            jsval * const rval)
    throw ()
{
    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return initObject(cx, obj, argc, argv);
}

JSBool SFVec3f::initObject(JSContext * const cx,
                           JSObject * obj,
                           const uintN argc,
                           jsval * const argv)
    throw ()
{
    jsdouble vec[] = { 0.0, 0.0, 0.0 };
    for (uintN i(0); i < ((argc < 3) ? argc : 3); ++i) {
        if (!JS_ValueToNumber(cx, argv[i], vec + i)) { return JS_FALSE; }
        if (vec[i] != vec[i]) {
            JS_ReportError(cx, "Argument %d of SFVec3f constructor is NaN.",
                           i + 1);
            return JS_FALSE;
        }
    }

    try {
        using std::auto_ptr;

        auto_ptr<openvrml::sfvec3f>
            sfvec3f(new openvrml::sfvec3f(vec3f(vec[0], vec[1], vec[2])));
        auto_ptr<sfield::sfdata> sfdata(new sfield::sfdata(sfvec3f.get()));
        sfvec3f.release();
        if (!JS_SetPrivate(cx, obj, sfdata.get())) { return JS_FALSE; }
        sfdata.release();
    } catch (std::bad_alloc &) {
        JS_ReportOutOfMemory(cx);
        return JS_FALSE;
    }
    return JS_TRUE;
}

JSBool SFVec3f::getProperty(JSContext * const cx,
                            JSObject * const obj,
                            const jsval id,
                            jsval * const vp)
    throw ()
{
    if (JSVAL_IS_INT(id) && JSVAL_TO_INT(id) >= 0 && JSVAL_TO_INT(id) < 3) {
        assert(JS_GetPrivate(cx, obj));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
        assert(dynamic_cast<openvrml::sfvec3f *>(&sfdata.field_value()));
        const openvrml::sfvec3f & thisVec =
            static_cast<openvrml::sfvec3f &>(sfdata.field_value());

        if (!JS_NewDoubleValue(cx, thisVec.value[JSVAL_TO_INT(id)], vp)) {
            return JS_FALSE;
        }
    }
    return JS_TRUE;
}

JSBool SFVec3f::setProperty(JSContext * const cx,
                            JSObject * const obj,
                            const jsval id,
                            jsval * const vp)
    throw ()
{
    if (JSVAL_IS_INT(id) && JSVAL_TO_INT(id) >= 0 && JSVAL_TO_INT(id) < 3) {
        assert(JS_GetPrivate(cx, obj));
        sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
        assert(dynamic_cast<openvrml::sfvec3f *>(&sfdata.field_value()));
        openvrml::sfvec3f & thisVec =
            static_cast<openvrml::sfvec3f &>(sfdata.field_value());

        jsdouble d;
        if (!JS_ValueToNumber(cx, *vp, &d)) { return JS_FALSE; }
        if (d != d) {
            JS_ReportError(cx, "Cannot set SFVec3f component to NaN.");
            return JS_FALSE;
        }

        switch (JSVAL_TO_INT(id)) {
        case 0:
            thisVec.value.x(d);
            break;
        case 1:
            thisVec.value.y(d);
            break;
        case 2:
            thisVec.value.z(d);
            break;
        default:
            assert(false);
        }
        sfdata.changed = true;
    }
    return JS_TRUE;
}

JSBool SFVec3f::add(JSContext * const cx,
                    JSObject * const obj,
                    const uintN argc,
                    jsval * const argv,
                    jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & obj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&obj_sfdata.field_value()));
    const openvrml::sfvec3f & thisVec =
        static_cast<openvrml::sfvec3f &>(obj_sfdata.field_value());

    //
    // Make sure our argument is an SFVec2f.
    //
    JSObject * arg_obj;
    if (!JS_ValueToObject(cx, argv[0], &arg_obj)) { return JS_FALSE; }
    if (!JS_InstanceOf(cx, arg_obj, &SFVec3f::jsclass, argv)) {
        return JS_FALSE;
    }

    assert(JS_GetPrivate(cx, arg_obj));
    const sfield::sfdata & arg_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, arg_obj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&arg_sfdata.field_value()));
    const openvrml::sfvec3f & argVec =
        static_cast<openvrml::sfvec3f &>(arg_sfdata.field_value());

    //
    // Construct the result object.
    //
    static JSObject * const proto = 0;
    JSObject * const parent = JS_GetParent(cx, obj);
    JSObject * const robj =
        JS_ConstructObject(cx, &SFVec3f::jsclass, proto, parent);
    if (!robj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, robj));
    sfield::sfdata & robj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&robj_sfdata.field_value()));
    openvrml::sfvec3f & resultVec =
        static_cast<openvrml::sfvec3f &>(robj_sfdata.field_value());

    resultVec.value = thisVec.value + argVec.value;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec3f::cross(JSContext * const cx,
                      JSObject * const obj,
                      const uintN argc,
                      jsval * const argv,
                      jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & obj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&obj_sfdata.field_value()));
    const openvrml::sfvec3f & thisVec =
        static_cast<openvrml::sfvec3f &>(obj_sfdata.field_value());

    //
    // Make sure our argument is an SFVec3f.
    //
    JSObject * arg_obj;
    if (!JS_ValueToObject(cx, argv[0], &arg_obj)) { return JS_FALSE; }
    if (!JS_InstanceOf(cx, arg_obj, &SFVec3f::jsclass, argv)) {
        return JS_FALSE;
    }

    assert(JS_GetPrivate(cx, arg_obj));
    const sfield::sfdata & arg_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, arg_obj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&arg_sfdata.field_value()));
    const openvrml::sfvec3f & argVec =
        static_cast<openvrml::sfvec3f &>(arg_sfdata.field_value());

    //
    // Construct the result object.
    //
    static JSObject * const proto = 0;
    JSObject * const parent = JS_GetParent(cx, obj);
    JSObject * const robj =
        JS_ConstructObject(cx, &SFVec3f::jsclass, proto, parent);
    if (!robj) {return JS_FALSE; }

    assert(JS_GetPrivate(cx, robj));
    sfield::sfdata & robj_sfdata =
        *static_cast<sfdata *>(JS_GetPrivate(cx, robj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&robj_sfdata.field_value()));
    openvrml::sfvec3f & resultVec =
        static_cast<openvrml::sfvec3f &>(robj_sfdata.field_value());

    resultVec.value = thisVec.value * argVec.value;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec3f::divide(JSContext * const cx,
                       JSObject * const obj,
                       const uintN argc,
                       jsval * const argv,
                       jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & obj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&obj_sfdata.field_value()));
    const openvrml::sfvec3f & thisVec =
        static_cast<openvrml::sfvec3f &>(obj_sfdata.field_value());

    //
    // Make sure our argument is a number.
    //
    jsdouble divisor;
    if (!JS_ValueToNumber(cx, argv[0], &divisor)) { return JS_FALSE; }

    //
    // Construct the result object.
    //
    static JSObject * const proto = 0;
    JSObject * const parent = JS_GetParent(cx, obj);
    JSObject * const robj =
        JS_ConstructObject(cx, &SFVec3f::jsclass, proto, parent);
    if (!robj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, robj));
    sfield::sfdata & robj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&robj_sfdata.field_value()));
    openvrml::sfvec3f & resultVec =
        static_cast<openvrml::sfvec3f &>(robj_sfdata.field_value());

    resultVec.value = thisVec.value / divisor;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec3f::dot(JSContext * const cx,
                    JSObject * const obj,
                    const uintN argc,
                    jsval * const argv,
                    jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & obj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&obj_sfdata.field_value()));
    const openvrml::sfvec3f & thisVec =
        static_cast<openvrml::sfvec3f &>(obj_sfdata.field_value());

    //
    // Make sure our argument is an SFVec3f.
    //
    JSObject * arg_obj;
    if (!JS_ValueToObject(cx, argv[0], &arg_obj)) { return JS_FALSE; }
    if (!JS_InstanceOf(cx, arg_obj, &SFVec3f::jsclass, argv)) {
        return JS_FALSE;
    }

    assert(JS_GetPrivate(cx, arg_obj));
    const sfield::sfdata & arg_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, arg_obj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&arg_sfdata.field_value()));
    const openvrml::sfvec3f & argVec =
        static_cast<openvrml::sfvec3f &>(arg_sfdata.field_value());

    const jsdouble result = thisVec.value.dot(argVec.value);
    if (!JS_NewDoubleValue(cx, result, rval)) { return JS_FALSE; }
    return JS_TRUE;
}

JSBool SFVec3f::length(JSContext * const cx,
                       JSObject * const obj,
                       uintN,
                       jsval *,
                       jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&sfdata.field_value()));
    const openvrml::sfvec3f & thisVec =
        static_cast<openvrml::sfvec3f &>(sfdata.field_value());

    if (!JS_NewDoubleValue(cx, thisVec.value.length(), rval)) {
        return JS_FALSE;
    }
    return JS_TRUE;
}

JSBool SFVec3f::multiply(JSContext * const cx,
                         JSObject * const obj,
                         const uintN argc,
                         jsval * const argv,
                         jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & obj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&obj_sfdata.field_value()));
    const openvrml::sfvec3f & thisVec =
        static_cast<openvrml::sfvec3f &>(obj_sfdata.field_value());

    //
    // Make sure our argument is a number.
    //
    jsdouble factor;
    if (!JS_ValueToNumber(cx, argv[0], &factor)) { return JS_FALSE; }

    //
    // Construct the result object.
    //
    static JSObject * const proto = 0;
    JSObject * const parent = JS_GetParent(cx, obj);
    JSObject * const robj =
        JS_ConstructObject(cx, &SFVec3f::jsclass, proto, parent);
    if (!robj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, robj));
    const sfield::sfdata & arg_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&arg_sfdata.field_value()));
    openvrml::sfvec3f & resultVec =
        static_cast<openvrml::sfvec3f &>(arg_sfdata.field_value());

    resultVec.value = thisVec.value * factor;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec3f::negate(JSContext * const cx,
                       JSObject * const obj,
                       uintN,
                       jsval *,
                       jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & obj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&obj_sfdata.field_value()));
    const openvrml::sfvec3f & thisVec =
        static_cast<openvrml::sfvec3f &>(obj_sfdata.field_value());

    //
    // Construct the result object.
    //
    static JSObject * const proto = 0;
    JSObject * const parent = JS_GetParent(cx, obj);
    JSObject * const robj =
        JS_ConstructObject(cx, &SFVec3f::jsclass, proto, parent);
    if (!robj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, robj));
    const sfield::sfdata & robj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&robj_sfdata.field_value()));
    openvrml::sfvec3f & resultVec =
        static_cast<openvrml::sfvec3f &>(robj_sfdata.field_value());

    resultVec.value = -thisVec.value;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec3f::normalize(JSContext * const cx,
                          JSObject * const obj,
                          uintN,
                          jsval *,
                          jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & obj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&obj_sfdata.field_value()));
    const openvrml::sfvec3f & thisVec =
        static_cast<openvrml::sfvec3f &>(obj_sfdata.field_value());

    //
    // Construct the result object.
    //
    static JSObject * const proto = 0;
    JSObject * const parent = JS_GetParent(cx, obj);
    JSObject * const robj =
        JS_ConstructObject(cx, &SFVec3f::jsclass, proto, parent);
    if (!robj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, robj));
    sfield::sfdata & robj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&robj_sfdata.field_value()));
    openvrml::sfvec3f & resultVec =
        static_cast<openvrml::sfvec3f &>(robj_sfdata.field_value());

    resultVec.value = thisVec.value.normalize();

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec3f::subtract(JSContext * const cx, JSObject * const obj,
                         const uintN argc, jsval * const argv,
                         jsval * const rval)
    throw ()
{
    assert(JS_GetPrivate(cx, obj));
    const sfield::sfdata & obj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&obj_sfdata.field_value()));
    const openvrml::sfvec3f & thisVec =
        static_cast<openvrml::sfvec3f &>(obj_sfdata.field_value());

    //
    // Make sure our argument is an SFVec2f.
    //
    JSObject * arg_obj;
    if (!JS_ValueToObject(cx, argv[0], &arg_obj)) { return JS_FALSE; }
    if (!JS_InstanceOf(cx, arg_obj, &SFVec3f::jsclass, argv)) {
        return JS_FALSE;
    }

    assert(JS_GetPrivate(cx, arg_obj));
    const sfield::sfdata & arg_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, arg_obj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&arg_sfdata.field_value()));
    const openvrml::sfvec3f & argVec =
        static_cast<openvrml::sfvec3f &>(arg_sfdata.field_value());

    //
    // Construct the result object.
    //
    static JSObject * const proto = 0;
    JSObject * const parent = JS_GetParent(cx, obj);
    JSObject * const robj =
        JS_ConstructObject(cx, &SFVec3f::jsclass, proto, parent);
    if (!robj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, robj));
    sfield::sfdata & robj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&robj_sfdata.field_value()));
    openvrml::sfvec3f & resultVec =
        static_cast<openvrml::sfvec3f &>(robj_sfdata.field_value());

    resultVec.value = thisVec.value - argVec.value;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}


MField::MFData::MFData(JsvalArray::size_type size):
    array(size)
{}

MField::MFData::~MFData()
{}

void MField::AddRoots(JSContext * const cx, JsvalArray & jsvalArray)
    throw (std::bad_alloc)
{
    using std::bad_alloc;

    size_t i;
    try {
        for (i = 0; i < jsvalArray.size(); ++i) {
            if (!JS_AddRoot(cx, &jsvalArray[i])) { throw bad_alloc(); }
        }
    } catch (bad_alloc &) {
        for (size_t j = 0; j < i - 1; ++j) {
            JS_RemoveRoot(cx, &jsvalArray[j]);
        }
        throw;
    }
}

void MField::RemoveRoots(JSContext * const cx, JsvalArray & jsvalArray)
    throw ()
{
    for (size_t i = 0; i < jsvalArray.size(); ++i) {
        const JSBool ok = JS_RemoveRoot(cx, &jsvalArray[i]);
        assert(ok);
    }
}

JSBool MField::getElement(JSContext * const cx,
                          JSObject * const obj,
                          const jsval id,
                          jsval * const vp)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(vp);
    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    if (JSVAL_IS_INT(id)
        && JSVAL_TO_INT(id) >= 0
        && size_t(JSVAL_TO_INT(id)) >= mfdata->array.size()) {
        *vp = mfdata->array[JSVAL_TO_INT(id)];
    }
    return JS_TRUE;
}

JSBool MField::getLength(JSContext * const cx, JSObject * const obj,
                         const jsval id, jsval * const vp) throw () {
    assert(cx);
    assert(obj);
    assert(vp);
    const MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    *vp = INT_TO_JSVAL(mfdata->array.size());
    return JS_TRUE;
}


template <typename Subclass>
JSObject * MFJSObject<Subclass>::initClass(JSContext * const cx,
                                           JSObject * const obj)
    throw ()
{
    assert(cx);
    assert(obj);

    static JSPropertySpec properties[] =
            { { "length", 0, JSPROP_PERMANENT, getLength, setLength },
              { 0, 0, 0, 0, 0 } };

    static JSFunctionSpec methods[] =
            { { "toString", toString, 0, 0, 0 },
              { 0, 0, 0, 0, 0 } };

    JSObject * proto = JS_InitClass(cx, obj, 0, &Subclass::jsclass,
                                    construct, 0, // constructor function, min arg count
                                    properties, methods,
                                    0, 0); // static properties and methods
    if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
    return proto;
}

template <typename Subclass>
JSBool MFJSObject<Subclass>::construct(JSContext * const cx,
                                       JSObject * obj,
	                               const uintN argc,
                                       jsval * const argv,
                                       jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(rval);

    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &Subclass::jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return initObject(cx, obj, argc, argv);
}

template <typename Subclass>
JSBool MFJSObject<Subclass>::initObject(JSContext * const cx,
                                        JSObject * const obj,
                                        const uintN argc,
                                        jsval * const argv)
    throw ()
{
    assert(cx);
    assert(obj);
    try {
        std::auto_ptr<MFData> mfdata(new MFData(argc));

        for (uintN i = 0; i < argc; ++i) {
            if (!JSVAL_IS_OBJECT(argv[i])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[i]),
                                      &Subclass::sfjsclass, argv)) {
                return JS_FALSE;
            }
            mfdata->array[i] = argv[i];
        }

        //
        // Protect array values from gc.
        //
        AddRoots(cx, mfdata->array);

        if (!JS_SetPrivate(cx, obj, mfdata.get())) { return JS_FALSE; }
        mfdata.release();
    } catch (std::bad_alloc &) {
        JS_ReportOutOfMemory(cx);
        return JS_FALSE;
    }
    return JS_TRUE;
}

template <typename Subclass>
JSBool MFJSObject<Subclass>::setElement(JSContext * const cx,
                                        JSObject * const obj,
                                        const jsval id,
                                        jsval * const vp)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(vp);
    if (JSVAL_IS_INT(id) && JSVAL_TO_INT(id) > 0) {
        MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);

        //
        // Make sure new value is of the correct type.
        //
        JSObject * vp_obj;
        if (!JS_ValueToObject(cx, *vp, &vp_obj)) { return JS_FALSE; }
        if (!JS_InstanceOf(cx, vp_obj, &Subclass::sfjsclass, 0)) {
            JS_ReportError(cx, "Expected a %s.", Subclass::sfjsclass.name);
            return JS_FALSE;
        }

        //
        // Grow array if necessary.
        //
        if (size_t(JSVAL_TO_INT(id)) >= mfdata->array.size()) {
            jsval newLength = INT_TO_JSVAL(JSVAL_TO_INT(id) + 1);
            if (!setLength(cx, obj, 0, &newLength)) { return JS_FALSE; }
        }

        //
        // Put the new element in the array.
        //
        mfdata->array[JSVAL_TO_INT(id)] = *vp;
        mfdata->changed = true;
    }
    return JS_TRUE;
}

template <typename Subclass>
JSBool MFJSObject<Subclass>::setLength(JSContext * const cx,
                                       JSObject * const obj,
                                       jsval,
                                       jsval * const vp)
    throw ()
{
    using std::copy;

    assert(cx);
    assert(obj);
    assert(vp);

    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    uint32 new_length;
    if (!JS_ValueToECMAUint32(cx, *vp, &new_length)) { return JS_FALSE; }

    if (new_length == mfdata->array.size()) {
        mfdata->changed = true;
        return JS_TRUE; // Nothing to do.
    }

    try {
        JsvalArray newArray(new_length);
        AddRoots(cx, newArray); // Protect from gc.

        //
        // Copy the values from the old array.
        //
        const size_t length = newArray.size() < mfdata->array.size()
                            ? newArray.size()
                            : mfdata->array.size();
        copy(mfdata->array.begin(), mfdata->array.begin() + length,
             newArray.begin());

        //
        // Initialize the rest of the array with new values.
        //
        try {
            for (size_t i = length; i < newArray.size(); ++i) {
                JSObject * const element =
                        JS_ConstructObject(cx, &Subclass::sfjsclass, 0, 0);
                if (!element) { throw std::bad_alloc(); }
                newArray[i] = OBJECT_TO_JSVAL(element);
            }
        } catch (std::bad_alloc &) {
            RemoveRoots(cx, newArray);
            throw;
        }

        //
        // Remove roots for the values in the old array.
        //
        RemoveRoots(cx, mfdata->array);

        //
        // Finally, swap the new vector with the old one.
        //
        std::swap(mfdata->array, newArray);
    } catch (std::bad_alloc &) {
        JS_ReportOutOfMemory(cx);
        return JS_FALSE;
    }
    mfdata->changed = true;
    return JS_TRUE;
}

template <typename Subclass>
JSBool MFJSObject<Subclass>::toString(JSContext * const cx,
                                      JSObject * const obj,
                                      uintN, jsval *,
                                      jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(rval);

    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    std::ostringstream out;
    out << '[';
    for (JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_OBJECT(mfdata->array[i]));
        const sfield::sfdata * const sfdata =
                static_cast<sfield::sfdata *>
                    (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
        assert(sfdata);
        out << sfdata->field_value();
        if ((i + 1) < mfdata->array.size()) { out << ", "; }
    }
    out << ']';

    JSString * jsstr = JS_NewStringCopyZ(cx, out.str().c_str());
    if (!jsstr) { return JS_FALSE; }
    *rval = STRING_TO_JSVAL(jsstr);
    return JS_TRUE;
}

template <typename Subclass>
void MFJSObject<Subclass>::finalize(JSContext * const cx, JSObject * const obj)
    throw ()
{
    assert(cx);
    assert(obj);
    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    if (mfdata) {
        RemoveRoots(cx, mfdata->array);
        delete mfdata;
        JS_SetPrivate(cx, obj, 0);
    }
}


template <typename Subclass>
JSObject * MFJSDouble<Subclass>::initClass(JSContext * const cx,
                                           JSObject * const obj)
    throw ()
{
    static JSPropertySpec properties[] =
            { { "length", 0, JSPROP_PERMANENT, getLength, setLength },
              { 0, 0, 0, 0, 0 } };

    static JSFunctionSpec methods[] =
            { { "toString", toString, 0, 0, 0 },
              { 0, 0, 0, 0, 0 } };

    JSObject * proto = JS_InitClass(cx, obj, 0, &Subclass::jsclass,
                                    construct, 0, // constructor function, min arg count
                                    properties, methods,
                                    0, 0); // static properties and methods
    if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
    return proto;
}

template <typename Subclass>
JSBool MFJSDouble<Subclass>::construct(JSContext * const cx,
                                       JSObject * obj,
	                               const uintN argc,
                                       jsval * const argv,
                                       jsval * rval)
    throw ()
{
    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &Subclass::jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return initObject(cx, obj, argc, argv);
}

template <typename Subclass>
JSBool MFJSDouble<Subclass>::initObject(JSContext * const cx,
                                        JSObject * const obj,
                                        const uintN argc,
                                        jsval * const argv)
    throw ()
{
    try {
        std::auto_ptr<MFData> mfdata(new MFData(argc));
        for (uintN i = 0; i < argc; ++i) {
            jsdouble number;
            if (!JS_ValueToNumber(cx, argv[i], &number)) { return JS_FALSE; }
            if (!JS_NewNumberValue(cx, number, &mfdata->array[i])) {
                return JS_FALSE;
            }
        }

        if (!JS_SetPrivate(cx, obj, mfdata.get())) { return JS_FALSE; }

        //
        // Protect array values from gc.
        //
        AddRoots(cx, mfdata->array);

        mfdata.release();

    } catch (std::bad_alloc &) {
        JS_ReportOutOfMemory(cx);
        return JS_FALSE;
    }
    return JS_TRUE;
}

template <typename Subclass>
JSBool MFJSDouble<Subclass>::setElement(JSContext * const cx,
                                        JSObject * const obj,
                                        const jsval id,
                                        jsval * const vp)
    throw ()
{
    if (JSVAL_IS_INT(id) && JSVAL_TO_INT(id) >= 0) {
        MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);

        //
        // Make sure new value is a number.
        //
        jsdouble number;
        if (!JS_ValueToNumber(cx, *vp, &number)) { return JS_FALSE; }

        //
        // Grow array if necessary.
        //
        if (size_t(JSVAL_TO_INT(id)) >= mfdata->array.size()) {
            jsval newLength = INT_TO_JSVAL(JSVAL_TO_INT(id) + 1);
            if (!setLength(cx, obj, 0, &newLength)) { return JS_FALSE; }
        }

        //
        // Put the new element in the array.
        //
        if (!JS_NewNumberValue(cx, number, &mfdata->array[JSVAL_TO_INT(id)])) {
            return JS_FALSE;
        }
        mfdata->changed = true;
    }
    return JS_TRUE;
}

template <typename Subclass>
JSBool MFJSDouble<Subclass>::setLength(JSContext * const cx,
                                       JSObject * const obj,
                                       const jsval id,
                                       jsval * const vp)
    throw ()
{
    using std::copy;

    assert(cx);
    assert(obj);
    assert(vp);

    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    uint32 new_length;
    if (!JS_ValueToECMAUint32(cx, *vp, &new_length)) { return JS_FALSE; }

    if (size_t(JSVAL_TO_INT(*vp)) == mfdata->array.size()) {
        mfdata->changed = true;
        return JS_TRUE; // Nothing to do.
    }

    try {
        JsvalArray newArray(new_length);
        AddRoots(cx, newArray); // Protect from gc.

        //
        // Copy the values from the old array.
        //
        const size_t length = newArray.size() < mfdata->array.size()
                            ? newArray.size()
                            : mfdata->array.size();
        copy(mfdata->array.begin(), mfdata->array.begin() + length,
             newArray.begin());

        //
        // Initialize the rest of the array with new values.
        //
        try {
            for (size_t i = length; i < newArray.size(); ++i) {
                if (!JS_NewDoubleValue(cx, 0.0, &newArray[i])) {
                    throw std::bad_alloc();
                }
            }
        } catch (std::bad_alloc &) {
            RemoveRoots(cx, newArray);
            throw;
        }

        //
        // Remove roots for the values in the old array.
        //
        RemoveRoots(cx, mfdata->array);

        //
        // Finally, swap the new vector with the old one.
        //
        std::swap(mfdata->array, newArray);
    } catch (std::bad_alloc & ex) {
        JS_ReportOutOfMemory(cx);
        return JS_FALSE;
    }
    mfdata->changed = true;
    return JS_TRUE;
}

template <typename Subclass>
JSBool MFJSDouble<Subclass>::toString(JSContext * const cx,
                                      JSObject * const obj,
                                      uintN, jsval *,
                                      jsval * const rval)
    throw ()
{
    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    std::ostringstream out;
    out << '[';
    for (JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_DOUBLE(mfdata->array[i]));
        out << *JSVAL_TO_DOUBLE(mfdata->array[i]);
        if ((i + 1) < mfdata->array.size()) { out << ", "; }
    }
    out << ']';

    JSString * const jsstr = JS_NewStringCopyZ(cx, out.str().c_str());
    if (!jsstr) { return JS_FALSE; }
    *rval = STRING_TO_JSVAL(jsstr);
    return JS_TRUE;
}

template <typename Subclass>
void MFJSDouble<Subclass>::finalize(JSContext * const cx, JSObject * const obj)
    throw ()
{
    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    if (mfdata) {
        RemoveRoots(cx, mfdata->array);
        delete mfdata;
        JS_SetPrivate(cx, obj, 0);
    }
}

JSClass MFColor::jsclass =
        { "MFColor", JSCLASS_HAS_PRIVATE,
          JS_PropertyStub, JS_PropertyStub,
          getElement, setElement,
          JS_EnumerateStub, JS_ResolveStub,
          JS_ConvertStub, finalize };

JSClass & MFColor::sfjsclass = SFColor::jsclass;

std::auto_ptr<openvrml::mfcolor>
MFColor::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (bad_conversion, std::bad_alloc)
{
    assert(cx);
    assert(obj);

    if (!JS_InstanceOf(cx, obj, &MFColor::jsclass, 0)) {
        throw bad_conversion("MFColor object expected.");
    }

    MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);
    std::auto_ptr<openvrml::mfcolor>
            mfcolor(new openvrml::mfcolor(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_OBJECT(mfdata->array[i]));
        assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                             &SFColor::jsclass, 0));
        const sfield::sfdata * const sfdata =
                static_cast<sfield::sfdata *>
                    (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
        assert(sfdata);

        const openvrml::sfcolor & sfcolor =
                static_cast<openvrml::sfcolor &>(sfdata->field_value());
        mfcolor->value[i] = sfcolor.value;
    }
    return mfcolor;
}

JSBool MFColor::toJsval(const std::vector<color> & colors,
                        JSContext * const cx,
                        JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    JSObject * const mfcolorObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mfcolorObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(colors.size());
    if (!setLength(cx, mfcolorObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata =
        static_cast<MFData *>(JS_GetPrivate(cx, mfcolorObj));
    for (size_t i = 0; i < colors.size(); ++i) {
        if (!SFColor::toJsval(colors[i], cx, obj, &mfdata->array[i])) {
            return JS_FALSE;
        }
    }
    *rval = OBJECT_TO_JSVAL(mfcolorObj);
    return JS_TRUE;
}

JSClass MFFloat::jsclass = {
    "MFFloat",
    JSCLASS_HAS_PRIVATE,
    JS_PropertyStub,
    JS_PropertyStub,
    getElement,
    setElement,
    JS_EnumerateStub,
    JS_ResolveStub,
    JS_ConvertStub,
    finalize
};

JSBool MFFloat::toJsval(const std::vector<float> & floats,
                        JSContext * const cx,
                        JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(rval);

    JSObject * const mffloatObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mffloatObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(floats.size());
    if (!setLength(cx, mffloatObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata =
        static_cast<MFData *>(JS_GetPrivate(cx, mffloatObj));
    for (size_t i = 0; i < floats.size(); ++i) {
        if (!JS_NewDoubleValue(cx, floats[i], &mfdata->array[i])) {
            return JS_FALSE;
        }
    }
    *rval = OBJECT_TO_JSVAL(mffloatObj);
    return JS_TRUE;
}

std::auto_ptr<openvrml::mffloat>
MFFloat::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (bad_conversion, std::bad_alloc)
{
    assert(cx);
    assert(obj);

    if (!JS_InstanceOf(cx, obj, &MFFloat::jsclass, 0)) {
        throw bad_conversion("MFFloat object expected.");
    }
    MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);
    std::auto_ptr<openvrml::mffloat>
            mffloat(new openvrml::mffloat(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_DOUBLE(mfdata->array[i]));
        mffloat->value[i] = *JSVAL_TO_DOUBLE(mfdata->array[i]);
    }
    return mffloat;
}

JSClass MFInt32::jsclass = {
    "MFInt32",
    JSCLASS_HAS_PRIVATE,
    JS_PropertyStub,
    JS_PropertyStub,
    getElement,
    setElement,
    JS_EnumerateStub,
    JS_ResolveStub,
    JS_ConvertStub,
    finalize
};

JSObject * MFInt32::initClass(JSContext * const cx, JSObject * const obj) {
    assert(cx);
    assert(obj);

    static JSPropertySpec properties[] =
            { { "length", 0, JSPROP_PERMANENT, getLength, setLength },
              { 0, 0, 0, 0, 0 } };

    static JSFunctionSpec methods[] =
            { { "toString", toString, 0, 0, 0 },
              { 0, 0, 0, 0, 0 } };

    JSObject * proto =
        JS_InitClass(cx, obj, 0, &jsclass,
                     construct, 0, // constructor function, min arg count
                     properties, methods,
                     0, 0); // static properties and methods
    if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
    return proto;
}

JSBool MFInt32::toJsval(const std::vector<openvrml::int32> & int32s,
                        JSContext * const cx,
                        JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(rval);

    JSObject * const mfint32Obj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mfint32Obj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(int32s.size());
    if (!setLength(cx, mfint32Obj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfint32Obj));
    for (size_t i = 0; i < int32s.size(); ++i) {
        mfdata->array[i] = INT_TO_JSVAL(int32s[i]);
    }
    *rval = OBJECT_TO_JSVAL(mfint32Obj);
    return JS_TRUE;
}

std::auto_ptr<openvrml::mfint32>
MFInt32::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (bad_conversion, std::bad_alloc)
{
    assert(cx);
    assert(obj);

    if (!JS_InstanceOf(cx, obj, &MFInt32::jsclass, 0)) {
        throw bad_conversion("MFInt32 object expected.");
    }
    MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);
    std::auto_ptr<openvrml::mfint32>
        mfint32(new openvrml::mfint32(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_INT(mfdata->array[i]));
        mfint32->value[i] = JSVAL_TO_INT(mfdata->array[i]);
    }
    return mfint32;
}

JSBool MFInt32::construct(JSContext * const cx, JSObject * obj,
	                  const uintN argc, jsval * const argv,
                          jsval * const rval)
{
    assert(cx);
    assert(obj);
    assert(rval);

    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return initObject(cx, obj, argc, argv);
}

JSBool MFInt32::initObject(JSContext * const cx,
                           JSObject * const obj,
                           const uintN argc,
                           jsval * const argv)
{
    assert(cx);
    assert(obj);

    try {
        std::auto_ptr<MFData> mfdata(new MFData(argc));
        for (uintN i = 0; i < argc; ++i) {
            //
            // Convert the jsval to an int32 and back to a jsval in order
            // to remove any decimal part.
            //
            ::int32 integer;
            if (!JS_ValueToECMAInt32(cx, argv[i], &integer)) {
                return JS_FALSE;
            }
            if (!JS_NewNumberValue(cx, jsdouble(integer), &mfdata->array[i])) {
                return JS_FALSE;
            }
        }
        if (!JS_SetPrivate(cx, obj, mfdata.get())) { return JS_FALSE; }
        mfdata.release();
    } catch (std::bad_alloc & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        return JS_FALSE;
    }
    return JS_TRUE;
}

JSBool MFInt32::setElement(JSContext * const cx,
                           JSObject * const obj,
                           const jsval id,
                           jsval * const vp)
{
    assert(cx);
    assert(obj);
    assert(vp);

    if (JSVAL_IS_INT(id) && JSVAL_TO_INT(id) >= 0) {
        const size_t index = JSVAL_TO_INT(id);

        MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);

        //
        // Grow array if necessary.
        //
        if (index >= mfdata->array.size()) {
            jsval newLength = INT_TO_JSVAL(JSVAL_TO_INT(id) + 1);
            if (!setLength(cx, obj, 0, &newLength)) { return JS_FALSE; }
        }

        //
        // Convert the jsval to an int32 and back to a jsval in order
        // to remove any decimal part.
        //
        ::int32 i;
        if (!JS_ValueToECMAInt32(cx, *vp, &i)) { return JS_FALSE; }
        if (!JS_NewNumberValue(cx, jsdouble(i), &mfdata->array[index])) {
            return JS_FALSE;
        }
        mfdata->changed = true;
    }
    return JS_TRUE;
}

JSBool MFInt32::setLength(JSContext * const cx,
                          JSObject * const obj,
                          const jsval id,
                          jsval * const vp)
{
    assert(cx);
    assert(obj);
    assert(vp);

    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    uint32 new_length;
    if (!JS_ValueToECMAUint32(cx, *vp, &new_length)) { return JS_FALSE; }

    try {
        if (size_t(JSVAL_TO_INT(*vp)) != mfdata->array.size()) {
            mfdata->array.resize(JSVAL_TO_INT(*vp));
        }
    } catch (std::bad_alloc &) {
        JS_ReportOutOfMemory(cx);
        return JS_FALSE;
    }
    mfdata->changed = true;
    return JS_TRUE;
}

JSBool MFInt32::toString(JSContext * const cx,
                         JSObject * const obj,
                         uintN,
                         jsval *,
                         jsval * const rval)
{
    assert(cx);
    assert(obj);

    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    std::ostringstream out;
    out << '[';
    for (JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        out << JSVAL_TO_INT(mfdata->array[i]);
        if ((i + 1) < mfdata->array.size()) { out << ", "; }
    }
    out << ']';

    JSString * const jsstr = JS_NewStringCopyZ(cx, out.str().c_str());
    if (!jsstr) { return JS_FALSE; }
    *rval = STRING_TO_JSVAL(jsstr);
    return JS_TRUE;
}

void MFInt32::finalize(JSContext * const cx, JSObject * const obj)
{
    delete static_cast<MFData *>(JS_GetPrivate(cx, obj));
    JS_SetPrivate(cx, obj, 0);
}

JSClass MFNode::jsclass = {
    "MFNode",
    JSCLASS_HAS_PRIVATE,
    JS_PropertyStub,
    JS_PropertyStub,
    getElement,
    setElement,
    JS_EnumerateStub,
    JS_ResolveStub,
    JS_ConvertStub,
    finalize
};

JSObject * MFNode::initClass(JSContext * const cx, JSObject * const obj)
    throw ()
{
    assert(cx);
    assert(obj);

    static JSPropertySpec properties[] =
            { { "length", 0, JSPROP_PERMANENT, getLength, setLength },
              { 0, 0, 0, 0, 0 } };

    static JSFunctionSpec methods[] =
            { { "toString", toString, 0, 0, 0 },
              { 0, 0, 0, 0, 0 } };

    JSObject * proto = JS_InitClass(cx, obj, 0, &jsclass,
                                    construct, 0, // constructor function, min arg count
                                    properties, methods,
                                    0, 0); // static properties and methods
    if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
    return proto;
}

JSBool MFNode::initObject(JSContext * const cx, JSObject * const obj,
                          const uintN argc, jsval * const argv)
    throw ()
{
    assert(cx);
    assert(obj);

    try {
        std::auto_ptr<MFData> mfdata(new MFData(argc));

        js_::script & script =
            *static_cast<js_::script *>(JS_GetContextPrivate(cx));
        JSClass & sfnode_jsclass = script.sfnode_class;

        for (uintN i = 0; i < argc; ++i) {
            //
            // Make sure all args are SFNodes.
            //
            if (!JSVAL_IS_OBJECT(argv[i])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[i]),
                                      &sfnode_jsclass, argv)) {
                return JS_FALSE;
            }
            mfdata->array[i] = argv[i];
        }
        if (!JS_SetPrivate(cx, obj, mfdata.get())) { return JS_FALSE; }
        mfdata.release();
    } catch (std::bad_alloc & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        return JS_FALSE;
    }
    return JS_TRUE;
}

JSBool MFNode::toJsval(const std::vector<node_ptr> & nodes,
                       JSContext * const cx,
                       JSObject * const obj,
                       jsval * const rval)
    throw ()
{
    JSObject * const mfnodeObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mfnodeObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(nodes.size());
    if (!setLength(cx, mfnodeObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata =
        static_cast<MFData *>(JS_GetPrivate(cx, mfnodeObj));
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (!SFNode::toJsval(nodes[i], cx, obj, &mfdata->array[i])) {
            return JS_FALSE;
        }
    }
    *rval = OBJECT_TO_JSVAL(mfnodeObj);
    return JS_TRUE;
}

std::auto_ptr<openvrml::mfnode>
MFNode::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (bad_conversion, std::bad_alloc)
{
    assert(cx);
    assert(obj);

# ifndef NDEBUG
    js_::script & script =
        *static_cast<js_::script *>(JS_GetContextPrivate(cx));
    JSClass & sfnode_jsclass = script.sfnode_class;
# endif

    if (!JS_InstanceOf(cx, obj, &MFNode::jsclass, 0)) {
        throw bad_conversion("MFNode object expected.");
    }
    MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);
    std::auto_ptr<openvrml::mfnode>
            mfnode(new openvrml::mfnode(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_OBJECT(mfdata->array[i]));
        assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                             &sfnode_jsclass, 0));
        const sfield::sfdata * const sfdata =
            static_cast<sfield::sfdata *>
                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
        assert(sfdata);

        const openvrml::sfnode & sfnode =
                static_cast<openvrml::sfnode &>(sfdata->field_value());
        mfnode->value[i] = sfnode.value;
    }
    return mfnode;
}

JSBool MFNode::construct(JSContext * const cx,
                         JSObject * obj,
                         const uintN argc,
                         jsval * const argv,
                         jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(rval);

    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return initObject(cx, obj, argc, argv);
}

JSBool MFNode::setElement(JSContext * const cx,
                          JSObject * const obj,
                          const jsval id,
                          jsval * const vp)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(vp);

    if (!JSVAL_IS_INT(id) && JSVAL_TO_INT(id) >= 0) {
        MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);

        //
        // Make sure new value is an SFNode.
        //
        JSObject * vp_obj;
        if (!JS_ValueToObject(cx, *vp, &vp_obj)) { return JS_FALSE; }
        js_::script & script =
            *static_cast<js_::script *>(JS_GetContextPrivate(cx));
        JSClass & sfnode_jsclass = script.sfnode_class;
        if (!JS_InstanceOf(cx, vp_obj, &sfnode_jsclass, 0)) {
            JS_ReportError(cx, "Expected an SFNode.");
            return JS_FALSE;
        }


        //
        // Grow array if necessary.
        //
        if (size_t(JSVAL_TO_INT(id)) >= mfdata->array.size()) {
            jsval newLength = INT_TO_JSVAL(JSVAL_TO_INT(id) + 1);
            if (!setLength(cx, obj, 0, &newLength)) { return JS_FALSE; }
        }

        //
        // Put the new element in the array.
        //
        mfdata->array[JSVAL_TO_INT(id)] = *vp;
        mfdata->changed = true;
    }
    return JS_TRUE;
}

JSBool MFNode::setLength(JSContext * const cx,
                         JSObject * const obj,
                         const jsval id,
                         jsval * const vp)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(vp);

    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    uint32 new_length;
    if (!JS_ValueToECMAUint32(cx, *vp, &new_length)) { return JS_FALSE; }

    try {
        using std::copy;
        using std::swap;

        JsvalArray newArray(new_length);
        AddRoots(cx, newArray); // Protect from gc.

        //
        // Copy the values from the old array.
        //
        const size_t length = newArray.size() < mfdata->array.size()
                            ? newArray.size()
                            : mfdata->array.size();
        copy(mfdata->array.begin(), mfdata->array.begin() + length,
             newArray.begin());

        //
        // Initialize the rest of the array with new values.
        //
        try {
            jsval arg = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, "Group {}"));

            js_::script & script =
                *static_cast<js_::script *>(JS_GetContextPrivate(cx));
            JSClass & sfnode_jsclass = script.sfnode_class;

            for (size_t i = length; i < newArray.size(); ++i) {
                JSObject * const element =
                    JS_ConstructObjectWithArguments(cx, &sfnode_jsclass, 0, 0,
                                                    1, &arg);
                if (!element) { throw std::bad_alloc(); }
                newArray[i] = OBJECT_TO_JSVAL(element);
            }
        } catch (std::bad_alloc &) {
            RemoveRoots(cx, newArray);
            throw;
        }

        //
        // Remove roots for the values in the old array.
        //
        RemoveRoots(cx, mfdata->array);

        //
        // Finally, swap the new vector with the old one.
        //
        swap(mfdata->array, newArray);
        mfdata->changed = true;
    } catch (std::bad_alloc &) {
        JS_ReportOutOfMemory(cx);
        return JS_FALSE;
    }
    return JS_TRUE;
}

JSBool MFNode::toString(JSContext * const cx,
                        JSObject * const obj,
                        const uintN argc,
                        jsval * const argv,
                        jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);

    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    std::ostringstream out;
    out << '[';
    for (JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        const sfield::sfdata * const sfdata =
            static_cast<sfield::sfdata *>
                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
        assert(sfdata);
        out << sfdata->field_value();
        if ((i + 1) < mfdata->array.size()) { out << ", "; }
    }
    out << ']';

    JSString * const jsstr = JS_NewStringCopyZ(cx, out.str().c_str());
    if (!jsstr) { return JS_FALSE; }
    *rval = STRING_TO_JSVAL(jsstr);
    return JS_TRUE;
}

void MFNode::finalize(JSContext * const cx, JSObject * const obj)
    throw ()
{
    assert(cx);
    assert(obj);
    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    if (mfdata) {
        RemoveRoots(cx, mfdata->array);
        delete mfdata;
        JS_SetPrivate(cx, obj, 0);
    }
}

JSClass MFRotation::jsclass = {
    "MFRotation",
    JSCLASS_HAS_PRIVATE,
    JS_PropertyStub,
    JS_PropertyStub,
    getElement,
    setElement,
    JS_EnumerateStub,
    JS_ResolveStub,
    JS_ConvertStub,
    finalize
};

JSClass & MFRotation::sfjsclass = SFRotation::jsclass;

JSBool MFRotation::toJsval(const std::vector<rotation> & rotations,
                           JSContext * const cx,
                           JSObject * const obj,
                           jsval * const rval)
    throw ()
{
    JSObject * const mfrotationObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mfrotationObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(rotations.size());
    if (!setLength(cx, mfrotationObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfrotationObj));
    for (size_t i = 0; i < rotations.size(); ++i) {
        if (!SFRotation::toJsval(rotations[i], cx, obj, &mfdata->array[i])) {
            return JS_FALSE;
        }
    }
    *rval = OBJECT_TO_JSVAL(mfrotationObj);
    return JS_TRUE;
}

std::auto_ptr<openvrml::mfrotation>
MFRotation::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (bad_conversion, std::bad_alloc)
{
    assert(cx);
    assert(obj);

    if (!JS_InstanceOf(cx, obj, &MFRotation::jsclass, 0)) {
        throw bad_conversion("MFRotation object expected.");
    }
    MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);
    std::auto_ptr<openvrml::mfrotation>
            mfrotation(new openvrml::mfrotation(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_OBJECT(mfdata->array[i]));
        assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                             &SFRotation::jsclass, 0));
        const sfield::sfdata * const sfdata =
            static_cast<sfield::sfdata *>
                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
        assert(sfdata);

        const openvrml::sfrotation & sfrotation =
                static_cast<openvrml::sfrotation &>(sfdata->field_value());
        mfrotation->value[i] = sfrotation.value;
    }
    return mfrotation;
}

JSClass MFString::jsclass = {
    "MFString",
    JSCLASS_HAS_PRIVATE,
    JS_PropertyStub,
    JS_PropertyStub,
    getElement,
    setElement,
    JS_EnumerateStub,
    JS_ResolveStub,
    JS_ConvertStub,
    finalize
};

JSObject * MFString::initClass(JSContext * const cx, JSObject * const obj)
    throw ()
{
    static JSPropertySpec properties[] =
            { { "length", 0, JSPROP_PERMANENT, getLength, setLength },
              { 0, 0, 0, 0, 0 } };

    static JSFunctionSpec methods[] =
            { { "toString", toString, 0, 0, 0 },
              { 0, 0, 0, 0, 0 } };

    JSObject * proto =
        JS_InitClass(cx, obj, 0, &jsclass,
                     construct, 0, // constructor function, min arg count
                     properties, methods,
                     0, 0); // static properties and methods
    if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
    return proto;
}

JSBool MFString::toJsval(const std::vector<std::string> & strings,
                         JSContext * const cx,
                         JSObject * const obj,
                         jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(rval);
    JSObject * const mfstringObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mfstringObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(strings.size());
    if (!setLength(cx, mfstringObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata =
        static_cast<MFData *>(JS_GetPrivate(cx, mfstringObj));
    for (size_t i = 0; i < strings.size(); ++i) {
        JSString * jsstring = JS_NewStringCopyZ(cx, strings[i].c_str());
        if (!jsstring) { return JS_FALSE; }
        mfdata->array[i] = STRING_TO_JSVAL(jsstring);
    }
    *rval = OBJECT_TO_JSVAL(mfstringObj);
    return JS_TRUE;
}

std::auto_ptr<openvrml::mfstring>
MFString::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (bad_conversion, std::bad_alloc)
{
    assert(cx);
    assert(obj);

    if (!JS_InstanceOf(cx, obj, &MFString::jsclass, 0)) {
        throw bad_conversion("MFString object expected.");
    }
    MField::MFData * const mfdata =
        static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);
    std::auto_ptr<openvrml::mfstring>
        mfstring(new openvrml::mfstring(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_STRING(mfdata->array[i]));
        const char * const str =
            JS_GetStringBytes(JSVAL_TO_STRING(mfdata->array[i]));
        mfstring->value[i] = str;
    }
    return mfstring;
}

JSBool MFString::construct(JSContext * const cx,
                           JSObject * obj,
	                   const uintN argc,
                           jsval * const argv,
                           jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(rval);

    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return initObject(cx, obj, argc, argv);
}

JSBool MFString::initObject(JSContext * const cx,
                            JSObject * const obj,
                            const uintN argc,
                            jsval * const argv)
    throw ()
{
    assert(cx);
    assert(obj);

    try {
        std::auto_ptr<MFData> mfdata(new MFData(argc));
        for (uintN i = 0; i < argc; ++i) {
            JSString * const str = JS_ValueToString(cx, argv[i]);
            if (!str) { return JS_FALSE; }
            mfdata->array[i] = STRING_TO_JSVAL(str);
        }

        //
        // Protect array values from gc.
        //
        AddRoots(cx, mfdata->array);

        if (!JS_SetPrivate(cx, obj, mfdata.get())) { return JS_FALSE; }
        mfdata.release();
    } catch (std::bad_alloc &) {
        JS_ReportOutOfMemory(cx);
        return JS_FALSE;
    }
    return JS_TRUE;
}

JSBool MFString::setElement(JSContext * const cx,
                            JSObject * const obj,
                            const jsval id,
                            jsval * const vp)
    throw ()
{
    if (JSVAL_IS_INT(id) && JSVAL_TO_INT(id) >= 0) {
        MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);

        //
        // Grow array if necessary.
        //
        if (size_t(JSVAL_TO_INT(id)) >= mfdata->array.size()) {
            jsval newLength = INT_TO_JSVAL(JSVAL_TO_INT(id) + 1);
            if (!setLength(cx, obj, 0, &newLength)) { return JS_FALSE; }
        }

        //
        // Convert the value to a JSString, if necessary.
        //
        JSString * const str = JS_ValueToString(cx, *vp);
        if (!str) { return JS_FALSE; }

        //
        // Put the new element in the array.
        //
        mfdata->array[JSVAL_TO_INT(id)] = STRING_TO_JSVAL(str);
        mfdata->changed = true;
    }
    return JS_TRUE;
}

JSBool MFString::setLength(JSContext * const cx,
                           JSObject * const obj,
                           const jsval id,
                           jsval * const vp)
    throw ()
{
    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    uint32 new_length;
    if (!JS_ValueToECMAUint32(cx, *vp, &new_length)) { return JS_FALSE; }

    try {
        JsvalArray newArray(new_length);
        AddRoots(cx, newArray); // Protect from gc.

        //
        // Copy the values from the old array.
        //
        const size_t length = newArray.size() < mfdata->array.size()
                            ? newArray.size()
                            : mfdata->array.size();
        std::copy(mfdata->array.begin(), mfdata->array.begin() + length,
                  newArray.begin());

        //
        // Initialize the rest of the array with new values.
        //
        try {
            for (size_t i = length; i < newArray.size(); ++i) {
                JSString * jsstring = JS_NewStringCopyZ(cx, "");
                if (!jsstring) { throw std::bad_alloc(); }
                newArray[i] = STRING_TO_JSVAL(jsstring);
            }
        } catch (std::bad_alloc &) {
            RemoveRoots(cx, newArray);
            throw;
        }

        //
        // Remove roots for the values in the old array.
        //
        RemoveRoots(cx, mfdata->array);

        //
        // Finally, swap the new vector with the old one.
        //
        std::swap(mfdata->array, newArray);
    } catch (std::bad_alloc &) {
        JS_ReportOutOfMemory(cx);
        return JS_FALSE;
    }
    mfdata->changed = true;
    return JS_TRUE;
}

JSBool MFString::toString(JSContext * const cx,
                          JSObject * const obj,
                          uintN,
                          jsval *,
                          jsval * const rval)
    throw ()
{
    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    std::ostringstream out;
    out << '[';
    for (JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_STRING(mfdata->array[i]));
        out << '\"'
            << JS_GetStringBytes(JSVAL_TO_STRING(mfdata->array[i]))
            << '\"';
        if ((i + 1) < mfdata->array.size()) { out << ", "; }
    }
    out << ']';

    JSString * const jsstr = JS_NewStringCopyZ(cx, out.str().c_str());
    if (!jsstr) { return JS_FALSE; }
    *rval = STRING_TO_JSVAL(jsstr);
    return JS_TRUE;
}

void MFString::finalize(JSContext * const cx, JSObject * const obj)
    throw ()
{
    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    if (mfdata) {
        RemoveRoots(cx, mfdata->array);
        delete mfdata;
        JS_SetPrivate(cx, obj, 0);
    }
}

JSClass MFTime::jsclass = {
    "MFTime",
    JSCLASS_HAS_PRIVATE,
    JS_PropertyStub,
    JS_PropertyStub,
    getElement,
    setElement,
    JS_EnumerateStub,
    JS_ResolveStub,
    JS_ConvertStub,
    finalize
};

JSBool MFTime::toJsval(const std::vector<double> & times,
                       JSContext * const cx,
                       JSObject * const obj,
                       jsval * const rval)
    throw ()
{
    JSObject * const mftimeObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mftimeObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(times.size());
    if (!setLength(cx, mftimeObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata =
        static_cast<MFData *>(JS_GetPrivate(cx, mftimeObj));
    for (size_t i = 0; i < times.size(); ++i) {
        if (!JS_NewDoubleValue(cx, times[i], &mfdata->array[i])) {
            return JS_FALSE;
        }
    }
    *rval = OBJECT_TO_JSVAL(mftimeObj);
    return JS_TRUE;
}

std::auto_ptr<openvrml::mftime>
MFTime::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (bad_conversion, std::bad_alloc)
{
    assert(cx);
    assert(obj);

    if (!JS_InstanceOf(cx, obj, &MFTime::jsclass, 0)) {
        throw bad_conversion("MFTime object expected.");
    }
    MField::MFData * const mfdata =
        static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);
    std::auto_ptr<openvrml::mftime>
        mftime(new openvrml::mftime(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_DOUBLE(mfdata->array[i]));
        mftime->value[i] = *JSVAL_TO_DOUBLE(mfdata->array[i]);
    }
    return mftime;
}

JSClass MFVec2f::jsclass = {
    "MFVec2f",
    JSCLASS_HAS_PRIVATE,
    JS_PropertyStub,
    JS_PropertyStub,
    getElement,
    setElement,
    JS_EnumerateStub,
    JS_ResolveStub,
    JS_ConvertStub,
    finalize
};

JSClass & MFVec2f::sfjsclass = SFVec2f::jsclass;

JSBool MFVec2f::toJsval(const std::vector<vec2f> & vec2fs,
                        JSContext * const cx,
                        JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    JSObject * const mfvec2fObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mfvec2fObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(vec2fs.size());
    if (!setLength(cx, mfvec2fObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata =
        static_cast<MFData *>(JS_GetPrivate(cx, mfvec2fObj));
    for (size_t i = 0; i < vec2fs.size(); ++i) {
        if (!SFVec2f::toJsval(vec2fs[i], cx, obj, &mfdata->array[i])) {
            return JS_FALSE;
        }
    }
    *rval = OBJECT_TO_JSVAL(mfvec2fObj);
    return JS_TRUE;
}

std::auto_ptr<openvrml::mfvec2f>
MFVec2f::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (bad_conversion, std::bad_alloc)
{
    assert(cx);
    assert(obj);
    if (!JS_InstanceOf(cx, obj, &MFVec2f::jsclass, 0)) {
        throw bad_conversion("MFVec2f object expected.");
    }
    MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);
    std::auto_ptr<openvrml::mfvec2f>
            mfvec2f(new openvrml::mfvec2f(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_OBJECT(mfdata->array[i]));
        assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                             &SFVec2f::jsclass, 0));
        const sfield::sfdata * const sfdata =
            static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
        assert(sfdata);

        const openvrml::sfvec2f & sfvec2f =
            static_cast<openvrml::sfvec2f &>(sfdata->field_value());
        mfvec2f->value[i] = sfvec2f.value;
    }
    return mfvec2f;
}

JSClass MFVec3f::jsclass = {
    "MFVec3f",
    JSCLASS_HAS_PRIVATE,
    JS_PropertyStub,
    JS_PropertyStub,
    getElement,
    setElement,
    JS_EnumerateStub,
    JS_ResolveStub,
    JS_ConvertStub,
    finalize
};

JSClass & MFVec3f::sfjsclass = SFVec3f::jsclass;

JSBool MFVec3f::toJsval(const std::vector<vec3f> & vec3fs,
                        JSContext * const cx,
                        JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    JSObject * const mfvec3fObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mfvec3fObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(vec3fs.size());
    if (!setLength(cx, mfvec3fObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata =
        static_cast<MFData *>(JS_GetPrivate(cx, mfvec3fObj));
    for (size_t i = 0; i < vec3fs.size(); ++i) {
        if (!SFVec3f::toJsval(vec3fs[i], cx, obj, &mfdata->array[i])) {
            return JS_FALSE;
        }
    }
    *rval = OBJECT_TO_JSVAL(mfvec3fObj);
    return JS_TRUE;
}

std::auto_ptr<openvrml::mfvec3f>
MFVec3f::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (bad_conversion, std::bad_alloc)
{
    assert(cx);
    assert(obj);
    if (!JS_InstanceOf(cx, obj, &MFVec3f::jsclass, 0)) {
        throw bad_conversion("MFVec3f object expected.");
    }
    MField::MFData * const mfdata =
        static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);
    std::auto_ptr<openvrml::mfvec3f>
        mfvec3f(new openvrml::mfvec3f(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0;
            i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_OBJECT(mfdata->array[i]));
        assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                             &SFVec3f::jsclass, 0));
        const sfield::sfdata * const sfdata =
            static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
        assert(sfdata);

        const openvrml::sfvec3f & sfvec3f =
            static_cast<openvrml::sfvec3f &>(sfdata->field_value());
        mfvec3f->value[i] = sfvec3f.value;
    }

    return mfvec3f;
}

JSClass VrmlMatrix::Row::jsclass = {
    "VrmlMatrixRow_",
    JSCLASS_HAS_PRIVATE,
    JS_PropertyStub,
    JS_PropertyStub,
    getElement,
    setElement,
    JS_EnumerateStub,
    JS_ResolveStub,
    JS_ConvertStub,
    JS_FinalizeStub
};

JSObject * VrmlMatrix::Row::initClass(JSContext * const cx,
                                      JSObject * const obj)
    throw ()
{
    return JS_InitClass(cx, obj, 0, &jsclass,
                        construct, 0,
                        0, 0,
                        0, 0);
}

JSBool VrmlMatrix::Row::construct(JSContext * const cx,
                                  JSObject * obj,
                                  const uintN argc,
                                  jsval * const argv,
                                  jsval * const rval)
    throw ()
{
    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return JS_TRUE;
}

JSBool VrmlMatrix::Row::getElement(JSContext * const cx,
                                   JSObject * const obj,
                                   const jsval id,
                                   jsval * const vp)
    throw ()
{
    assert(cx);
    assert(obj);

    if (JSVAL_IS_INT(id) && JSVAL_TO_INT(id) >= 0 && JSVAL_TO_INT(id) < 4) {
        assert(JS_GetPrivate(cx, obj));
        const float (&row)[4] =
            *static_cast<float (*)[4]>(JS_GetPrivate(cx, obj));

        if (!JS_NewDoubleValue(cx, row[JSVAL_TO_INT(id)], vp)) {
            return JS_FALSE;
        }
    }
    return JS_TRUE;
}

JSBool VrmlMatrix::Row::setElement(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp)
    throw ()
{
    assert(cx);
    assert(obj);

    if (JSVAL_IS_INT(id) && JSVAL_TO_INT(id) >= 0 && JSVAL_TO_INT(id) < 4) {
        jsdouble d;
        if (!JS_ValueToNumber(cx, *vp, &d)) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, obj));
        float (&row)[4] = *static_cast<float (*)[4]>(JS_GetPrivate(cx, obj));
        row[JSVAL_TO_INT(id)] = d;
    }
    return JS_TRUE;
}

JSClass VrmlMatrix::jsclass = {
    "VrmlMatrix",
    JSCLASS_HAS_PRIVATE,
    JS_PropertyStub,
    JS_PropertyStub,
    getElement,
    setElement,
    JS_EnumerateStub,
    JS_ResolveStub,
    JS_ConvertStub,
    finalize
};

JSObject * VrmlMatrix::initClass(JSContext * const cx, JSObject * const obj)
    throw ()
{
    static JSFunctionSpec methods[] =
            { { "setTransform", setTransform, 0, 0, 0 },
              { "getTransform", getTransform, 0, 0, 0 },
              { "inverse", inverse, 0, 0, 0 },
              { "transpose", transpose, 0, 0, 0 },
              { "multLeft", multLeft, 1, 0, 0 },
              { "multRight", multRight, 1, 0, 0 },
              { "multVecMatrix", multVecMatrix, 1, 0, 0 },
              { "multMatrixVec", multMatrixVec, 1, 0, 0 },
              { "toString", toString, 0, 0, 0 },
              { 0, 0, 0, 0, 0 } };

    JSObject * const proto = JS_InitClass(cx, obj, 0, &jsclass,
                                          construct, 0,
                                          0, methods,
                                          0, 0);
    if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
    return proto;
}

JSBool VrmlMatrix::construct(JSContext * const cx,
                             JSObject * obj,
                             const uintN argc,
                             jsval * const argv,
                             jsval * const rval)
    throw ()
{
    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return initObject(cx, obj, argc, argv);
}

JSBool VrmlMatrix::initObject(JSContext * const cx,
                              JSObject * const obj,
                              uintN argc,
                              jsval * const argv)
    throw ()
{
    assert(cx);
    assert(obj);

    argc = (argc < 16)
         ? argc
         : 16;
    try {
        std::auto_ptr<mat4f> mat(new mat4f);
        for (uintN i = 0; i < argc; ++i) {
            jsdouble d;
            if (!JS_ValueToNumber(cx, argv[i], &d)) { return JS_FALSE; }
            (&(*mat)[0][0])[i] = d;
        }
        if (!JS_SetPrivate(cx, obj, mat.get())) { return JS_FALSE; }
        mat.release();
    } catch (std::bad_alloc &) {
        JS_ReportOutOfMemory(cx);
        return JS_FALSE;
    }
    return JS_TRUE;
}

JSBool VrmlMatrix::getElement(JSContext * const cx,
                              JSObject * const obj,
                              const jsval id,
                              jsval * const vp)
    throw ()
{
    assert(cx);
    assert(obj);

    if (JSVAL_IS_INT(id) && JSVAL_TO_INT(id) >= 0 && JSVAL_TO_INT(id) < 4) {
        mat4f * const thisMat = static_cast<mat4f *>(JS_GetPrivate(cx, obj));

        //
        // Construct the result object.
        //
        JSObject * const robj = JS_ConstructObject(cx, &Row::jsclass, 0,
                                                   JS_GetParent(cx, obj));
        if (!robj) { return JS_FALSE; }

        float (&row)[4] = (*thisMat)[JSVAL_TO_INT(id)];
        if (!JS_SetPrivate(cx, robj, &row)) { return JS_FALSE; }
        *vp = OBJECT_TO_JSVAL(robj);
    }
    return JS_TRUE;
}

JSBool VrmlMatrix::setElement(JSContext * const cx,
                              JSObject * const obj,
                              const jsval id,
                              jsval * const vp)
    throw ()
{
    return JS_TRUE;
}

JSBool VrmlMatrix::setTransform(JSContext * const cx,
                                JSObject * const obj,
                                const uintN argc,
                                jsval * const argv,
                                jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);

    using openvrml::sfvec3f;
    using openvrml::sfrotation;

    vec3f translation(0.0, 0.0, 0.0);
    rotation rot(0.0, 0.0, 1.0, 0.0);
    vec3f scale(1.0, 1.0, 1.0);
    rotation scaleOrientation(0.0, 0.0, 1.0, 0.0);
    vec3f center(0.0, 0.0, 0.0);

    for (uintN i = 0; i < argc; ++i) {
        JSObject * arg_obj;
        if (!JS_ValueToObject(cx, argv[i], &arg_obj)) { return JS_FALSE; }
        assert(JS_GetPrivate(cx, arg_obj));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, arg_obj));
        switch (i) {
        case 0:
            if (!JS_InstanceOf(cx, arg_obj, &js_::SFVec3f::jsclass, argv)) {
                return JS_FALSE;
            }
            assert(dynamic_cast<sfvec3f *>(&sfdata.field_value()));
            translation = static_cast<sfvec3f &>(sfdata.field_value()).value;
            break;

        case 1:
            if (!JS_InstanceOf(cx, arg_obj, &js_::SFRotation::jsclass, argv)) {
                return JS_FALSE;
            }
            assert(dynamic_cast<sfrotation *>(&sfdata.field_value()));
            rot = static_cast<sfrotation &>(sfdata.field_value()).value;
            break;

        case 2:
            if (!JS_InstanceOf(cx, arg_obj, &js_::SFVec3f::jsclass, argv)) {
                return JS_FALSE;
            }
            assert(dynamic_cast<sfvec3f *>(&sfdata.field_value()));
            scale = static_cast<sfvec3f &>(sfdata.field_value()).value;
            break;

        case 3:
            if (!JS_InstanceOf(cx, arg_obj, &js_::SFRotation::jsclass, argv)) {
                return JS_FALSE;
            }
            assert(dynamic_cast<sfrotation *>(&sfdata.field_value()));
            scaleOrientation =
                static_cast<sfrotation &>(sfdata.field_value()).value;
            break;

        case 4:
            if (!JS_InstanceOf(cx, arg_obj, &js_::SFVec3f::jsclass, argv)) {
                return JS_FALSE;
            }
            assert(dynamic_cast<sfvec3f *>(&sfdata.field_value()));
            center = static_cast<sfvec3f &>(sfdata.field_value()).value;
            break;

        default:
            break;
        }
    }

    mat4f * const thisMat = static_cast<mat4f *>(JS_GetPrivate(cx, obj));
    assert(thisMat);

    *thisMat = mat4f::transformation(translation,
                                     rot,
                                     scale,
                                     scaleOrientation,
                                     center);
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

JSBool VrmlMatrix::getTransform(JSContext * const cx,
                                JSObject * const obj,
                                const uintN argc,
                                jsval * const argv,
                                jsval * const rval)
    throw ()
{
    using openvrml::sfvec3f;
    using openvrml::sfrotation;

    assert(cx);
    assert(obj);

    vec3f translation;
    rotation rot;
    vec3f scale;

    const mat4f * const thisMat = static_cast<mat4f *>(JS_GetPrivate(cx, obj));
    assert(thisMat);

    thisMat->transformation(translation, rot, scale);

    for (uintN i = 0; i < argc; ++i) {
        if (JSVAL_IS_NULL(argv[i])) { continue; }
        JSObject * arg_obj;
        if (!JS_ValueToObject(cx, argv[i], &arg_obj)) { return JS_FALSE; }
        assert(JS_GetPrivate(cx, arg_obj));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, arg_obj));
        switch (i) {
        case 0:
            if (!JS_InstanceOf(cx, arg_obj, &js_::SFVec3f::jsclass, argv)) {
                return JS_FALSE;
            }
            assert(dynamic_cast<sfvec3f *>(&sfdata.field_value()));
            static_cast<sfvec3f &>(sfdata.field_value()).value = translation;
            break;

        case 1:
            if (!JS_InstanceOf(cx, arg_obj, &js_::SFRotation::jsclass, argv)) {
                return JS_FALSE;
            }
            assert(dynamic_cast<sfrotation *>(&sfdata.field_value()));
            static_cast<sfrotation &>(sfdata.field_value()).value = rot;
            break;

        case 2:
            if (!JS_InstanceOf(cx, arg_obj, &js_::SFVec3f::jsclass, argv)) {
                return JS_FALSE;
            }
            assert(dynamic_cast<sfvec3f *>(&sfdata.field_value()));
            static_cast<sfvec3f &>(sfdata.field_value()).value = scale;
            break;

        default:
            break;
        }
    }
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

JSBool VrmlMatrix::inverse(JSContext * const cx,
                           JSObject * const obj,
                           const uintN argc,
                           jsval * const argv,
                           jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);

    JSObject * robj = JS_ConstructObject(cx, &VrmlMatrix::jsclass, 0,
                                         JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    const mat4f * const thisMat = static_cast<mat4f *>(JS_GetPrivate(cx, obj));
    assert(thisMat);

    mat4f * const newMat = static_cast<mat4f *>(JS_GetPrivate(cx, robj));
    assert(newMat);
    *newMat = thisMat->inverse();
    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool VrmlMatrix::transpose(JSContext * const cx,
                             JSObject * const obj,
                             const uintN argc,
                             jsval * const argv,
                             jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);

    JSObject * robj = JS_ConstructObject(cx, &VrmlMatrix::jsclass, 0,
                                         JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    const mat4f * const thisMat = static_cast<mat4f *>(JS_GetPrivate(cx, obj));
    assert(thisMat);

    mat4f * const newMat = static_cast<mat4f *>(JS_GetPrivate(cx, robj));
    assert(newMat);
    *newMat = thisMat->transpose();
    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool VrmlMatrix::multLeft(JSContext * const cx,
                            JSObject * const obj,
                            const uintN argc,
                            jsval * const argv,
                            jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);

    JSObject * arg_obj;
    if (!JS_ValueToObject(cx, argv[0], &arg_obj)) { return JS_FALSE; }
    if (!JS_InstanceOf(cx, arg_obj, &VrmlMatrix::jsclass, argv)) {
        return JS_FALSE;
    }

    const mat4f * const argMat =
        static_cast<mat4f *>(JS_GetPrivate(cx, arg_obj));
    assert(argMat);

    const mat4f * const thisMat = static_cast<mat4f *>(JS_GetPrivate(cx, obj));
    assert(thisMat);

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &VrmlMatrix::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    mat4f * const resultMat = static_cast<mat4f *>(JS_GetPrivate(cx, robj));
    assert(resultMat);

    *resultMat = *argMat * *thisMat;
    return JS_TRUE;
}

JSBool VrmlMatrix::multRight(JSContext * const cx,
                             JSObject * const obj,
                             const uintN argc,
                             jsval * const argv,
                             jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(argc >= 1);

    JSObject * arg_obj;
    if (!JS_ValueToObject(cx, argv[0], &arg_obj)) { return JS_FALSE; }
    if (!JS_InstanceOf(cx, arg_obj, &VrmlMatrix::jsclass, argv)) {
        return JS_FALSE;
    }

    assert(JS_GetPrivate(cx, arg_obj));
    const mat4f & argMat = *static_cast<mat4f *>(JS_GetPrivate(cx, arg_obj));

    assert(JS_GetPrivate(cx, obj));
    const mat4f & thisMat = *static_cast<mat4f *>(JS_GetPrivate(cx, obj));

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &VrmlMatrix::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, robj));
    mat4f & resultMat = *static_cast<mat4f *>(JS_GetPrivate(cx, robj));

    resultMat = thisMat * argMat;
    return JS_TRUE;
}

JSBool VrmlMatrix::multVecMatrix(JSContext * const cx,
                                 JSObject * const obj,
                                 const uintN argc,
                                 jsval * const argv,
                                 jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);

    //
    // Make sure argument is an SFVec3f.
    //
    JSObject * arg_obj;
    if (!JS_ValueToObject(cx, argv[0], &arg_obj)) { return JS_FALSE; }
    if (!JS_InstanceOf(cx, arg_obj, &SFVec3f::jsclass, argv)) {
        return JS_FALSE;
    }

    assert(JS_GetPrivate(cx, arg_obj));
    const sfield::sfdata & arg_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, arg_obj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&arg_sfdata.field_value()));
    const openvrml::sfvec3f & argVec =
            static_cast<openvrml::sfvec3f &>(arg_sfdata.field_value());

    assert(JS_GetPrivate(cx, obj));
    const mat4f & thisMat = *static_cast<mat4f *>(JS_GetPrivate(cx, obj));

    //
    // Construct the result object.
    //
    static JSObject * const proto = 0;
    JSObject * const parent = JS_GetParent(cx, obj);
    JSObject * const robj =
        JS_ConstructObject(cx, &SFVec3f::jsclass, proto, parent);
    if (!robj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, robj));
    sfield::sfdata & robj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&robj_sfdata.field_value()));
    openvrml::sfvec3f & resultVec =
        static_cast<openvrml::sfvec3f &>(robj_sfdata.field_value());

    resultVec.value = argVec.value * thisMat;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool VrmlMatrix::multMatrixVec(JSContext * const cx,
                                 JSObject * const obj,
                                 const uintN argc,
                                 jsval * const argv,
                                 jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);

    //
    // Make sure argument is an SFVec3f.
    //
    JSObject * arg_obj;
    if (JS_ValueToObject(cx, argv[0], &arg_obj)) { return JS_FALSE; }
    if (!JS_InstanceOf(cx, arg_obj, &SFVec3f::jsclass, argv)) {
        return JS_FALSE;
    }

    const sfield::sfdata & arg_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, arg_obj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&arg_sfdata.field_value()));
    const openvrml::sfvec3f & argVec =
        static_cast<openvrml::sfvec3f &>(arg_sfdata.field_value());

    assert(JS_GetPrivate(cx, obj));
    const mat4f & thisMat = *static_cast<mat4f *>(JS_GetPrivate(cx, obj));

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    assert(JS_GetPrivate(cx, robj));
    sfield::sfdata & robj_sfdata =
        *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
    assert(dynamic_cast<openvrml::sfvec3f *>(&robj_sfdata.field_value()));
    openvrml::sfvec3f & resultVec =
        static_cast<openvrml::sfvec3f &>(robj_sfdata.field_value());

    resultVec.value = thisMat * argVec.value;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool VrmlMatrix::toString(JSContext * const cx,
                            JSObject * const obj,
                            const uintN argc,
                            jsval * const argv,
                            jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(rval);

    assert(JS_GetPrivate(cx, obj));
    const mat4f & thisMat = *static_cast<mat4f *>(JS_GetPrivate(cx, obj));

    JSString * jsstr = 0;
    try {
        std::ostringstream out;
        out << thisMat;
        jsstr = JS_NewStringCopyZ(cx, out.str().c_str());
        if (!jsstr) { return JS_FALSE; }
    } catch (std::bad_alloc &) {
        JS_ReportOutOfMemory(cx);
        return JS_FALSE;
    }
    *rval = STRING_TO_JSVAL(jsstr);
    return JS_TRUE;
}

void VrmlMatrix::finalize(JSContext * const cx, JSObject * const obj) throw ()
{
    delete static_cast<mat4f *>(JS_GetPrivate(cx, obj));
    JS_SetPrivate(cx, obj, 0);
}

} // namespace js_

} // namespace
# endif // OPENVRML_ENABLE_SCRIPT_NODE_JAVASCRIPT

} // namespace openvrml
