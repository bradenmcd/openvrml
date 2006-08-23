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
# include <boost/array.hpp>
# include <boost/test/unit_test.hpp>
# include <openvrml/basetypes.h>

using namespace std;
using namespace openvrml;

void stream_insertion()
{
    const image img(2, 2, 1);
    const string image_str = "2 2 1 0x0 0x0 0x0 0x0";
    ostringstream out;
    out << img;
    BOOST_REQUIRE(out.good());
    BOOST_REQUIRE_EQUAL(out.str(), image_str);
}

void stream_extraction()
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

void stream_extraction_insufficient_pixel_values()
{
    const string image_str = "2 2 1 0xFF 0x00 0xFF";
    istringstream in(image_str);
    image img;
    in >> img;
    BOOST_REQUIRE(in.fail());
}

boost::unit_test::test_suite * init_unit_test_suite(int, char * [])
{
    using boost::unit_test::test_suite;
    test_suite * const suite = BOOST_TEST_SUITE("image");
    suite->add(BOOST_TEST_CASE(&stream_insertion));
    suite->add(BOOST_TEST_CASE(&stream_extraction));
    suite->add(BOOST_TEST_CASE(&stream_extraction_insufficient_pixel_values));
    return suite;
}
