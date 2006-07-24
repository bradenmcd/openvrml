// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
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
 * @brief Class template to simplify implementation of @c exposedField%s.
 *
 * @c exposedfield conveniently implements an @c event_listener and an @c
 * event_emitter.  Trivial @c exposedField%s can be implemented simply by
 * instantiating this template with a @link openvrml::FieldValueConcept
 * FieldValue @endlink.  For the purposes of OpenVRML, a <em>trivial @c
 * exposedField</em> is one that has <strong>no</strong> side-effects.  That
 * is, it simply receives an event, updates an internal value, and fires an @c
 * eventOut.  Nontrivial @c exposedField%s (i.e., those with side-effects) can
 * generally be implemented by inheriting an instance of this class template
 * and overriding @c exposedfield<FieldValue>::event_side_effect.
 */

/**
 * @fn openvrml::exposedfield::exposedfield(openvrml::node & node, const typename FieldValue::value_type & value)
 *
 * @brief Construct.
 *
 * @param[in] node  a reference to the @c node to which the @c exposedField
 *                  belongs.
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
 * -# set the @c exposedField value.
 * -# call @c exposedfield<FieldValue>::event_side_effect.
 * -# set the modified flag.
 * -# emit the event.
 *
 * @warning This function should not be overridden by subclasses.  Subclasses
 *          should override
 *          @c exposedfield<FieldValue>::event_side_effect instead.
 *
 * @param[in] value     new value.
 * @param[in] timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @fn void openvrml::exposedfield::event_side_effect(const FieldValue & value, double timestamp)
 *
 * @brief Called by @c exposedfield<FieldValue>::do_process_event.
 *
 * Subclasses should override this method to implement event handling
 * functionality specific to a particular @c exposedField. The default
 * implementation of this function does nothing.
 *
 * @param[in] value     new value.
 * @param[in] timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
