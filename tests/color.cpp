// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// Copyright 2006  Braden McDaniel
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

# include <sstream>
# include <boost/test/unit_test.hpp>
# include <openvrml/basetypes.h>

using namespace std;
using namespace openvrml;

void stream_extraction_without_commas()
{
    color c1(1.0, 1.0, 1.0), c2(0.5, 0.5, 0.5), c3, c4;
    string color_str = "1.0 1.0 1.0 0.5 0.5 0.5";
    istringstream in(color_str);
    in >> c3 >> c4;
    BOOST_REQUIRE(in.good());
    BOOST_REQUIRE_EQUAL(c1, c3);
    BOOST_REQUIRE_EQUAL(c2, c4);
}

void stream_extraction_with_commas()
{
    color c1(1.0, 1.0, 1.0), c2(0.5, 0.5, 0.5), c3, c4;
    string color_str = "1.0, 1.0, 1.0, 0.5, 0.5, 0.5";
    istringstream in(color_str);
    in >> c3 >> c4;
    BOOST_REQUIRE(in.good());
    BOOST_REQUIRE_EQUAL(c1, c3);
    BOOST_REQUIRE_EQUAL(c2, c4);
}

void stream_extraction_fail_on_invalid_intensity()
{
    color c;
    string color_str = "2.0 0.5 0.5";
    istringstream in(color_str);
    in >> c;
    BOOST_REQUIRE(in.fail());
}

boost::unit_test::test_suite * init_unit_test_suite(int, char * [])
{
    using boost::unit_test::test_suite;
    test_suite * const suite = BOOST_TEST_SUITE("color");
    suite->add(BOOST_TEST_CASE(&stream_extraction_without_commas));
    suite->add(BOOST_TEST_CASE(&stream_extraction_with_commas));
    suite->add(BOOST_TEST_CASE(&stream_extraction_fail_on_invalid_intensity));
    return suite;
}
