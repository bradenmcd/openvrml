// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
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

# include "browser.h"
# include "x3d_interpolation.h"
# include <x3d-interpolation/coordinate_interpolator2d.h>
# include <x3d-interpolation/position_interpolator2d.h>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

void register_interpolation_node_metatypes(openvrml::browser & b)
{
    using boost::shared_ptr;
    using openvrml::node_metatype;
    using namespace openvrml_node_x3d_interpolation;
    b.add_node_metatype(coordinate_interpolator2d_metatype::id,
                        shared_ptr<node_metatype>(
                            new coordinate_interpolator2d_metatype(b)));
    b.add_node_metatype(position_interpolator2d_metatype::id,
                        shared_ptr<node_metatype>(
                            new position_interpolator2d_metatype(b)));
}
