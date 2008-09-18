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
# include "x3d_rendering.h"
# include <x3d-rendering/color_rgba.h>
# include <x3d-rendering/indexed_triangle_fan_set.h>
# include <x3d-rendering/indexed_triangle_set.h>
# include <x3d-rendering/indexed_triangle_strip_set.h>
# include <x3d-rendering/triangle_fan_set.h>
# include <x3d-rendering/triangle_set.h>
# include <x3d-rendering/triangle_strip_set.h>

void register_rendering_node_metatypes(openvrml::browser & b)
{
    using boost::shared_ptr;
    using openvrml::node_metatype;
    using namespace openvrml_node_x3d_rendering;
    b.add_node_metatype(color_rgba_metatype::id,
                        shared_ptr<node_metatype>(new color_rgba_metatype(b)));
    b.add_node_metatype(indexed_triangle_fan_set_metatype::id,
                        shared_ptr<node_metatype>(
                            new indexed_triangle_fan_set_metatype(b)));
    b.add_node_metatype(indexed_triangle_set_metatype::id,
                        shared_ptr<node_metatype>(
                            new indexed_triangle_set_metatype(b)));
    b.add_node_metatype(indexed_triangle_strip_set_metatype::id,
                        shared_ptr<node_metatype>(
                            new indexed_triangle_strip_set_metatype(b)));
    b.add_node_metatype(triangle_fan_set_metatype::id,
                        shared_ptr<node_metatype>(
                            new triangle_fan_set_metatype(b)));
    b.add_node_metatype(triangle_set_metatype::id,
                        shared_ptr<node_metatype>(
                            new triangle_set_metatype(b)));
    b.add_node_metatype(triangle_strip_set_metatype::id,
                        shared_ptr<node_metatype>(
                            new triangle_strip_set_metatype(b)));
}
