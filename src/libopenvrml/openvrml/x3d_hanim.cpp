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
# include "x3d_hanim.h"
# include <x3d-h-anim/h_anim_displacer.h>
# include <x3d-h-anim/h_anim_humanoid.h>
# include <x3d-h-anim/h_anim_joint.h>
# include <x3d-h-anim/h_anim_segment.h>
# include <x3d-h-anim/h_anim_site.h>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

void register_hanim_node_metatypes(openvrml::browser & b)
{
    using boost::shared_ptr;
    using openvrml::node_metatype;
    using namespace openvrml_node_x3d_h_anim;
    b.add_node_metatype(h_anim_displacer_metatype::id,
                        shared_ptr<node_metatype>(
                            new h_anim_displacer_metatype(b)));
    b.add_node_metatype(h_anim_humanoid_metatype::id,
                        shared_ptr<node_metatype>(
                            new h_anim_humanoid_metatype(b)));
    b.add_node_metatype(h_anim_joint_metatype::id,
                        shared_ptr<node_metatype>(
                            new h_anim_joint_metatype(b)));
    b.add_node_metatype(h_anim_segment_metatype::id,
                        shared_ptr<node_metatype>(
                            new h_anim_segment_metatype(b)));
    b.add_node_metatype(h_anim_site_metatype::id,
                        shared_ptr<node_metatype>(new h_anim_site_metatype(b)));
}
