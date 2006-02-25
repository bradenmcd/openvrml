// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
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
 * @brief The <code>field_value::type_id</code> for the type accepted by the
 *        event listener.
 *
 * @return The <code>field_value::type_id</code> for the type accepted by the
 *         event listener.
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
 * @brief The <code>node</code> to which the <code>node_event_listener</code>
 *        belongs.
 */

/**
 * @brief Construct.
 *
 * @param[in] n the <code>node</code> to which the <code>node_event_listener</code>
 *          belongs.
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
 * @brief The <code>node</code> to which the <code>node_event_listener</code>
 *        belongs.
 *
 * @return the <code>node</code> to which the <code>node_event_listener</code>
 *         belongs.
 */
openvrml::node & openvrml::node_event_listener::node() const OPENVRML_NOTHROW
{
    return *this->node_;
}

/**
 * @brief The associated eventIn identifier.
 *
 * This function delegates to <code>node_event_listener::do_eventin_id</code>.
 *
 * @return the associated eventIn identifier.
 */
const std::string openvrml::node_event_listener::eventin_id() const
    OPENVRML_NOTHROW
{
    return this->do_eventin_id();
}

/**
 * @fn const std::string openvrml::node_event_listener::do_eventin_id() const
 *
 * @brief The associated eventIn identifier.
 *
 * Concrete subclasses must implement this function.
 *
 * @return the associated eventIn identifier.
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
 *
 * @param[in] node  the node to which the event_listener belongs.
 */

/**
 * @fn openvrml::field_value_listener::~field_value_listener()
 *
 * @brief Destroy.
 */

/**
 * @fn openvrml::field_value::type_id openvrml::field_value_listener::type() const
 *
 * @brief <code>FieldValue::field_value_type_id</code>.
 *
 * @return <code>FieldValue::field_value_type_id</code>.
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
 * @brief Called by
 *      <code>field_value_listener&lt;FieldValue&gt;::do_process_event</code>.
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
 * @brief sfbool event listener.
 */

/**
 * @typedef openvrml::sfcolor_listener
 *
 * @brief sfcolor event listener.
 */

/**
 * @typedef openvrml::sfcolorrgba_listener
 *
 * @brief sfcolorrgba event listener.
 */

/**
 * @typedef openvrml::sfdouble_listener
 *
 * @brief sfdouble event listener.
 */

/**
 * @typedef openvrml::sffloat_listener
 *
 * @brief sffloat event listener.
 */

/**
 * @typedef openvrml::sfimage_listener
 *
 * @brief sfimage event listener.
 */

/**
 * @typedef openvrml::sfint32_listener
 *
 * @brief sfint32 event listener.
 */

/**
 * @typedef openvrml::sfnode_listener
 *
 * @brief sfnode event listener.
 */

/**
 * @typedef openvrml::sfrotation_listener
 *
 * @brief sfrotation event listener.
 */

/**
 * @typedef openvrml::sfstring_listener
 *
 * @brief sfstring event listener.
 */

/**
 * @typedef openvrml::sftime_listener
 *
 * @brief sftime event listener.
 */

/**
 * @typedef openvrml::sfvec2d_listener
 *
 * @brief sfvec2d event listener.
 */

/**
 * @typedef openvrml::sfvec2f_listener
 *
 * @brief sfvec2f event listener.
 */

/**
 * @typedef openvrml::sfvec3d_listener
 *
 * @brief sfvec3d event listener.
 */

/**
 * @typedef openvrml::sfvec3f_listener
 *
 * @brief sfvec3f event listener.
 */

/**
 * @typedef openvrml::mfcolor_listener
 *
 * @brief mfcolor event listener.
 */

/**
 * @typedef openvrml::mfbool_listener
 *
 * @brief mfbool event listener.
 */

/**
 * @typedef openvrml::mfcolorrgba_listener
 *
 * @brief mfcolorrgba event listener.
 */

/**
 * @typedef openvrml::mfdouble_listener
 *
 * @brief mfdouble event listener.
 */

