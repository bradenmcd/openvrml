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

Route::Route(const std::string & fromEventOut,
             const VrmlNodePtr & toNode, const std::string & toEventIn):
        prevRoute(0), nextRoute(0), fromEventOut(fromEventOut), toNode(toNode),
        toEventIn(toEventIn) {}

Route::Route(const Route & route): prevRoute(0), nextRoute(0),
        fromEventOut(route.fromEventOut), toNode(route.toNode),
        toEventIn(route.toEventIn) {}

Route * Route::getPrev() const {
    return this->prevRoute;
}

void Route::setPrev(Route * route) {
    this->prevRoute = route;
}

Route * Route::getNext() const {
    return this->nextRoute;
}

void Route::setNext(Route * route) {
    this->nextRoute = route;
}
