// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// Copyright 2005  Braden McDaniel
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

# include <fstream>
# include <sstream>
# include <boost/thread.hpp>
# include <boost/test/unit_test.hpp>
# include "test_browser.h"

using namespace std;
using namespace openvrml;

void create_vrml_from_stream()
{
    test_browser b(vrml97_profile_id);

    const char vrmlstring[] = "Group {}";
    stringstream vrmlstream(vrmlstring);

    vector<node_ptr> nodes = b.create_vrml_from_stream(vrmlstream);

    BOOST_REQUIRE(nodes.size() == 1);
    BOOST_REQUIRE(nodes[0] != node_ptr(0));
    BOOST_CHECK_EQUAL(nodes[0]->type().id(), "Group");
}

void create_vrml_from_url()
{
    {
	ofstream file("test.wrl");
        file << "#VRML V2.0 utf8" << endl
	     << "Shape {}" << endl;
    }
    test_browser b(vrml97_profile_id);
    const char vrmlstring[] = "Group {}";
    stringstream vrmlstream(vrmlstring);
    vector<node_ptr> nodes = b.create_vrml_from_stream(vrmlstream);
    vector<string> url(1, "test.wrl");
    b.create_vrml_from_url(url, nodes[0], "set_children");

    boost::xtime t;
    boost::xtime_get(&t, boost::TIME_UTC);
    t.sec += 1;
    boost::thread::sleep(t);

    grouping_node * group = node_cast<grouping_node *>(nodes[0].get());
    BOOST_REQUIRE(group);
    const vector<node_ptr> & children = group->children();
    BOOST_REQUIRE(children.size() == 1);
    BOOST_CHECK_EQUAL(children[0]->type().id(), "Shape");

    int remove_result = remove("test.wrl");
    BOOST_CHECK(remove_result == 0);
}

boost::unit_test::test_suite * init_unit_test_suite(int, char * [])
{
    using boost::unit_test::test_suite;
    test_suite * const suite = BOOST_TEST_SUITE("browser");
    suite->add(BOOST_TEST_CASE(&create_vrml_from_stream));
    suite->add(BOOST_TEST_CASE(&create_vrml_from_url));
    return suite;
}