/**
 * @typedef openvrml::mffloat_listener
 *
 * @brief mffloat event listener.
 */

/**
 * @typedef openvrml::mfimage_listener
 *
 * @brief mfimage event listener.
 */

/**
 * @typedef openvrml::mfint32_listener
 *
 * @brief mfint32 event listener.
 */

/**
 * @typedef openvrml::mfnode_listener
 *
 * @brief mfnode event listener.
 */

/**
 * @typedef openvrml::mfrotation_listener
 *
 * @brief mfrotation event listener.
 */

/**
 * @typedef openvrml::mfstring_listener
 *
 * @brief mfstring event listener.
 */

/**
 * @typedef openvrml::mftime_listener
 *
 * @brief mftime event listener.
 */

/**
 * @typedef openvrml::mfvec2d_listener
 *
 * @brief mfvec2d event listener.
 */

/**
 * @typedef openvrml::mfvec2f_listener
 *
 * @brief mfvec2f event listener.
 */

/**
 * @typedef openvrml::mfvec3d_listener
 *
 * @brief mfvec3d event listener.
 */

/**
 * @typedef openvrml::mfvec3f_listener
 *
 * @brief mfvec3f event listener.
 */


/**
 * @class openvrml::node_field_value_listener
 *
 * @brief Base class for <code>field_value_listener</code>s that are members
 *        of nodes.
 */

/**
 * @fn openvrml::node_field_value_listener::node_field_value_listener(openvrml::node & n)
 *
 * @brief Construct.
 *
 * @param[in] n the node that the listener is a member of.
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
 * @brief The implementation of <code>node</code> calls
 *        <code>event_emitter::emit_event</code>.
 *
 * The only things that should be emitting events are <code>node</code>s.
 * Subclasses of <code>node</code> should call <code>node::emit_event</code> to
 * emit an event.
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
 * @brief A reference to the field_value for the event_emitter.
 */

/**
 * @typedef openvrml::event_emitter::listener_set
 *
 * @brief Set of event_listeners.
 */

/**
 * @var openvrml::event_emitter::listener_set openvrml::event_emitter::listeners_
 *
 * @brief The listeners registered for this emitter.
 *
 * When emit_event is called, each of the registered listeners will be sent an
 * event.
 */

/**
 * @var double openvrml::event_emitter::last_time_
 *
 * @brief The timestamp of the last event emitted.
 */

/**
 * @brief Construct.
 *
 * @param[in] value <code>field_value</code> associated with this emitter.
 */
openvrml::event_emitter::event_emitter(const field_value & value)
    OPENVRML_NOTHROW:
    value_(value)
{}

/**
 * @brief Destroy.
 */
openvrml::event_emitter::~event_emitter() OPENVRML_NOTHROW
{}

/**
 * @brief Get the mutex for the <code>event_emitter</code>.
 *
 * @return a reference to the <code>event_emitter</code>'s mutex.
 */
boost::recursive_mutex & openvrml::event_emitter::mutex() const
    OPENVRML_NOTHROW
{
    return this->mutex_;
}

/**
 * @brief A reference to the <code>field_value</code> for the
 *        <code>event_emitter</code>.
 *
 * @return a reference to the <code>field_value</code> for the
 *         <code>event_emitter</code>.
 */
const openvrml::field_value & openvrml::event_emitter::value() const
    OPENVRML_NOTHROW
{
    return this->value_;
}

/**
 * @brief The associated eventOut identifier.
 *
 * This function delegates to <code>event_emitter::do_eventout_id</code>.
 *
 * @return the associated eventOut identifier.
 */
const std::string openvrml::event_emitter::eventout_id() const
    OPENVRML_NOTHROW
{
    return this->do_eventout_id();
}

/**
 * @fn const std::string openvrml::event_emitter::do_eventout_id() const
 *
 * @brief The associated eventOut identifier.
 *
 * Concrete subclasses must implement this function.
 *
 * @return the associated eventOut identifier.
 */

