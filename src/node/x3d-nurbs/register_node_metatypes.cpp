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

# include "contour2d.h"
# include "contour_polyline2d.h"
# include "coordinate_double.h"
# include "nurbs_curve.h"
# include "nurbs_curve2d.h"
# include "nurbs_orientation_interpolator.h"
# include "nurbs_patch_surface.h"
# include "nurbs_position_interpolator.h"
# include "nurbs_set.h"
# include "nurbs_surface_interpolator.h"
# include "nurbs_swept_surface.h"
# include "nurbs_swung_surface.h"
# include "nurbs_texture_coordinate.h"
# include "nurbs_trimmed_surface.h"
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
    using namespace openvrml_node_x3d_nurbs;

    openvrml::browser & b = registry.browser();

    registry.register_node_metatype(
        contour2d_metatype::id,
        shared_ptr<node_metatype>(new contour2d_metatype(b)));
    registry.register_node_metatype(
        contour_polyline2d_metatype::id,
        shared_ptr<node_metatype>(new contour_polyline2d_metatype(b)));
    registry.register_node_metatype(
        coordinate_double_metatype::id,
        shared_ptr<node_metatype>(new coordinate_double_metatype(b)));
    registry.register_node_metatype(
        nurbs_curve_metatype::id,
        shared_ptr<node_metatype>(new nurbs_curve_metatype(b)));
    registry.register_node_metatype(
        nurbs_curve2d_metatype::id,
        shared_ptr<node_metatype>(new nurbs_curve2d_metatype(b)));
    registry.register_node_metatype(
        nurbs_orientation_interpolator_metatype::id,
        shared_ptr<node_metatype>(
            new nurbs_orientation_interpolator_metatype(b)));
    registry.register_node_metatype(
        nurbs_patch_surface_metatype::id,
        shared_ptr<node_metatype>(new nurbs_patch_surface_metatype(b)));
    registry.register_node_metatype(
        nurbs_position_interpolator_metatype::id,
        shared_ptr<node_metatype>(new nurbs_position_interpolator_metatype(b)));
    registry.register_node_metatype(
        nurbs_set_metatype::id,
        shared_ptr<node_metatype>(new nurbs_set_metatype(b)));
    registry.register_node_metatype(
        nurbs_surface_interpolator_metatype::id,
        shared_ptr<node_metatype>(new nurbs_surface_interpolator_metatype(b)));
    registry.register_node_metatype(
        nurbs_swept_surface_metatype::id,
        shared_ptr<node_metatype>(new nurbs_swept_surface_metatype(b)));
    registry.register_node_metatype(
        nurbs_swung_surface_metatype::id,
        shared_ptr<node_metatype>(new nurbs_swung_surface_metatype(b)));
    registry.register_node_metatype(
        nurbs_texture_coordinate_metatype::id,
        shared_ptr<node_metatype>(new nurbs_texture_coordinate_metatype(b)));
    registry.register_node_metatype(
        nurbs_trimmed_surface_metatype::id,
        shared_ptr<node_metatype>(new nurbs_trimmed_surface_metatype(b)));
}
