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

# include "exposedfield.h"

/**
 * @class openvrml::exposedfield
 *
 * @brief Class template to simplify implementation of exposedFields.
 *
 * <code>exposedfield</code> conveniently implements an
 * <code>event_listener</code> and an <code>event_emitter</code>. Trivial
 * exposedFields can be implemented simply by instantiating this template with
 * a FieldValue. For the purposes of OpenVRML, a <em>trivial exposedField</em>
 * is one that has <strong>no</strong> side-effects. That is, it simply
 * receives an event, updates an internal value, and fires an eventOut.
 * Nontrivial exposedFields (i.e., those with side-effects) can generally be
 * implemented by inheriting an instance of this class template and overriding
 * <code>exposedfield<FieldValue>::event_side_effect</code>.
 */

/**
 * @fn openvrml::exposedfield::exposedfield(openvrml::node & node, const typename FieldValue::value_type & value)
 *
 * @brief Construct.
 *
 * @param[in] node  a reference to the node to which the exposedField belongs.
 * @param[in] value default value.
 */

/**
 * @fn openvrml::exposedfield::exposedfield(const exposedfield<FieldValue> & obj)
 *
 * @brief Construct a copy.
 *
 * @param[in] obj   the instance to copy.
 */

/**
 * @fn openvrml::exposedfield::~exposedfield()
 *
 * @brief Destroy.
 */

/**
 * @fn void openvrml::exposedfield::do_process_event(const FieldValue & value, double timestamp)
 *
 * @brief Process an event.
 *
 * This function performs the following steps:
 *
 * -# set the exposedField value.
 * -# call <code>exposedfield<FieldValue>::event_side_effect</code>.
 * -# set the modified flag.
 * -# emit the event.
 *
 * @warning This function should not be overridden by subclasses.  Subclasses
 *          should override
 *          <code>exposedfield<FieldValue>::event_side_effect</code> instead.
 *
 * @param[in] value     new value.
 * @param[in] timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @fn void openvrml::exposedfield::event_side_effect(const FieldValue & value, double timestamp)
 *
 * @brief Called by <code>exposedfield<FieldValue>::do_process_event</code>.
 *
 * Subclasses should override this method to implement event handling
 * functionality specific to a particular exposedField. The default
 * implementation of this function does nothing.
 *
 * @param[in] value     new value.
 * @param[in] timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
