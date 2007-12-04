// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
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

# include <iostream>
# include <fstream>
# include "test_resource_fetcher.h"

using namespace std;
using namespace openvrml;

int main(int argc, char * argv[])
{
    if (argc < 2) {
        cerr << argv[0] << ": missing file name argument" << endl;
        return EXIT_FAILURE;
    }
    try {
        ifstream in;
        in.open(argv[1]);
        if (!in.is_open()) {
            cerr << argv[0] << ": could not open file \"" << argv[1]
                 << endl;
            return EXIT_FAILURE;
        }

        test_resource_fetcher fetcher;
        browser b(fetcher, cout, cerr);
        b.create_vrml_from_stream(in, vrml_media_type);
    } catch (invalid_vrml & ex) {
        cerr << ex.url << ':' << ex.line << ':' << ex.column << ": error: "
             << ex.what() << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
