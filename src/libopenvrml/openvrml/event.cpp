// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2004, 2005  Braden McDaniel
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

/**
 * @class openvrml::event_listener
 *
 * @brief Abstract base class of event listeners.
 */

/**
 * @brief Construct.
 */
openvrml::event_listener::event_listener() OPENVRML_NOTHROW
{}

/**
 * @brief Destroy.
 */
openvrml::event_listener::~event_listener() OPENVRML_NOTHROW
{}

/**
 * @fn openvrml::field_value::type_id openvrml::event_listener::type() const
 *
 * @brief The @c field_value::type_id for the type accepted by the event
 *        listener.
 *
 * @return The @c field_value::type_id for the type accepted by the event
 *         listener.
 */


/**
 * @class openvrml::node_event_listener
 *
 * @brief Base class for event listeners of nodes.
 */

/**
 * @internal
 *
 * @var openvrml::node * openvrml::node_event_listener::node_
 *
 * @brief The @c node to which the @c node_event_listener belongs.
 */

/**
 * @brief Construct.
 *
 * @param[in] n the @c node to which the @c node_event_listener belongs.
 */
openvrml::node_event_listener::node_event_listener(openvrml::node & n)
    OPENVRML_NOTHROW:
    node_(&n)
{}

/**
 * @brief Destroy.
 */
openvrml::node_event_listener::~node_event_listener() OPENVRML_NOTHROW
{}

/**
 * @brief The @c node to which the @c node_event_listener belongs.
 *
 * @return the @c node to which the @c node_event_listener belongs.
 */
openvrml::node & openvrml::node_event_listener::node() const OPENVRML_NOTHROW
{
    return *this->node_;
}

/**
 * @brief The associated @c eventIn identifier.
 *
 * This function delegates to @c node_event_listener::do_eventin_id.
 *
 * @return the associated @c eventIn identifier.
 */
const std::string openvrml::node_event_listener::eventin_id() const
    OPENVRML_NOTHROW
{
    return this->do_eventin_id();
}

/**
 * @fn const std::string openvrml::node_event_listener::do_eventin_id() const
 *
 * @brief The associated @c eventIn identifier.
 *
 * Concrete subclasses must implement this function.
 *
 * @return the associated @c eventIn identifier.
 */


/**
 * @class openvrml::field_value_listener
 *
 * @brief Concrete event listener template.
 */

/**
 * @fn openvrml::field_value_listener::field_value_listener()
 *
 * @brief Construct.
 */

/**
 * @fn openvrml::field_value_listener::~field_value_listener()
 *
 * @brief Destroy.
 */

/**
 * @fn openvrml::field_value::type_id openvrml::field_value_listener::type() const
 *
 * @brief @c FieldValue::field_value_type_id.
 *
 * @return @c FieldValue::field_value_type_id.
 */

/**
 * @fn void openvrml::field_value_listener::process_event(const FieldValue & value, double timestamp)
 *
 * @brief Process an event.
 *
 * @param[in] value     the event value.
 * @param[in] timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @fn void openvrml::field_value_listener::do_process_event(const FieldValue & value, double timestamp)
 *
 * @brief Called by @c field_value_listener<FieldValue>::do_process_event.
 *
 * Subclasses must implement this function.
 *
 * @param[in] value     the event value.
 * @param[in] timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */


/**
 * @typedef openvrml::sfbool_listener
 *
 * @brief @c sfbool event listener.
 */

/**
 * @typedef openvrml::sfcolor_listener
 *
 * @brief @c sfcolor event listener.
 */

/**
 * @typedef openvrml::sfcolorrgba_listener
 *
 * @brief @c sfcolorrgba event listener.
 */

/**
 * @typedef openvrml::sfdouble_listener
 *
 * @brief @c sfdouble event listener.
 */

/**
 * @typedef openvrml::sffloat_listener
 *
 * @brief @c sffloat event listener.
 */

/**
 * @typedef openvrml::sfimage_listener
 *
 * @brief @c sfimage event listener.
 */

