// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 2004  Braden McDaniel
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

# include "event.h"

namespace openvrml {

/**
 * @class event_listener
 *
 * @brief Abstract base class of event listeners.
 */

/**
 * @internal
 *
 * @var node & event_listener::node_
 *
 * @brief The node to which the event_listener belongs.
 */

/**
 * @brief Construct.
 *
 * @param node  the node to which the event_listener belongs.
 */
event_listener::event_listener(openvrml::node & node) throw ():
    node_(node)
{}

/**
 * @brief Destroy.
 */
event_listener::~event_listener() throw ()
{}

/**
 * @brief The <code>node</code> to which the <code>event_listener</code>
 *        belongs.
 *
 * @return the <code>node</code> to which the <code>event_listener</code>
 *         belongs.
 */
node & event_listener::node() throw ()
{
    return this->node_;
}

/**
 * @fn field_value::type_id event_listener::type() const throw ()
 *
 * @brief The <code>field_value::type_id</code> for the type accepted by the
 *        event listener.
 *
 * @return The <code>field_value::type_id</code> for the type accepted by the
 *         event listener.
 */


/**
 * @class field_value_listener
 *
 * @brief Concrete event listener template.
 */

/**
 * @fn field_value_listener<FieldValue>::field_value_listener(openvrml::node & node) throw ()
 *
 * @brief Construct.
 *
 * @param node  the node to which the event_listener belongs.
 */

/**
 * @fn field_value_listener<FieldValue>::~field_value_listener() throw ()
 *
 * @brief Destroy.
 */

/**
 * @fn field_value::type_id field_value_listener<FieldValue>::type() const throw ()
 *
 * @brief <code>FieldValue::field_value_type_id</code>.
 *
 * @return <code>FieldValue::field_value_type_id</code>.
 */

/**
 * @fn void field_value_listener<FieldValue>::process_event(const FieldValue & value, double timestamp) throw (std::bad_alloc)
 *
 * @brief Process an event.
 *
 * @param value     the event value.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @typedef sfbool_listener
 *
 * @brief sfbool event listener.
 */

/**
 * @typedef sfcolor_listener
 *
 * @brief sfcolor event listener.
 */

/**
 * @typedef sffloat_listener
 *
 * @brief sffloat event listener.
 */

/**
 * @typedef sfimage_listener
 *
 * @brief sfimage event listener.
 */

/**
 * @typedef sfint32_listener
 *
 * @brief sfint32 event listener.
 */

/**
 * @typedef sfnode_listener
 *
 * @brief sfnode event listener.
 */

/**
 * @typedef sfrotation_listener
 *
 * @brief sfrotation event listener.
 */

/**
 * @typedef sfstring_listener
 *
 * @brief sfstring event listener.
 */

/**
 * @typedef sftime_listener
 *
 * @brief sftime event listener.
 */

/**
 * @typedef sfvec2f_listener
 *
 * @brief sfvec2f event listener.
 */

/**
 * @typedef sfvec3f_listener
 *
 * @brief sfvec3f event listener.
 */

/**
 * @typedef mfcolor_listener
 *
 * @brief mfcolor event listener.
 */

/**
 * @typedef mffloat_listener
 *
 * @brief mffloat event listener.
 */

/**
 * @typedef mfint32_listener
 *
 * @brief mfint32 event listener.
 */

/**
 * @typedef mfnode_listener
 *
 * @brief mfnode event listener.
 */

/**
 * @typedef mfrotation_listener
 *
 * @brief mfrotation event listener.
 */

/**
 * @typedef mfstring_listener
 *
 * @brief mfstring event listener.
 */

/**
 * @typedef mftime_listener
 *
 * @brief mftime event listener.
 */

/**
 * @typedef mfvec2f_listener
 *
 * @brief mfvec2f event listener.
 */

/**
 * @typedef mfvec3f_listener
 *
 * @brief mfvec3f event listener.
 */


/**
 * @class event_emitter
 *
 * @brief Abstract base class of event emitters.
 */

/**
 * @brief Create an event_emitter.
 *
 * @param value value to emit.
 *
 * @return an event_emitter.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<event_emitter>
event_emitter::create(const field_value & value) throw (std::bad_alloc)
{
    std::auto_ptr<event_emitter> emitter;
    switch (value.type()) {
    case field_value::sfbool_id:
        emitter.reset(new sfbool_emitter(static_cast<const sfbool &>(value)));
        break;
    case field_value::sfcolor_id:
        emitter.reset(
            new sfcolor_emitter(static_cast<const sfcolor &>(value)));
        break;
    case field_value::sffloat_id:
        emitter.reset(
            new sffloat_emitter(static_cast<const sffloat &>(value)));
        break;
    case field_value::sfimage_id:
        emitter.reset(
            new sfimage_emitter(static_cast<const sfimage &>(value)));
        break;
    case field_value::sfint32_id:
        emitter.reset(
            new sfint32_emitter(static_cast<const sfint32 &>(value)));
        break;
    case field_value::sfnode_id:
        emitter.reset(new sfnode_emitter(static_cast<const sfnode &>(value)));
        break;
    case field_value::sfstring_id:
        emitter.reset(
            new sfstring_emitter(static_cast<const sfstring &>(value)));
        break;
    case field_value::sfrotation_id:
        emitter.reset(
            new sfrotation_emitter(static_cast<const sfrotation &>(value)));
        break;
    case field_value::sftime_id:
        emitter.reset(new sftime_emitter(static_cast<const sftime &>(value)));
        break;
    case field_value::sfvec2f_id:
        emitter.reset(
            new sfvec2f_emitter(static_cast<const sfvec2f &>(value)));
        break;
    case field_value::sfvec3f_id:
        emitter.reset(
            new sfvec3f_emitter(static_cast<const sfvec3f &>(value)));
        break;
    case field_value::mfcolor_id:
        emitter.reset(
            new mfcolor_emitter(static_cast<const mfcolor &>(value)));
        break;
    case field_value::mffloat_id:
        emitter.reset(
            new mffloat_emitter(static_cast<const mffloat &>(value)));
        break;
    case field_value::mfint32_id:
        emitter.reset(
            new mfint32_emitter(static_cast<const mfint32 &>(value)));
        break;
    case field_value::mfnode_id:
        emitter.reset(new mfnode_emitter(static_cast<const mfnode &>(value)));
        break;
    case field_value::mfstring_id:
        emitter.reset(
            new mfstring_emitter(static_cast<const mfstring &>(value)));
        break;
    case field_value::mfrotation_id:
        emitter.reset(
            new mfrotation_emitter(static_cast<const mfrotation &>(value)));
        break;
    case field_value::mftime_id:
        emitter.reset(new mftime_emitter(static_cast<const mftime &>(value)));
        break;
    case field_value::mfvec2f_id:
        emitter.reset(
            new mfvec2f_emitter(static_cast<const mfvec2f &>(value)));
        break;
    case field_value::mfvec3f_id:
        emitter.reset(
            new mfvec3f_emitter(static_cast<const mfvec3f &>(value)));
        break;
    default:
        assert(false);
    }
    return emitter;
}

/**
 * @internal
 *
 * @var const field_value & event_emitter::value_
 *
 * @brief A reference to the field_value for the event_emitter.
 */

/**
 * @typedef event_emitter::listener_set
 *
 * @brief Set of event_listeners.
 */

/**
 * @var event_emitter::listener_set event_emitter::listeners_
 *
 * @brief The listeners registered for this emitter.
 *
 * When emit_event is called, each of the registered listeners will be sent an
 * event.
 */

/**
 * @var double event_emitter::last_time_
 *
 * @brief The timestamp of the last event emitted.
 */

/**
 * @brief Construct.
 *
 * @param value field_value associated with this emitter.
 */
event_emitter::event_emitter(const field_value & value) throw ():
    value_(value)
{}

/**
 * @brief Destroy.
 */
event_emitter::~event_emitter() throw ()
{}

/**
 * @brief A reference to the <code>field_value</code> for the
 *        <code>event_emitter</code>.
 *
 * @return a reference to the <code>field_value</code> for the
 *         <code>event_emitter</code>.
 */
const field_value & event_emitter::value() const throw ()
{
    return this->value_;
}

/**
 * @brief Registered listeners.
 *
 * @return the set of registered event_listeners.
 */
const event_emitter::listener_set & event_emitter::listeners() const throw ()
{
    return this->listeners_;
}

/**
 * @brief Registered listeners.
 *
 * @return the set of registered event_listeners.
 */
event_emitter::listener_set & event_emitter::listeners() throw ()
{
    return this->listeners_;
}

/**
 * @brief The timestamp of the last event emitted.
 *
 * @return the timestamp of the last event emitted.
 */
double event_emitter::last_time() const throw ()
{
    return this->last_time_;
}

/**
 * @brief Set the timestamp of the last event emitted.
 *
 * @param t the timestamp of the last event emitted.
 */
void event_emitter::last_time(const double t) throw ()
{
    this->last_time_ = t;
}

/**
 * @fn void event_emitter::emit_event(double timestamp) throw (std::bad_alloc)
 *
 * @brief Emit an event.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * This function is called by node::emit_event.
 */


/**
 * @class field_value_emitter
 *
 * @brief Concrete event emitter template.
 */

/**
 * @fn field_value_emitter::field_value_emitter(const FieldValue & value) throw ()
 *
 * @brief Construct.
 */

/**
 * @fn field_value_emitter::~field_value_emitter() throw ()
 *
 * @brief Destroy.
 */

/**
 * @fn void field_value_emitter::emit_event(double timestamp) throw (std::bad_alloc)
 *
 * @brief Emit an event.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @fn bool field_value_emitter::add(field_value_listener<FieldValue> & listener) throw (std::bad_alloc)
 *
 * @brief Add an event listener.
 *
 * @param listener  the listener to add.
 *
 * @return @c true if @p listener was added; @c false if @p listener was not
 *         added (if it was already registered for the emitter).
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @fn bool field_value_emitter::remove(field_value_listener<FieldValue> & listener)
 *
 * @brief Remove an event listener.
 *
 * @param listener  the listener to remove.
 *
 * @return @c true if @p listener was removed; @c false if @p listener was not
 *         removed (if it was not registered for the emitter).
 */

/**
 * @typedef sfbool_emitter
 *
 * @brief sfbool event emitter.
 */

/**
 * @typedef sfcolor_emitter
 *
 * @brief sfcolor event emitter.
 */

/**
 * @typedef sffloat_emitter
 *
 * @brief sffloat event emitter.
 */

/**
 * @typedef sfimage_emitter
 *
 * @brief sfimage event emitter.
 */

/**
 * @typedef sfint32_emitter
 *
 * @brief sfint32 event emitter.
 */

/**
 * @typedef sfnode_emitter
 *
 * @brief sfnode event emitter.
 */

/**
 * @typedef sfrotation_emitter
 *
 * @brief sfrotation event emitter.
 */

/**
 * @typedef sfstring_emitter
 *
 * @brief sfstring event emitter.
 */

/**
 * @typedef sftime_emitter
 *
 * @brief sftime event emitter.
 */

/**
 * @typedef sfvec2f_emitter
 *
 * @brief sfvec2f event emitter.
 */

/**
 * @typedef sfvec3f_emitter
 *
 * @brief sfvec3f event emitter.
 */

/**
 * @typedef mfcolor_emitter
 *
 * @brief mfcolor event emitter.
 */

/**
 * @typedef mffloat_emitter
 *
 * @brief mffloat event emitter.
 */

/**
 * @typedef mfint32_emitter
 *
 * @brief mfint32 event emitter.
 */

/**
 * @typedef mfnode_emitter
 *
 * @brief mfnode event emitter.
 */

/**
 * @typedef mfrotation_emitter
 *
 * @brief mfrotation event emitter.
 */

/**
 * @typedef mfstring_emitter
 *
 * @brief mfstring event emitter.
 */

/**
 * @typedef mftime_emitter
 *
 * @brief mftime event emitter.
 */

/**
 * @typedef mfvec2f_emitter
 *
 * @brief mfvec2f event emitter.
 */

/**
 * @typedef mfvec3f_emitter
 *
 * @brief mfvec3f event emitter.
 */

} // namespace openvrml
