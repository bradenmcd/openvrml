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
#   include <openvrml/basetypes.h>

namespace openvrml {

    class frustum;
    class axis_aligned_bounding_box;
    class bounding_sphere;
    class mat4f;

    class bounding_volume {
    public:
        enum intersection {
            inside = 1,
            outside = -1,
            partial = 0
        };

        virtual ~bounding_volume() = 0;
        virtual void maximize() = 0;
        virtual bool maximized() const = 0;
        virtual intersection
        intersect_frustum(const openvrml::frustum & frustum) const = 0;
        virtual void extend(const bounding_volume & bv) = 0;
        virtual void extend(const vec3f & p) = 0;
        virtual void extend(const axis_aligned_bounding_box & bbox) = 0;
        virtual void extend(const bounding_sphere & bs) = 0;
        virtual void enclose(const std::vector<vec3f> & points) = 0;
        virtual void ortho_transform(const mat4f & M) = 0;
        virtual void transform(const mat4f & M) = 0;
    };


    class bounding_sphere : public bounding_volume {
        vec3f center_;
        float radius_;

    public:
        bounding_sphere();
        virtual ~bounding_sphere();

        virtual intersection
        intersect_frustum(const openvrml::frustum & frustum) const;

        virtual void extend(const bounding_volume & b);
        virtual void extend(const vec3f & p);
        virtual void extend(const axis_aligned_bounding_box & b);
        virtual void extend(const bounding_sphere & b);
        virtual void enclose(const std::vector<vec3f> & points);

        virtual void maximize();
        virtual bool maximized() const;

        virtual void ortho_transform(const mat4f & t);
        virtual void transform(const mat4f & t);

        void center(const vec3f & c);
        const vec3f & center() const;

        void radius(float r);
        float radius() const;
    };


    class axis_aligned_bounding_box : public bounding_volume {
    public:
        virtual ~axis_aligned_bounding_box();
        virtual intersection
        intersect_frustum(const openvrml::frustum & frustum) const;
        virtual void extend(const bounding_volume & b);
        virtual void extend(const vec3f & p);
        virtual void extend(const axis_aligned_bounding_box & b);
        virtual void extend(const bounding_sphere & b);
        virtual void enclose(const std::vector<vec3f> & points);
        virtual void maximize();
        virtual bool maximized() const;
        virtual void ortho_transform(const mat4f & M);
        virtual void transform(const mat4f & M);
    };
}

#endif
