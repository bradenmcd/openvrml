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

#   include <openvrml/basetypes.h>

namespace openvrml {

    class frustum;
    class axis_aligned_bounding_box;
    class bounding_sphere;

    class OPENVRML_API bounding_volume {
    public:
        enum intersection {
            outside = -1,
            partial = 0,
            inside = 1
        };

        virtual ~bounding_volume() OPENVRML_NOTHROW = 0;

        void maximize();
        bool maximized() const OPENVRML_NOTHROW;
        intersection
        intersect_frustum(const openvrml::frustum & frustum) const;
        void extend(const vec3f & p);
        void extend(const bounding_volume & bv);
        void enclose(const std::vector<vec3f> & points);
        void ortho_transform(const mat4f & M);
        void transform(const mat4f & M);

    private:
        virtual void do_maximize() = 0;
        virtual bool do_maximized() const = 0;
        virtual intersection
        do_intersect_frustum(const openvrml::frustum & frustum) const = 0;
        virtual void do_extend(const vec3f & p) = 0;
        virtual void do_extend(const axis_aligned_bounding_box & bbox) = 0;
        virtual void do_extend(const bounding_sphere & bs) = 0;
        virtual void do_enclose(const std::vector<vec3f> & points) = 0;
        virtual void do_ortho_transform(const mat4f & M) = 0;
        virtual void do_transform(const mat4f & M) = 0;
    };


    class OPENVRML_API bounding_sphere : public bounding_volume {
        vec3f center_;
        float radius_;

    public:
        bounding_sphere();
        virtual ~bounding_sphere() OPENVRML_NOTHROW;

        void center(const vec3f & c);
        const vec3f & center() const;
        void radius(float r);
        float radius() const;

    private:
        virtual void do_maximize();
        virtual bool do_maximized() const;
        virtual intersection
        do_intersect_frustum(const openvrml::frustum & frustum) const;
        virtual void do_extend(const vec3f & p);
        virtual void do_extend(const axis_aligned_bounding_box & b);
        virtual void do_extend(const bounding_sphere & b);
        virtual void do_enclose(const std::vector<vec3f> & points);
        virtual void do_ortho_transform(const mat4f & t);
        virtual void do_transform(const mat4f & t);
    };


    class OPENVRML_API axis_aligned_bounding_box : public bounding_volume {
    public:
        virtual ~axis_aligned_bounding_box() OPENVRML_NOTHROW;

    private:
        virtual void do_maximize();
        virtual bool do_maximized() const;
        virtual intersection
        do_intersect_frustum(const openvrml::frustum & frustum) const;
        virtual void do_extend(const vec3f & p);
        virtual void do_extend(const axis_aligned_bounding_box & b);
        virtual void do_extend(const bounding_sphere & b);
        virtual void do_enclose(const std::vector<vec3f> & points);
        virtual void do_ortho_transform(const mat4f & M);
        virtual void do_transform(const mat4f & M);
    };
}

#endif
