// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
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

# include <iostream>
# include <fstream>
# include <openvrml/vrml97_grammar.h>

using namespace std;
using namespace boost::spirit;
using namespace openvrml;

int main(int argc, char * argv[])
{
    ifstream infile;
    if (argc > 1) {
        infile.open(argv[1]);
        if (!infile.is_open()) {
            cerr << argv[0] << ": could not open file \"" << argv[1] << endl;
            return EXIT_FAILURE;
        }
    }

    istream & in(infile.is_open() ? infile : cin);
    const string filename(infile.is_open() ? argv[1] : "<stdin>");

    typedef multi_pass<istreambuf_iterator<char> > multi_pass_iterator_t;
    typedef istream::char_type char_t;

    multi_pass_iterator_t
        in_begin(make_multi_pass(istreambuf_iterator<char_t>(in))),
        in_end(make_multi_pass(istreambuf_iterator<char_t>()));

    typedef position_iterator<multi_pass_iterator_t> iterator_t;

    iterator_t first(in_begin, in_end, filename), last;

    vrml97_skip_grammar skip_g;
    vrml97_grammar<> g;

    BOOST_SPIRIT_DEBUG_NODE(skip_g);
    BOOST_SPIRIT_DEBUG_NODE(g);

    if (!parse(first, last, g, skip_g).full) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
