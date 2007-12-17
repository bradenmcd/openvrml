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

# ifndef OPENVRML_FRUSTUM_H
#   define OPENVRML_FRUSTUM_H

namespace openvrml {

    class OPENVRML_API frustum {
    public:
        float fovy;
        float fovx;
        double z_near;
        double z_far;
        float left_plane[4];
        float right_plane[4];
        float top_plane[4];
        float bot_plane[4];

        frustum();
        frustum(float fovy, float aspect, double z_near, double z_far);

    private:
        void update();
    };
}

# endif // OPENVRML_FRUSTUM_H