/**
 * @typedef openvrml::sfint32_listener
 *
 * @brief @c sfint32 event listener.
 */

/**
 * @typedef openvrml::sfnode_listener
 *
 * @brief @c sfnode event listener.
 */

/**
 * @typedef openvrml::sfrotation_listener
 *
 * @brief @c sfrotation event listener.
 */

/**
 * @typedef openvrml::sfstring_listener
 *
 * @brief @c sfstring event listener.
 */

/**
 * @typedef openvrml::sftime_listener
 *
 * @brief @c sftime event listener.
 */

/**
 * @typedef openvrml::sfvec2d_listener
 *
 * @brief @c sfvec2d event listener.
 */

/**
 * @typedef openvrml::sfvec2f_listener
 *
 * @brief @c sfvec2f event listener.
 */

/**
 * @typedef openvrml::sfvec3d_listener
 *
 * @brief @c sfvec3d event listener.
 */

/**
 * @typedef openvrml::sfvec3f_listener
 *
 * @brief @c sfvec3f event listener.
 */

/**
 * @typedef openvrml::mfcolor_listener
 *
 * @brief @c mfcolor event listener.
 */

/**
 * @typedef openvrml::mfbool_listener
 *
 * @brief @c mfbool event listener.
 */

/**
 * @typedef openvrml::mfcolorrgba_listener
 *
 * @brief @c mfcolorrgba event listener.
 */

/**
 * @typedef openvrml::mfdouble_listener
 *
 * @brief @c mfdouble event listener.
 */

/**
 * @typedef openvrml::mffloat_listener
 *
 * @brief @c mffloat event listener.
 */

/**
 * @typedef openvrml::mfimage_listener
 *
 * @brief @c mfimage event listener.
 */

/**
 * @typedef openvrml::mfint32_listener
 *
 * @brief @c mfint32 event listener.
 */

/**
 * @typedef openvrml::mfnode_listener
 *
 * @brief @c mfnode event listener.
 */

/**
 * @typedef openvrml::mfrotation_listener
 *
 * @brief @c mfrotation event listener.
 */

/**
 * @typedef @c openvrml::mfstring_listener
 *
 * @brief @c mfstring event listener.
 */

/**
 * @typedef openvrml::mftime_listener
 *
 * @brief @c mftime event listener.
 */

/**
 * @typedef openvrml::mfvec2d_listener
 *
 * @brief @c mfvec2d event listener.
 */

/**
 * @typedef openvrml::mfvec2f_listener
 *
 * @brief @c mfvec2f event listener.
 */

/**
 * @typedef openvrml::mfvec3d_listener
 *
 * @brief @c mfvec3d event listener.
 */

/**
 * @typedef openvrml::mfvec3f_listener
 *
 * @brief @c mfvec3f event listener.
 */


/**
 * @class openvrml::node_field_value_listener
 *
 * @brief Base class for @c field_value_listener%s that are members of nodes.
 */

/**
 * @fn openvrml::node_field_value_listener::node_field_value_listener(openvrml::node & n)
 *
 * @brief Construct.
 *
 * @param[in] n the @c node of which the listener is a member.
 */

/**
 * @fn openvrml::node_field_value_listener::~node_field_value_listener()
 *
 * @brief Destroy.
 */


/**
 * @class openvrml::event_emitter
 *
 * @brief Abstract base class of event emitters.
 */

/**
 * @var class openvrml::event_emitter::node
 *
 * @brief The implementation of @c node calls @c event_emitter::emit_event.
 *
 * The only things that should be emitting events are @c node%s.  Subclasses
 * of @c node should call @c node::emit_event to emit an event.
 */

/**
 * @internal
 *
 * @var boost::recursive_mutex openvrml::event_emitter::mutex_
 *
 * @brief Object mutex.
 */

/**
 * @internal
 *
 * @var const openvrml::field_value & openvrml::event_emitter::value_
 *
 * @brief A reference to the @c field_value for the @c event_emitter.
 */

/**
 * @typedef openvrml::event_emitter::listener_set
 *
 * @brief Set of @c event_listener%s.
 */

