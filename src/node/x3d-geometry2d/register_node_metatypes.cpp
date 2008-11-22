// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2006, 2007, 2008  Braden McDaniel
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 3 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, see <http://www.gnu.org/licenses/>.
//

# include "arc2d.h"
# include "arc_close2d.h"
# include "circle2d.h"
# include "disk2d.h"
# include "polyline2d.h"
# include "polypoint2d.h"
# include "rectangle2d.h"
# include "triangle_set2d.h"
# include <openvrml/browser.h>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

extern "C"
# ifdef _WIN32
__declspec(dllexport)
# else
OPENVRML_API
# endif
void
openvrml_register_node_metatypes(openvrml::node_metatype_registry & registry)
{
    using boost::shared_ptr;
    using openvrml::node_metatype;
    using namespace openvrml_node_x3d_geometry2d;

    openvrml::browser & b = registry.browser();

    registry.register_node_metatype(
        arc2d_metatype::id,
        shared_ptr<node_metatype>(new arc2d_metatype(b)));
    registry.register_node_metatype(
        arc_close2d_metatype::id,
        shared_ptr<node_metatype>(new arc_close2d_metatype(b)));
    registry.register_node_metatype(
        circle2d_metatype::id,
        shared_ptr<node_metatype>(new circle2d_metatype(b)));
    registry.register_node_metatype(
        disk2d_metatype::id,
        shared_ptr<node_metatype>(new disk2d_metatype(b)));
    registry.register_node_metatype(
        polyline2d_metatype::id,
        shared_ptr<node_metatype>(new polyline2d_metatype(b)));
    registry.register_node_metatype(
        polypoint2d_metatype::id,
        shared_ptr<node_metatype>(new polypoint2d_metatype(b)));
    registry.register_node_metatype(
        rectangle2d_metatype::id,
        shared_ptr<node_metatype>(new rectangle2d_metatype(b)));
    registry.register_node_metatype(
        triangle_set2d_metatype::id,
        shared_ptr<node_metatype>(new triangle_set2d_metatype(b)));
}
