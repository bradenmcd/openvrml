// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2000  Christopher K. St. John
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

# ifndef VRMLRENDERCONTEXT_H
#   define VRMLRENDERCONTEXT_H

#   include <openvrml/bounding_volume.h>

namespace openvrml {

    class OPENVRML_API rendering_context {
        mat4f * modelview;

    public:
        bounding_volume::intersection cull_flag;
        bool draw_bounding_spheres;

        rendering_context();
        rendering_context(bounding_volume::intersection cull_flag,
                          mat4f & modelview);

        // Use compiler-generated copy-ctor, dtor, operator=.

        const mat4f & matrix() const;
        void matrix(mat4f & modelview);
    };
}

# endif
