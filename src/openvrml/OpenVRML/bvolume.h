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

# ifndef OPENVRML_BVOLUME_H
#   define OPENVRML_BVOLUME_H

#   include <iosfwd>
#   include <vector>
#   include <OpenVRML/basetypes.h>

namespace OpenVRML {

    class VrmlFrustum;
    class AABox;
    class BSphere;
    class mat4f;

    class OPENVRML_SCOPE BVolume {
    public:
        enum Intersection { inside = 1, outside = -1, partial = 0 };

        virtual ~BVolume() = 0;
        virtual bool isMAX() const = 0;
        virtual void setMAX() = 0;
        virtual Intersection
        intersectFrustum(const VrmlFrustum & frustum) const = 0;
        virtual void extend(const BVolume & b) = 0;
        virtual void extend(const vec3f & p) = 0;
        virtual void extend(const AABox & b) = 0;
        virtual void extend(const BSphere & b) = 0;
        virtual void enclose(const std::vector<vec3f> & points) = 0;
        virtual void orthoTransform(const mat4f & M) = 0;
        virtual void transform(const mat4f & M) = 0;
    };


    class OPENVRML_SCOPE BSphere : public BVolume {
        vec3f center;
        float radius;

    public:
        BSphere();
        virtual ~BSphere();

        void reset();

        virtual Intersection
        intersectFrustum(const VrmlFrustum & frustum) const;

        virtual void extend(const BVolume & b);
        virtual void extend(const vec3f & p);
        virtual void extend(const AABox & b);
        virtual void extend(const BSphere & b);
        virtual void enclose(const std::vector<vec3f> & points);

        virtual bool isMAX() const;
        virtual void setMAX();

        virtual void orthoTransform(const mat4f & M);
        virtual void transform(const mat4f & M);

        const vec3f & getCenter() const;
        void setCenter(const vec3f & c);

        float getRadius() const;
        void setRadius(float r);
    };


    class OPENVRML_SCOPE AABox : public BVolume {
    public:
        virtual ~AABox();
        virtual Intersection
        intersectFrustum(const VrmlFrustum & frustum) const;
        virtual void extend(const BVolume & b);
        virtual void extend(const vec3f & p);
        virtual void extend(const AABox & b);
        virtual void extend(const BSphere & b);
        virtual void enclose(const std::vector<vec3f> & points);
        virtual bool isMAX() const;
        virtual void setMAX();
        virtual void orthoTransform(const mat4f & M);
        virtual void transform(const mat4f & M);
    };
}

#endif
