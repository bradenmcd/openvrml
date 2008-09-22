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
# include "x3d_geospatial.h"
# include <x3d-geospatial/geo_coordinate.h>
# include <x3d-geospatial/geo_elevation_grid.h>
# include <x3d-geospatial/geo_location.h>
# include <x3d-geospatial/geo_lod.h>
# include <x3d-geospatial/geo_metadata.h>
# include <x3d-geospatial/geo_origin.h>
# include <x3d-geospatial/geo_position_interpolator.h>
# include <x3d-geospatial/geo_touch_sensor.h>
# include <x3d-geospatial/geo_viewpoint.h>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

void register_geospatial_node_metatypes(openvrml::browser & b)
{
    using boost::shared_ptr;
    using openvrml::node_metatype;
    using namespace openvrml_node_x3d_geospatial;
    b.add_node_metatype(geo_coordinate_metatype::id,
                        shared_ptr<node_metatype>(
                            new geo_coordinate_metatype(b)));
    b.add_node_metatype(geo_elevation_grid_metatype::id,
                        shared_ptr<node_metatype>(
                            new geo_elevation_grid_metatype(b)));
    b.add_node_metatype(geo_location_metatype::id,
                        shared_ptr<node_metatype>(
                            new geo_location_metatype(b)));
    b.add_node_metatype(geo_lod_metatype::id,
                        shared_ptr<node_metatype>(new geo_lod_metatype(b)));
    b.add_node_metatype(geo_metadata_metatype::id,
                        shared_ptr<node_metatype>(
                            new geo_metadata_metatype(b)));
    b.add_node_metatype(geo_origin_metatype::id,
                        shared_ptr<node_metatype>(new geo_origin_metatype(b)));
    b.add_node_metatype(geo_position_interpolator_metatype::id,
                        shared_ptr<node_metatype>(
                            new geo_position_interpolator_metatype(b)));
    b.add_node_metatype(geo_touch_sensor_metatype::id,
                        shared_ptr<node_metatype>(
                            new geo_touch_sensor_metatype(b)));
    b.add_node_metatype(geo_viewpoint_metatype::id,
                        shared_ptr<node_metatype>(
                            new geo_viewpoint_metatype(b)));
}
