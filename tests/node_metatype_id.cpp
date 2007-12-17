// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// Copyright 2007  Braden McDaniel
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along
// with this library; if not, see <http://www.gnu.org/licenses/>.
//

# define BOOST_TEST_MAIN
# define BOOST_TEST_MODULE node_metatype_id

# include <boost/test/unit_test.hpp>
# include <openvrml/browser.h>

using namespace std;
using namespace openvrml;

BOOST_AUTO_TEST_CASE(construct_from_urn)
{
    node_metatype_id id("urn:foo:bar");
}

BOOST_AUTO_TEST_CASE(construct_from_url)
{
    node_metatype_id id("http://example.com");
}

BOOST_AUTO_TEST_CASE(construct_from_url_with_fragment_id)
{
    node_metatype_id id("http://example.com#Foo");
}

BOOST_AUTO_TEST_CASE(construct_nested_proto_id)
{
    node_metatype_id id("http://example.com#Foo#Bar");
}

BOOST_AUTO_TEST_CASE(construct_from_relative_url)
{
    BOOST_CHECK_THROW(node_metatype_id id("../foo/bar"),
                      std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(construct_from_arbitrary_string)
{
    BOOST_CHECK_THROW(node_metatype_id id("string"), std::invalid_argument);
}