/**
 * @internal
 *
 * @var openvrml::event_emitter::listener_set openvrml::event_emitter::listeners_
 *
 * @brief The listeners registered for this emitter.
 *
 * When @c emit_event is called, each of the registered listeners will be sent
 * an event.
 */

/**
 * @internal
 *
 * @var double openvrml::event_emitter::last_time_
 *
 * @brief The timestamp of the last event emitted.
 */

/**
 * @brief Construct.
 *
 * @param[in] value @c field_value associated with this emitter.
 */
openvrml::event_emitter::event_emitter(const field_value & value)
    OPENVRML_NOTHROW:
    value_(value),
    last_time_(0.0)
{}

/**
 * @brief Destroy.
 */
openvrml::event_emitter::~event_emitter() OPENVRML_NOTHROW
{}

/**
 * @brief Get the mutex for the @c event_emitter.
 *
 * @return a reference to the <code>event_emitter</code>'s mutex.
 */
boost::recursive_mutex & openvrml::event_emitter::mutex() const
    OPENVRML_NOTHROW
{
    return this->mutex_;
}

/**
 * @brief A reference to the @c field_value for the @c event_emitter.
 *
 * @return a reference to the @c field_value for the @c event_emitter.
 */
const openvrml::field_value & openvrml::event_emitter::value() const
    OPENVRML_NOTHROW
{
    return this->value_;
}

/**
 * @brief The associated @c eventOut identifier.
 *
 * This function delegates to @c event_emitter::do_eventout_id.
 *
 * @return the associated @c eventOut identifier.
 */
const std::string openvrml::event_emitter::eventout_id() const
    OPENVRML_NOTHROW
{
    return this->do_eventout_id();
}

/**
 * @fn const std::string openvrml::event_emitter::do_eventout_id() const
 *
 * @brief The associated @c eventOut identifier.
 *
 * Concrete subclasses must implement this function.
 *
 * @return the associated eventOut identifier.
 */

/**
 * @brief Registered listeners.
 *
 * @return the set of registered @c event_listener%s.
 */
openvrml::event_emitter::listener_set & openvrml::event_emitter::listeners()
    OPENVRML_NOTHROW
{
    return this->listeners_;
}

/**
 * @overload
 *
 * @return the set of registered @c event_listener%s.
 */
const openvrml::event_emitter::listener_set &
openvrml::event_emitter::listeners() const OPENVRML_NOTHROW
{
    return this->listeners_;
}

/**
 * @brief The timestamp of the last event emitted.
 *
 * @return the timestamp of the last event emitted.
 */
double openvrml::event_emitter::last_time() const OPENVRML_NOTHROW
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    return this->last_time_;
}

/**
 * @brief Set the timestamp of the last event emitted.
 *
 * @param[in] t the timestamp of the last event emitted.
 */
void openvrml::event_emitter::last_time(const double t) OPENVRML_NOTHROW
{
    boost::recursive_mutex::scoped_lock lock(this->mutex_);
    this->last_time_ = t;
}

/**
 * @fn void openvrml::event_emitter::emit_event(double timestamp)
 *
 * @brief Emit an event.
 *
 * @param[in] timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * This function is called by @c node::emit_event.
 */


/**
 * @class openvrml::field_value_emitter
 *
 * @brief Concrete event emitter template.
 */

/**
 * @fn openvrml::field_value_emitter::field_value_emitter(const FieldValue & value)
 *
 * @brief Construct.
 */

/**
 * @fn openvrml::field_value_emitter::~field_value_emitter()
 *
 * @brief Destroy.
 */

