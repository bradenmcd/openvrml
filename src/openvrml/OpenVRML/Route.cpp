//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// 

# include "Route.h"
# include <string.h>

Route::Route(const char * fromEventOut,
             VrmlNode * toNode, const char * toEventIn ):
        d_prev(0), d_next(0) {
    d_fromEventOut = new char[strlen(fromEventOut)+1];
    strcpy(d_fromEventOut, fromEventOut);
    d_toNode = toNode;
    d_toEventIn = new char[strlen(toEventIn)+1];
    strcpy(d_toEventIn, toEventIn);
}

Route::Route(const Route & r) {
    d_fromEventOut = new char[strlen(r.d_fromEventOut)+1];
    strcpy(d_fromEventOut, r.d_fromEventOut);
    d_toNode = r.d_toNode;
    d_toEventIn = new char[strlen(r.d_toEventIn)+1];
    strcpy(d_toEventIn, r.d_toEventIn);
}

Route::~Route() {
    delete [] d_fromEventOut;
    delete [] d_toEventIn;
}
