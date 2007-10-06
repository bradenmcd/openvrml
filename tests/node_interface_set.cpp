// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// Copyright 2007  Braden McDaniel
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

# define BOOST_TEST_MAIN
# define BOOST_TEST_MODULE node_interface_set

# include <boost/test/unit_test.hpp>
# include <openvrml/node.h>

//
// Check to see that node_interface_set is working correctly.
// node_interface_set is an instance of std::set, so basically this
// makes sure that node_interface_compare is doing the right thing.
//

using namespace std;
using namespace openvrml;

BOOST_AUTO_TEST_CASE(insert_eventin)
{
    node_interface_set interfaces;
    const node_interface interface(node_interface::eventin_id,
				   field_value::sfbool_id,
				   "set_zzz");
    BOOST_REQUIRE(interfaces.insert(interface).second);
}

BOOST_AUTO_TEST_CASE(insert_exactly_conflicting_eventin)
{
    node_interface_set interfaces;
    const node_interface interface1(node_interface::eventin_id,
				    field_value::sfbool_id,
				    "set_zzz");
    BOOST_CHECK(interfaces.insert(interface1).second);

    const node_interface interface2(node_interface::eventin_id,
				    field_value::sfcolor_id,
				    "set_zzz");
    BOOST_REQUIRE(!interfaces.insert(interface2).second);
}

BOOST_AUTO_TEST_CASE(insert_conflicting_exposedfield)
{
    node_interface_set interfaces;
    const node_interface interface1(node_interface::eventin_id,
				    field_value::sfbool_id,
				    "set_zzz");
    BOOST_CHECK(interfaces.insert(interface1).second);
    const node_interface interface2(node_interface::exposedfield_id,
				    field_value::sfcolor_id,
				    "zzz");
    BOOST_REQUIRE(!interfaces.insert(interface2).second);
}

BOOST_AUTO_TEST_CASE(insert_eventout)
{
    node_interface_set interfaces;
    const node_interface interface1(node_interface::eventin_id,
				    field_value::sfbool_id,
				    "set_zzz");
    BOOST_CHECK(interfaces.insert(interface1).second);
    const node_interface interface2(node_interface::eventout_id,
				    field_value::sfbool_id,
				    "zzz_changed");
    BOOST_REQUIRE(interfaces.insert(interface2).second);
}

BOOST_AUTO_TEST_CASE(insert_exactly_conflicting_eventout)
{
    node_interface_set interfaces;
    const node_interface interface1(node_interface::eventout_id,
				    field_value::sfbool_id,
				    "zzz_changed");
    BOOST_CHECK(interfaces.insert(interface1).second);
    const node_interface interface2(node_interface::eventout_id,
				    field_value::sfcolor_id,
				    "zzz_changed");
    BOOST_REQUIRE(!interfaces.insert(interface2).second);
}

BOOST_AUTO_TEST_CASE(insert_field)
{
    node_interface_set interfaces;
    const node_interface interface1(node_interface::eventin_id,
				    field_value::sfbool_id,
				    "set_zzz");
    BOOST_CHECK(interfaces.insert(interface1).second);
    const node_interface interface2(node_interface::eventout_id,
				    field_value::sfbool_id,
				    "zzz_changed");
    BOOST_CHECK(interfaces.insert(interface2).second);
    const node_interface interface3(node_interface::field_id,
				    field_value::sfint32_id,
				    "zzz");
    BOOST_REQUIRE(interfaces.insert(interface3).second);
}

BOOST_AUTO_TEST_CASE(insert_exposedfield)
{
    node_interface_set interfaces;
    const node_interface interface1(node_interface::eventin_id,
				    field_value::sfbool_id,
				    "set_zzz");
    BOOST_CHECK(interfaces.insert(interface1).second);
    const node_interface interface2(node_interface::eventout_id,
				    field_value::sfbool_id,
				    "zzz_changed");
    BOOST_CHECK(interfaces.insert(interface2).second);
    const node_interface interface3(node_interface::field_id,
				    field_value::sfint32_id,
				    "zzz");
    BOOST_CHECK(interfaces.insert(interface3).second);
    const node_interface interface4(node_interface::exposedfield_id,
				    field_value::sfrotation_id,
				    "yyy");
    BOOST_REQUIRE(interfaces.insert(interface4).second);
}

BOOST_AUTO_TEST_CASE(find_exposedfield_matching_eventin)
{
    node_interface_set interfaces;
    const node_interface interface(node_interface::exposedfield_id,
				   field_value::sfrotation_id,
				   "yyy");
    BOOST_CHECK(interfaces.insert(interface).second);
    const node_interface_set::const_iterator pos =
	interfaces.find(interface);
    BOOST_CHECK(pos != interfaces.end());
    BOOST_REQUIRE(interface == *pos);
}
