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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <boost/array.hpp>
# include <private.h>
# include "browser.h"
# include "node_impl_util.h"
# include "x3d_geometry2d.h"
# include <x3d-geometry2d/arc2d.h>
# include <x3d-geometry2d/arc_close2d.h>
# include <x3d-geometry2d/circle2d.h>
# include <x3d-geometry2d/disk2d.h>
# include <x3d-geometry2d/polyline2d.h>
# include <x3d-geometry2d/polypoint2d.h>
# include <x3d-geometry2d/rectangle2d.h>
# include <x3d-geometry2d/triangle_set2d.h>

void register_geometry2d_node_metatypes(openvrml::browser & b)
{
    using boost::shared_ptr;
    using openvrml::node_metatype;
    using namespace openvrml_node_x3d_geometry2d;
    b.add_node_metatype(arc2d_metatype::id,
                        shared_ptr<node_metatype>(new arc2d_metatype(b)));
    b.add_node_metatype(arc_close2d_metatype::id,
                        shared_ptr<node_metatype>(new arc_close2d_metatype(b)));
    b.add_node_metatype(circle2d_metatype::id,
                        shared_ptr<node_metatype>(new circle2d_metatype(b)));
    b.add_node_metatype(disk2d_metatype::id,
                        shared_ptr<node_metatype>(new disk2d_metatype(b)));
    b.add_node_metatype(polyline2d_metatype::id,
                        shared_ptr<node_metatype>(new polyline2d_metatype(b)));
    b.add_node_metatype(polypoint2d_metatype::id,
                        shared_ptr<node_metatype>(new polypoint2d_metatype(b)));
    b.add_node_metatype(rectangle2d_metatype::id,
                        shared_ptr<node_metatype>(new rectangle2d_metatype(b)));
    b.add_node_metatype(triangle_set2d_metatype::id,
                        shared_ptr<node_metatype>(
                            new triangle_set2d_metatype(b)));
}
