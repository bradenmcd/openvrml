//
// OpenVRML
//
// Copyright (C) 2000  Christopher K. St. John
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// 

#ifndef VRMLFRUSTUM_H
#define VRMLFRUSTUM_H

#include <iostream.h>
#include "common.h"

namespace OpenVRML {

    class OPENVRML_SCOPE VrmlFrustum {
    public:
        float fovy;
        float fovx;
        double z_near;
        double z_far;
        float left_plane[4];
        float right_plane[4];
        float top_plane[4];
        float bot_plane[4];

        VrmlFrustum();
        VrmlFrustum(float afovy, float aaspect, double anear, double afar);

        ostream & dump(ostream & ostr) const;

    private:
        void update();
    };
}

#endif // VRMLFRUSTUM_H
