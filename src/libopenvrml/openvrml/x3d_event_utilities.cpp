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

# include <openvrml/browser.h>
# include "x3d_event_utilities.h"
# include <x3d-event-utilities/boolean_filter.h>
# include <x3d-event-utilities/boolean_sequencer.h>
# include <x3d-event-utilities/boolean_toggle.h>
# include <x3d-event-utilities/boolean_trigger.h>
# include <x3d-event-utilities/integer_sequencer.h>
# include <x3d-event-utilities/integer_trigger.h>
# include <x3d-event-utilities/time_trigger.h>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

void register_event_utilities_node_metatypes(openvrml::browser & b)
{
    using boost::shared_ptr;
    using openvrml::node_metatype;
    using namespace openvrml_node_x3d_event_utilities;
    b.add_node_metatype(
        boolean_filter_metatype::id,
        shared_ptr<node_metatype>(new boolean_filter_metatype(b)));
    b.add_node_metatype(
        boolean_sequencer_metatype::id,
        shared_ptr<node_metatype>(new boolean_sequencer_metatype(b)));
    b.add_node_metatype(
        boolean_toggle_metatype::id,
        shared_ptr<node_metatype>(new boolean_toggle_metatype(b)));
    b.add_node_metatype(
        boolean_trigger_metatype::id,
        shared_ptr<node_metatype>(new boolean_trigger_metatype(b)));
    b.add_node_metatype(
        integer_sequencer_metatype::id,
        shared_ptr<node_metatype>(new integer_sequencer_metatype(b)));
    b.add_node_metatype(
        integer_trigger_metatype::id,
        shared_ptr<node_metatype>(new integer_trigger_metatype(b)));
    b.add_node_metatype(
        time_trigger_metatype::id,
        shared_ptr<node_metatype>(new time_trigger_metatype(b)));
}
