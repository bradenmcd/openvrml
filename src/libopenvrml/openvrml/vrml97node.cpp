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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include "vrml97node.h"
# include "browser.h"

# include <vrml97/anchor.h>
# include <vrml97/appearance.h>
# include <vrml97/audio_clip.h>
# include <vrml97/background.h>
# include <vrml97/billboard.h>
# include <vrml97/box.h>
# include <vrml97/collision.h>
# include <vrml97/color.h>
# include <vrml97/color_interpolator.h>
# include <vrml97/cone.h>
# include <vrml97/coordinate.h>
# include <vrml97/coordinate_interpolator.h>
# include <vrml97/cylinder.h>
# include <vrml97/cylinder_sensor.h>
# include <vrml97/directional_light.h>
# include <vrml97/elevation_grid.h>
# include <vrml97/extrusion.h>
# include <vrml97/fog.h>
# include <vrml97/font_style.h>
# include <vrml97/group.h>
# include <vrml97/image_texture.h>
# include <vrml97/indexed_face_set.h>
# include <vrml97/indexed_line_set.h>
# include <vrml97/inline.h>
# include <vrml97/lod.h>
# include <vrml97/material.h>
# include <vrml97/movie_texture.h>
# include <vrml97/navigation_info.h>
# include <vrml97/normal.h>
# include <vrml97/normal_interpolator.h>
# include <vrml97/orientation_interpolator.h>
# include <vrml97/pixel_texture.h>
# include <vrml97/plane_sensor.h>
# include <vrml97/point_light.h>
# include <vrml97/point_set.h>
# include <vrml97/position_interpolator.h>
# include <vrml97/proximity_sensor.h>
# include <vrml97/scalar_interpolator.h>
# include <vrml97/shape.h>
# include <vrml97/sound.h>
# include <vrml97/sphere.h>
# include <vrml97/sphere_sensor.h>
# include <vrml97/spot_light.h>
# include <vrml97/switch.h>
# include <vrml97/text.h>
# include <vrml97/texture_coordinate.h>
# include <vrml97/texture_transform.h>
# include <vrml97/time_sensor.h>
# include <vrml97/touch_sensor.h>
# include <vrml97/transform.h>
# include <vrml97/viewpoint.h>
# include <vrml97/visibility_sensor.h>
# include <vrml97/world_info.h>
# include <vrml97/cad_layer.h>
# include <vrml97/cad_assembly.h>

template void std::vector<boost::intrusive_ptr<openvrml::node> >::pop_back();