/**
 * @brief Registered listeners.
 *
 * @return the set of registered event_listeners.
 */
const openvrml::event_emitter::listener_set &
openvrml::event_emitter::listeners() const OPENVRML_NOTHROW
{
    return this->listeners_;
}

/**
 * @brief Registered listeners.
 *
 * @return the set of registered event_listeners.
 */
openvrml::event_emitter::listener_set & openvrml::event_emitter::listeners()
    OPENVRML_NOTHROW
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
 * This function is called by node::emit_event.
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
 * @brief sfbool event emitter.
 */

/**
 * @typedef openvrml::sfcolor_emitter
 *
 * @brief sfcolor event emitter.
 */

/**
 * @typedef openvrml::sfcolorrgba_emitter
 *
 * @brief sfcolorrgba event emitter.
 */

/**
 * @typedef openvrml::sfdouble_emitter
 *
 * @brief sfdouble event emitter.
 */

/**
 * @typedef openvrml::sffloat_emitter
 *
 * @brief sffloat event emitter.
 */

/**
 * @typedef openvrml::sfimage_emitter
 *
 * @brief sfimage event emitter.
 */

/**
 * @typedef openvrml::sfint32_emitter
 *
 * @brief sfint32 event emitter.
 */

/**
 * @typedef openvrml::sfnode_emitter
 *
 * @brief sfnode event emitter.
 */

/**
 * @typedef openvrml::sfrotation_emitter
 *
 * @brief sfrotation event emitter.
 */

/**
 * @typedef openvrml::sfstring_emitter
 *
 * @brief sfstring event emitter.
 */

/**
 * @typedef openvrml::sftime_emitter
 *
 * @brief sftime event emitter.
 */

/**
 * @typedef openvrml::sfvec2d_emitter
 *
 * @brief sfvec2d event emitter.
 */

/**
 * @typedef openvrml::sfvec2f_emitter
 *
 * @brief sfvec2f event emitter.
 */

/**
 * @typedef openvrml::sfvec3d_emitter
 *
 * @brief sfvec3d event emitter.
 */

/**
 * @typedef openvrml::sfvec3f_emitter
 *
 * @brief sfvec3f event emitter.
 */

/**
 * @typedef openvrml::mfbool_emitter
 *
 * @brief mfbool event emitter.
 */

/**
 * @typedef openvrml::mfcolor_emitter
 *
 * @brief mfcolor event emitter.
 */

/**
 * @typedef openvrml::mfcolorrgba_emitter
 *
 * @brief mfcolorrgba event emitter.
 */

/**
 * @typedef openvrml::mfdouble_emitter
 *
 * @brief mfdouble event emitter.
 */

/**
 * @typedef openvrml::mfimage_emitter
 *
 * @brief mfimage event emitter.
 */

/**
 * @typedef openvrml::mffloat_emitter
 *
 * @brief mffloat event emitter.
 */

/**
 * @typedef openvrml::mfint32_emitter
 *
 * @brief mfint32 event emitter.
 */

/**
 * @typedef openvrml::mfnode_emitter
 *
 * @brief mfnode event emitter.
 */

/**
 * @typedef openvrml::mfrotation_emitter
 *
 * @brief mfrotation event emitter.
 */

/**
 * @typedef openvrml::mfstring_emitter
 *
 * @brief mfstring event emitter.
 */

/**
 * @typedef openvrml::mftime_emitter
 *
 * @brief mftime event emitter.
 */

/**
 * @typedef openvrml::mfvec2d_emitter
 *
 * @brief mfvec2d event emitter.
 */

/**
 * @typedef openvrml::mfvec2f_emitter
 *
 * @brief mfvec2f event emitter.
 */

/**
 * @typedef openvrml::mfvec3d_emitter
 *
 * @brief mfvec3d event emitter.
 */

/**
 * @typedef openvrml::mfvec3f_emitter
 *
 * @brief mfvec3f event emitter.
 */
