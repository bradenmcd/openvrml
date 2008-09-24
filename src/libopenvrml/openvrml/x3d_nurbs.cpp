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

# include "browser.h"
# include "x3d_nurbs.h"
# include <x3d-nurbs/contour2d.h>
# include <x3d-nurbs/contour_polyline2d.h>
# include <x3d-nurbs/coordinate_double.h>
# include <x3d-nurbs/nurbs_curve.h>
# include <x3d-nurbs/nurbs_curve2d.h>
# include <x3d-nurbs/nurbs_orientation_interpolator.h>
# include <x3d-nurbs/nurbs_patch_surface.h>
# include <x3d-nurbs/nurbs_position_interpolator.h>
# include <x3d-nurbs/nurbs_set.h>
# include <x3d-nurbs/nurbs_surface_interpolator.h>
# include <x3d-nurbs/nurbs_swept_surface.h>
# include <x3d-nurbs/nurbs_swung_surface.h>
# include <x3d-nurbs/nurbs_texture_coordinate.h>
# include <x3d-nurbs/nurbs_trimmed_surface.h>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

void register_nurbs_node_metatypes(openvrml::browser & b)
{
    using boost::shared_ptr;
    using openvrml::node_metatype;
    using namespace openvrml_node_x3d_nurbs;
    b.add_node_metatype(contour2d_metatype::id,
                        shared_ptr<node_metatype>(new contour2d_metatype(b)));
    b.add_node_metatype(contour_polyline2d_metatype::id,
                        shared_ptr<node_metatype>(
                            new contour_polyline2d_metatype(b)));
    b.add_node_metatype(coordinate_double_metatype::id,
                        shared_ptr<node_metatype>(
                            new coordinate_double_metatype(b)));
    b.add_node_metatype(nurbs_curve_metatype::id,
                        shared_ptr<node_metatype>(
                            new nurbs_curve_metatype(b)));
    b.add_node_metatype(nurbs_curve2d_metatype::id,
                        shared_ptr<node_metatype>(
                            new nurbs_curve2d_metatype(b)));
    b.add_node_metatype(nurbs_orientation_interpolator_metatype::id,
                        shared_ptr<node_metatype>(
                            new nurbs_orientation_interpolator_metatype(b)));
    b.add_node_metatype(nurbs_patch_surface_metatype::id,
                        shared_ptr<node_metatype>(
                            new nurbs_patch_surface_metatype(b)));
    b.add_node_metatype(nurbs_position_interpolator_metatype::id,
                        shared_ptr<node_metatype>(
                            new nurbs_position_interpolator_metatype(b)));
    b.add_node_metatype(nurbs_set_metatype::id,
                        shared_ptr<node_metatype>(new nurbs_set_metatype(b)));
    b.add_node_metatype(nurbs_surface_interpolator_metatype::id,
                        shared_ptr<node_metatype>(
                            new nurbs_surface_interpolator_metatype(b)));
    b.add_node_metatype(nurbs_swept_surface_metatype::id,
                        shared_ptr<node_metatype>(
                            new nurbs_swept_surface_metatype(b)));
    b.add_node_metatype(nurbs_swung_surface_metatype::id,
                        shared_ptr<node_metatype>(
                            new nurbs_swung_surface_metatype(b)));
    b.add_node_metatype(nurbs_texture_coordinate_metatype::id,
                        shared_ptr<node_metatype>(
                            new nurbs_texture_coordinate_metatype(b)));
    b.add_node_metatype(nurbs_trimmed_surface_metatype::id,
                        shared_ptr<node_metatype>(
                            new nurbs_trimmed_surface_metatype(b)));
}