void register_vrml97_node_metatypes(openvrml::browser & b)
{
    using boost::shared_ptr;
    using openvrml::node_metatype;
    using namespace openvrml_node_vrml97;
    b.add_node_metatype(anchor_metatype::id,
                        shared_ptr<node_metatype>(new anchor_metatype(b)));
    b.add_node_metatype(appearance_metatype::id,
                        shared_ptr<node_metatype>(new appearance_metatype(b)));
    b.add_node_metatype(audio_clip_metatype::id,
                        shared_ptr<node_metatype>(new audio_clip_metatype(b)));
    b.add_node_metatype(background_metatype::id,
                        shared_ptr<node_metatype>(new background_metatype(b)));
    b.add_node_metatype(billboard_metatype::id,
                        shared_ptr<node_metatype>(new billboard_metatype(b)));
    b.add_node_metatype(box_metatype::id,
                        shared_ptr<node_metatype>(new box_metatype(b)));
    b.add_node_metatype(collision_metatype::id,
                        shared_ptr<node_metatype>(new collision_metatype(b)));
    b.add_node_metatype(color_metatype::id,
                        shared_ptr<node_metatype>(new color_metatype(b)));
    b.add_node_metatype(color_interpolator_metatype::id,
                        shared_ptr<node_metatype>(
                            new color_interpolator_metatype(b)));
    b.add_node_metatype(cone_metatype::id,
                        shared_ptr<node_metatype>(new cone_metatype(b)));
    b.add_node_metatype(coordinate_metatype::id,
                        shared_ptr<node_metatype>(new coordinate_metatype(b)));
    b.add_node_metatype(coordinate_interpolator_metatype::id,
                        shared_ptr<node_metatype>(
                            new coordinate_interpolator_metatype(b)));
    b.add_node_metatype(cylinder_metatype::id,
                        shared_ptr<node_metatype>(new cylinder_metatype(b)));
    b.add_node_metatype(cylinder_sensor_metatype::id,
                        shared_ptr<node_metatype>(
                            new cylinder_sensor_metatype(b)));
    b.add_node_metatype(directional_light_metatype::id,
                        shared_ptr<node_metatype>(
                            new directional_light_metatype(b)));
    b.add_node_metatype(elevation_grid_metatype::id,
                        shared_ptr<node_metatype>(
                            new elevation_grid_metatype(b)));
    b.add_node_metatype(extrusion_metatype::id,
                        shared_ptr<node_metatype>(
                            new extrusion_metatype(b)));
    b.add_node_metatype(fog_metatype::id,
                        shared_ptr<node_metatype>(new fog_metatype(b)));
    b.add_node_metatype(font_style_metatype::id,
                        shared_ptr<node_metatype>(new font_style_metatype(b)));
    b.add_node_metatype(group_metatype::id,
                        shared_ptr<node_metatype>(new group_metatype(b)));
    b.add_node_metatype(image_texture_metatype::id,
                        shared_ptr<node_metatype>(
                            new image_texture_metatype(b)));
    b.add_node_metatype(indexed_face_set_metatype::id,
                        shared_ptr<node_metatype>(
                            new indexed_face_set_metatype(b)));
    b.add_node_metatype(indexed_line_set_metatype::id,
                        shared_ptr<node_metatype>(
                            new indexed_line_set_metatype(b)));
    b.add_node_metatype(inline_metatype::id,
                        shared_ptr<node_metatype>(
                            new inline_metatype(b)));
    b.add_node_metatype(lod_metatype::id,
                        shared_ptr<node_metatype>(new lod_metatype(b)));
    b.add_node_metatype(material_metatype::id,
                        shared_ptr<node_metatype>(new material_metatype(b)));
    b.add_node_metatype(movie_texture_metatype::id,
                        shared_ptr<node_metatype>(
                            new movie_texture_metatype(b)));
    b.add_node_metatype(navigation_info_metatype::id,
                        shared_ptr<node_metatype>(
                            new navigation_info_metatype(b)));
    b.add_node_metatype(normal_metatype::id,
                        shared_ptr<node_metatype>(new normal_metatype(b)));
    b.add_node_metatype(normal_interpolator_metatype::id,
                        shared_ptr<node_metatype>(
                            new normal_interpolator_metatype(b)));
    b.add_node_metatype(orientation_interpolator_metatype::id,
                        shared_ptr<node_metatype>(
                            new orientation_interpolator_metatype(b)));
    b.add_node_metatype(pixel_texture_metatype::id,
                        shared_ptr<node_metatype>(
                            new pixel_texture_metatype(b)));
    b.add_node_metatype(plane_sensor_metatype::id,
                        shared_ptr<node_metatype>(
                            new plane_sensor_metatype(b)));
    b.add_node_metatype(point_light_metatype::id,
                        shared_ptr<node_metatype>(
                            new point_light_metatype(b)));
    b.add_node_metatype(point_set_metatype::id,
                        shared_ptr<node_metatype>(
                            new point_set_metatype(b)));
    b.add_node_metatype(position_interpolator_metatype::id,
                        shared_ptr<node_metatype>(
                            new position_interpolator_metatype(b)));
    b.add_node_metatype(proximity_sensor_metatype::id,
                        shared_ptr<node_metatype>(
                            new proximity_sensor_metatype(b)));
    b.add_node_metatype(scalar_interpolator_metatype::id,
                        shared_ptr<node_metatype>(
                            new scalar_interpolator_metatype(b)));
    b.add_node_metatype(shape_metatype::id,
                        shared_ptr<node_metatype>(new shape_metatype(b)));
    b.add_node_metatype(sound_metatype::id,
                        shared_ptr<node_metatype>(new sound_metatype(b)));
    b.add_node_metatype(sphere_metatype::id,
                        shared_ptr<node_metatype>(new sphere_metatype(b)));
    b.add_node_metatype(sphere_sensor_metatype::id,
                        shared_ptr<node_metatype>(
                            new sphere_sensor_metatype(b)));
    b.add_node_metatype(spot_light_metatype::id,
                        shared_ptr<node_metatype>(new spot_light_metatype(b)));
    b.add_node_metatype(switch_metatype::id,
                        shared_ptr<node_metatype>(new switch_metatype(b)));
    b.add_node_metatype(text_metatype::id,
                        shared_ptr<node_metatype>(new text_metatype(b)));
    b.add_node_metatype(texture_coordinate_metatype::id,
                        shared_ptr<node_metatype>(
                            new texture_coordinate_metatype(b)));
    b.add_node_metatype(texture_transform_metatype::id,
                        shared_ptr<node_metatype>(
                            new texture_transform_metatype(b)));
    b.add_node_metatype(time_sensor_metatype::id,
                        shared_ptr<node_metatype>(
                            new time_sensor_metatype(b)));
    b.add_node_metatype(touch_sensor_metatype::id,
                        shared_ptr<node_metatype>(
                            new touch_sensor_metatype(b)));
    b.add_node_metatype(transform_metatype::id,
                        shared_ptr<node_metatype>(new transform_metatype(b)));
    b.add_node_metatype(viewpoint_metatype::id,
                        shared_ptr<node_metatype>(new viewpoint_metatype(b)));
    b.add_node_metatype(visibility_sensor_metatype::id,
                        shared_ptr<node_metatype>(
                            new visibility_sensor_metatype(b)));
    b.add_node_metatype(world_info_metatype::id,
                        shared_ptr<node_metatype>(new world_info_metatype(b)));
    b.add_node_metatype(cad_layer_metatype::id,
                        shared_ptr<node_metatype>(new cad_layer_metatype(b)));
    b.add_node_metatype(cad_assembly_metatype::id,
                        shared_ptr<node_metatype>(
                            new cad_assembly_metatype(b)));
}
