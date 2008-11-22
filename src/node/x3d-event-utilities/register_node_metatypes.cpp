// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2006, 2007, 2008  Braden McDaniel
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

# include "boolean_filter.h"
# include "boolean_sequencer.h"
# include "boolean_toggle.h"
# include "boolean_trigger.h"
# include "integer_sequencer.h"
# include "integer_trigger.h"
# include "time_trigger.h"
# include <openvrml/browser.h>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

extern "C"
# ifdef _WIN32
__declspec(dllexport)
# else
OPENVRML_API
# endif
void
openvrml_register_node_metatypes(openvrml::node_metatype_registry & registry)
{
    using boost::shared_ptr;
    using openvrml::node_metatype;
    using namespace openvrml_node_x3d_event_utilities;

    openvrml::browser & b = registry.browser();

    registry.register_node_metatype(
        boolean_filter_metatype::id,
        shared_ptr<node_metatype>(new boolean_filter_metatype(b)));
    registry.register_node_metatype(
        boolean_sequencer_metatype::id,
        shared_ptr<node_metatype>(new boolean_sequencer_metatype(b)));
    registry.register_node_metatype(
        boolean_toggle_metatype::id,
        shared_ptr<node_metatype>(new boolean_toggle_metatype(b)));
    registry.register_node_metatype(
        boolean_trigger_metatype::id,
        shared_ptr<node_metatype>(new boolean_trigger_metatype(b)));
    registry.register_node_metatype(
        integer_sequencer_metatype::id,
        shared_ptr<node_metatype>(new integer_sequencer_metatype(b)));
    registry.register_node_metatype(
        integer_trigger_metatype::id,
        shared_ptr<node_metatype>(new integer_trigger_metatype(b)));
    registry.register_node_metatype(
        time_trigger_metatype::id,
        shared_ptr<node_metatype>(new time_trigger_metatype(b)));
}
