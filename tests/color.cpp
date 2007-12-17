// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// Copyright 2006, 2007  Braden McDaniel
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
# define BOOST_TEST_MODULE color

# include <sstream>
# include <boost/test/unit_test.hpp>
# include <openvrml/basetypes.h>

using namespace std;
using namespace openvrml;

BOOST_AUTO_TEST_CASE(stream_extraction_without_commas)
{
    color c1 = make_color(1.0, 1.0, 1.0), c2 = make_color(0.5, 0.5, 0.5), c3, c4;
    string color_str = "1.0 1.0 1.0 0.5 0.5 0.5";
    istringstream in(color_str);
    in >> c3 >> c4;
    BOOST_REQUIRE(in.good());
    BOOST_REQUIRE_EQUAL(c1, c3);
    BOOST_REQUIRE_EQUAL(c2, c4);
}

BOOST_AUTO_TEST_CASE(stream_extraction_with_commas)
{
    color c1 = make_color(1.0, 1.0, 1.0), c2 = make_color(0.5, 0.5, 0.5), c3, c4;
    string color_str = "1.0, 1.0, 1.0, 0.5, 0.5, 0.5";
    istringstream in(color_str);
    in >> c3 >> c4;
    BOOST_REQUIRE(in.good());
    BOOST_REQUIRE_EQUAL(c1, c3);
    BOOST_REQUIRE_EQUAL(c2, c4);
}

BOOST_AUTO_TEST_CASE(stream_extraction_fail_on_invalid_intensity)
{
    color c;
    string color_str = "2.0 0.5 0.5";
    istringstream in(color_str);
    in >> c;
    BOOST_REQUIRE(in.fail());
}
