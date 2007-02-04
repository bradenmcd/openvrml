// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// Copyright 2004, 2005  Braden McDaniel
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

# include <cstdlib>
# include <iostream>
# include <sstream>
# include "test_browser.h"

int main()
{
    using namespace std;
    using namespace openvrml;

    try {
        test_browser b;

        const char vrmlstring[] =
            "Anchor {"
            "  children []"
            "  description \"\""
            "  parameter []"
            "  url []"
            "  bboxCenter 0 0 0"
            "  bboxSize -1 -1 -1"
            "}";
        stringstream vrmlstream(vrmlstring);

        vector<boost::intrusive_ptr<node> > nodes =
            b.create_vrml_from_stream(vrmlstream);
        if (nodes.size() != 1) {
            return EXIT_FAILURE;
        }
        if (!nodes[0]) {
            return EXIT_FAILURE;
        }
        if (nodes[0]->type().id() != "Anchor") {
            return EXIT_FAILURE;
        }
    } catch (...) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
