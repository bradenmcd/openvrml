// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2008  Braden McDaniel
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

# include "bad_url.h"

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

/**
 * @file openvrml/bad_url.h
 *
 * @brief Exceptions indicating various types of resource retrieval failure.
 */

/**
 * @class openvrml::bad_url openvrml/bad_url.h
 *
 * @brief Thrown when there is a problem resolving a URI.
 */

/**
 * @brief Construct.
 *
 * @param[in] message   Informative text.
 */
openvrml::bad_url::bad_url(const std::string & message):
    std::runtime_error(message)
{}

/**
 * @brief Destroy.
 */
openvrml::bad_url::~bad_url() throw ()
{}


/**
 * @class openvrml::invalid_url openvrml/bad_url.h
 *
 * @brief Thrown when parsing a URI fails.
 */

/**
 * @brief Construct.
 */
openvrml::invalid_url::invalid_url():
    bad_url("invalid URI")
{}

/**
 * @brief Destroy.
 */
openvrml::invalid_url::~invalid_url() throw ()
{}


/**
 * @class openvrml::bad_media_type openvrml/bad_url.h
 *
 * @brief Thrown when an unexpected media type is received.
 */

/**
 * @brief Construct.
 *
 * @param[in] received_type the media type that was unexpectedly received.
 */
openvrml::bad_media_type::bad_media_type(const std::string & received_type)
    OPENVRML_NOTHROW:
    bad_url("unexpected media type \"" + received_type + "\"")
{}

/**
 * @brief Destroy.
 */
openvrml::bad_media_type::~bad_media_type() throw ()
{}


/**
 * @class openvrml::unreachable_url openvrml/browser.h
 *
 * @brief Thrown when a URI cannot be reached.
 */

/**
 * @brief Construct.
 */
openvrml::unreachable_url::unreachable_url():
    bad_url("unreachable URI")
{}

/**
 * @brief Destroy.
 */
openvrml::unreachable_url::~unreachable_url() throw ()
{}


/**
 * @class openvrml::no_alternative_url openvrml/bad_url.h
 *
 * @brief Exception thrown when no URI in an alternative URI list can be
 *        resolved.
 */

/**
 * @brief Construct.
 */
openvrml::no_alternative_url::no_alternative_url():
    bad_url("no alternative URI could be resolved")
{}

/**
 * @brief Destroy.
 */
openvrml::no_alternative_url::~no_alternative_url() throw ()
{}
