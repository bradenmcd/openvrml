// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
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

# ifndef VRMLRENDERCONTEXT_H
#   define VRMLRENDERCONTEXT_H

#   include <OpenVRML/common.h>
#   include <OpenVRML/bvolume.h>

namespace OpenVRML {

    class mat4f;

    class OPENVRML_SCOPE VrmlRenderContext {
        BVolume::Intersection cull_flag;
        mat4f * modelview;
        bool draw_bspheres;

    public:
        VrmlRenderContext();
        VrmlRenderContext(BVolume::Intersection cull_flag, mat4f & modelview);

        // Use compiler-generated copy-ctor, dtor, operator=.

        BVolume::Intersection getCullFlag() const;
        void setCullFlag(BVolume::Intersection intersection);
        const mat4f & getMatrix() const;
        void setMatrix(mat4f & modelview);
        void setDrawBSpheres(bool f);
        bool getDrawBSpheres() const;
    };
}

# endif