/**
 * @fn void openvrml::field_value_emitter::emit_event(double timestamp)
 *
 * @brief Emit an event.
 *
 * @param[in] timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @fn bool openvrml::field_value_emitter::add(field_value_listener<FieldValue> & listener)
 *
 * @brief Add an event listener.
 *
 * @param[in] listener  the listener to add.
 *
 * @return @c true if @p listener was added; @c false if @p listener was not
 *         added (if it was already registered for the emitter).
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @fn bool openvrml::field_value_emitter::remove(field_value_listener<FieldValue> & listener)
 *
 * @brief Remove an event listener.
 *
 * @param[in] listener  the listener to remove.
 *
 * @return @c true if @p listener was removed; @c false if @p listener was not
 *         removed (if it was not registered for the emitter).
 */

/**
 * @typedef openvrml::sfbool_emitter
 *
 * @brief @c sfbool event emitter.
 */

/**
 * @typedef openvrml::sfcolor_emitter
 *
 * @brief @c sfcolor event emitter.
 */

/**
 * @typedef openvrml::sfcolorrgba_emitter
 *
 * @brief @c sfcolorrgba event emitter.
 */

/**
 * @typedef openvrml::sfdouble_emitter
 *
 * @brief @c sfdouble event emitter.
 */

/**
 * @typedef openvrml::sffloat_emitter
 *
 * @brief @c sffloat event emitter.
 */

/**
 * @typedef openvrml::sfimage_emitter
 *
 * @brief @c sfimage event emitter.
 */

/**
 * @typedef openvrml::sfint32_emitter
 *
 * @brief @c sfint32 event emitter.
 */

/**
 * @typedef openvrml::sfnode_emitter
 *
 * @brief @c sfnode event emitter.
 */

/**
 * @typedef openvrml::sfrotation_emitter
 *
 * @brief @c sfrotation event emitter.
 */

/**
 * @typedef openvrml::sfstring_emitter
 *
 * @brief @c sfstring event emitter.
 */

/**
 * @typedef openvrml::sftime_emitter
 *
 * @brief @c sftime event emitter.
 */

/**
 * @typedef openvrml::sfvec2d_emitter
 *
 * @brief @c sfvec2d event emitter.
 */

/**
 * @typedef openvrml::sfvec2f_emitter
 *
 * @brief @c sfvec2f event emitter.
 */

/**
 * @typedef openvrml::sfvec3d_emitter
 *
 * @brief @c sfvec3d event emitter.
 */

/**
 * @typedef openvrml::sfvec3f_emitter
 *
 * @brief @c sfvec3f event emitter.
 */

/**
 * @typedef openvrml::mfbool_emitter
 *
 * @brief @c mfbool event emitter.
 */

/**
 * @typedef openvrml::mfcolor_emitter
 *
 * @brief @c mfcolor event emitter.
 */

/**
 * @typedef openvrml::mfcolorrgba_emitter
 *
 * @brief @c mfcolorrgba event emitter.
 */

/**
 * @typedef openvrml::mfdouble_emitter
 *
 * @brief @c mfdouble event emitter.
 */

/**
 * @typedef openvrml::mfimage_emitter
 *
 * @brief @c mfimage event emitter.
 */

/**
 * @typedef openvrml::mffloat_emitter
 *
 * @brief @c mffloat event emitter.
 */

/**
 * @typedef openvrml::mfint32_emitter
 *
 * @brief @c mfint32 event emitter.
 */

/**
 * @typedef openvrml::mfnode_emitter
 *
 * @brief @c mfnode event emitter.
 */

/**
 * @typedef openvrml::mfrotation_emitter
 *
 * @brief @c mfrotation event emitter.
 */

/**
 * @typedef openvrml::mfstring_emitter
 *
 * @brief @c mfstring event emitter.
 */

/**
 * @typedef openvrml::mftime_emitter
 *
 * @brief @c mftime event emitter.
 */

/**
 * @typedef openvrml::mfvec2d_emitter
 *
 * @brief @c mfvec2d event emitter.
 */

/**
 * @typedef openvrml::mfvec2f_emitter
 *
 * @brief @c mfvec2f event emitter.
 */

/**
 * @typedef openvrml::mfvec3d_emitter
 *
 * @brief @c mfvec3d event emitter.
 */

/**
 * @typedef openvrml::mfvec3f_emitter
 *
 * @brief @c mfvec3f event emitter.
 */
