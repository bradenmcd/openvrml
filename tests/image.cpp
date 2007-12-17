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
# define BOOST_TEST_MODULE image

# include <sstream>
# include <boost/array.hpp>
# include <boost/test/unit_test.hpp>
# include <openvrml/basetypes.h>

using namespace std;
using namespace openvrml;

BOOST_AUTO_TEST_CASE(stream_insertion)
{
    const image img(2, 2, 1);
    const string image_str = "2 2 1 0x0 0x0 0x0 0x0";
    ostringstream out;
    out << img;
    BOOST_REQUIRE(out.good());
    BOOST_REQUIRE_EQUAL(out.str(), image_str);
}

BOOST_AUTO_TEST_CASE(stream_extraction)
{
    const boost::array<unsigned char, 4> pixels = { 0xFF, 0x00, 0xFF, 0x00 };
    const image img1(2, 2, 1, pixels.begin(), pixels.end());
    image img2;
    const string image_str = "2 2 1 0xFF 0x00 0xFF 0x00";
    istringstream in(image_str);
    in >> img2;
    BOOST_REQUIRE(in.good());
    BOOST_REQUIRE_EQUAL(img1, img2);
}

BOOST_AUTO_TEST_CASE(stream_extraction_insufficient_pixel_values)
{
    const string image_str = "2 2 1 0xFF 0x00 0xFF";
    istringstream in(image_str);
    image img;
    in >> img;
    BOOST_REQUIRE(in.fail());
}
