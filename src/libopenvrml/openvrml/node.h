// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
// Copyright (C) 2002  Braden McDaniel
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

# ifndef OPENVRML_NODE_H
#   define OPENVRML_NODE_H

#   include <deque>
#   include <iostream>
#   include <list>
#   include <set>
#   include <stdexcept>
#   include <utility>
#   include <openvrml/field.h>
#   include <openvrml/field_value_ptr.h>
#   include <openvrml/node_type_ptr.h>
#   include <openvrml/scope_ptr.h>
#   include <openvrml/viewer.h>
#   include <openvrml/rendering_context.h>

namespace openvrml {

    class node_interface {
    public:
        enum type_id {
            invalid_type_id,
            eventin_id,
            eventout_id,
            exposedfield_id,
            field_id
        };

        type_id type;
        field_value::type_id field_type;
        std::string id;

        node_interface(type_id type,
                       field_value::type_id field_type,
                       const std::string & id);
    };

    std::ostream & operator<<(std::ostream & out,
                              node_interface::type_id type);
    std::istream & operator>>(std::istream & in,
                              node_interface::type_id & type);

    inline bool operator==(const node_interface & lhs,
                           const node_interface & rhs)
        throw ()
    {
        return lhs.type == rhs.type
            && lhs.field_type == rhs.field_type
            && lhs.id == rhs.id;
    }

    inline bool operator!=(const node_interface & lhs,
                           const node_interface & rhs)
        throw ()
    {
        return !(lhs == rhs);
    }

    class node_type;

    class unsupported_interface : public std::runtime_error {
    public:
        explicit unsupported_interface(const std::string & message);
        unsupported_interface(const node_type & type,
                              const std::string & interface_id);
        unsupported_interface(const node_type & type,
                              node_interface::type_id interface_type,
                              const std::string & interface_id);
        virtual ~unsupported_interface() throw ();
    };


    class node_interface_set {
        struct id_less :
                std::binary_function<node_interface, node_interface, bool> {
            bool operator()(const node_interface & lhs,
                            const node_interface & rhs) const
            {
                return lhs.id < rhs.id;
            }
        };

        std::set<node_interface, id_less> interfaces;

    public:
        typedef std::set<node_interface, id_less>::const_iterator
            const_iterator;

        void add(const node_interface & interface)
            throw (std::invalid_argument, std::bad_alloc);
        const_iterator begin() const throw ();
        const_iterator end() const throw ();
        const_iterator find(const std::string & id) const throw ();
    };

    inline node_interface_set::const_iterator node_interface_set::begin() const
        throw ()
    {
        return this->interfaces.begin();
    }

    inline node_interface_set::const_iterator node_interface_set::end() const
        throw ()
    {
        return this->interfaces.end();
    }


    class browser;
    class viewpoint_node;

    class node_class {
    public:
        openvrml::browser & browser;

        virtual ~node_class() throw () = 0;
        virtual void initialize(viewpoint_node * initial_viewpoint,
                                double time) throw ();
        virtual void render(openvrml::viewer & viewer) throw ();
        virtual const node_type_ptr
        create_type(const std::string & id,
                    const node_interface_set & interfaces)
            throw (unsupported_interface, std::bad_alloc) = 0;

    protected:
        explicit node_class(openvrml::browser & b) throw ();
    };

    class node_type {
    public:
        openvrml::node_class & node_class;
        const std::string id;

        virtual ~node_type() throw () = 0;

        field_value::type_id has_eventin(const std::string & id) const
            throw ();
        field_value::type_id has_eventout(const std::string & id) const
            throw ();
        field_value::type_id has_field(const std::string & id) const
            throw ();
        field_value::type_id has_exposedfield(const std::string & id) const
            throw ();

        virtual const node_interface_set & interfaces() const throw () = 0;
        virtual const node_ptr create_node(const scope_ptr & scope) const
            throw (std::bad_alloc) = 0;

    protected:
        node_type(openvrml::node_class & c, const std::string & id)
            throw (std::bad_alloc);
    };


    class field_value_type_mismatch :
        public std::runtime_error {
    public:
        field_value_type_mismatch();
        virtual ~field_value_type_mismatch() throw ();
    };


    typedef std::deque<node *> node_path;


    class bounding_volume;
    class script_node;
    class appearance_node;
    class child_node;
    class color_node;
    class coordinate_node;
    class font_style_node;
    class geometry_node;
    class grouping_node;
    class material_node;
    class normal_node;
    class sound_source_node;
    class texture_node;
    class texture_coordinate_node;
    class texture_transform_node;
    class transform_node;
    class viewpoint_node;

    namespace vrml97_node {
        class anchor_node;
        class audio_clip_node;
        class cylinder_sensor_node;
        class abstract_light_node;
        class movie_texture_node;
        class navigation_info_node;
        class plane_sensor_node;
        class point_light_node;
        class sphere_sensor_node;
        class spot_light_node;
        class time_sensor_node;
        class touch_sensor_node;
    }

    class scene;

    std::ostream & operator<<(std::ostream & out, const node & n);

    class node {
        friend std::ostream & operator<<(std::ostream & out,
                                         const node & n);

    public:
        class route {
        public:
            const std::string from_eventout;
            const node_ptr to_node;
            const std::string to_eventin;

            route(const std::string & from_eventout, const node_ptr & to_node,
                  const std::string & to_eventin);
            route(const route & route);
        };

        typedef std::list<route> routes_t;

        struct polled_eventout_value {
            const field_value_ptr value;
            bool modified;

            polled_eventout_value();
            polled_eventout_value(const field_value_ptr & value,
                                  bool modified);
        };

    private:
        scope_ptr scope_;
        openvrml::scene * scene_;
        bool modified_;
        bool bounding_volume_dirty_;
        routes_t routes_;

        typedef std::map<std::string, polled_eventout_value *>
            eventout_is_map_t;
        eventout_is_map_t eventout_is_map;

    public:
        const node_type & type;

        virtual ~node() throw () = 0;

        const std::string & id() const throw ();
        void id(const std::string & node_id) throw (std::bad_alloc);

        const scope_ptr & scope() const throw ();

        openvrml::scene * scene() const throw ();

        std::ostream & print(std::ostream & out, size_t indent) const;

        void add_eventout_is(const std::string & eventout_id,
                             polled_eventout_value & eventout_value)
            throw (unsupported_interface, std::bad_alloc);

        void initialize(openvrml::scene & scene, double timestamp)
            throw (std::bad_alloc);
        void relocate() throw (std::bad_alloc);

        const field_value & field(const std::string & id) const
            throw (unsupported_interface);
        void field(const std::string & id, const field_value & value)
            throw (unsupported_interface, std::bad_cast, std::bad_alloc);
        void process_event(const std::string & id, const field_value & value,
                           double timestamp)
            throw (unsupported_interface, std::bad_cast, std::bad_alloc);
        const field_value & eventout(const std::string & id) const
            throw (unsupported_interface);

        void shutdown(double timestamp) throw ();

        virtual const script_node * to_script() const throw ();
        virtual script_node * to_script() throw ();
        virtual const appearance_node * to_appearance() const throw ();
        virtual appearance_node * to_appearance() throw ();
        virtual const child_node * to_child() const throw ();
        virtual child_node * to_child() throw ();
        virtual const color_node * to_color() const throw ();
        virtual color_node * to_color() throw ();
        virtual const coordinate_node * to_coordinate() const throw ();
        virtual coordinate_node * to_coordinate() throw ();
        virtual const font_style_node * to_font_style() const throw ();
        virtual font_style_node * to_font_style() throw () ;
        virtual const geometry_node * to_geometry() const throw ();
        virtual geometry_node * to_geometry() throw ();
        virtual const grouping_node * to_grouping() const throw ();
        virtual grouping_node * to_grouping() throw ();
        virtual const material_node * to_material() const throw ();
        virtual material_node * to_material() throw ();
        virtual const normal_node * to_normal() const throw ();
        virtual normal_node * to_normal() throw ();
        virtual const sound_source_node * to_sound_source() const throw ();
        virtual sound_source_node * to_sound_source() throw ();
        virtual const texture_node * to_texture() const throw ();
        virtual texture_node * to_texture() throw ();
        virtual const texture_coordinate_node * to_texture_coordinate() const
            throw ();
        virtual texture_coordinate_node * to_texture_coordinate() throw ();
        virtual const texture_transform_node * to_texture_transform() const
            throw ();
        virtual texture_transform_node * to_texture_transform() throw ();
        virtual const transform_node * to_transform() const throw ();
        virtual transform_node * to_transform() throw ();
        virtual const viewpoint_node * to_viewpoint() const throw ();
        virtual viewpoint_node * to_viewpoint() throw ();

        virtual vrml97_node::anchor_node * to_anchor() const;
        virtual vrml97_node::audio_clip_node * to_audio_clip() const;
        virtual vrml97_node::cylinder_sensor_node * to_cylinder_sensor() const;
        virtual vrml97_node::abstract_light_node * to_light() const;
        virtual vrml97_node::movie_texture_node * to_movie_texture() const;
        virtual vrml97_node::navigation_info_node * to_navigation_info() const;
        virtual vrml97_node::plane_sensor_node * to_plane_sensor() const;
        virtual vrml97_node::point_light_node * to_point_light() const;
        virtual vrml97_node::sphere_sensor_node * to_sphere_sensor() const;
        virtual vrml97_node::spot_light_node * to_spot_light() const;
        virtual vrml97_node::time_sensor_node * to_time_sensor() const;
        virtual vrml97_node::touch_sensor_node * to_touch_sensor() const;

        virtual bool modified() const;
        void modified(bool value);

        virtual void bounding_volume(const openvrml::bounding_volume & v);
        virtual const openvrml::bounding_volume & bounding_volume() const;

        virtual void bounding_volume_dirty(bool f);
        virtual bool bounding_volume_dirty() const;

        void add_route(const std::string & from_eventout,
                       const node_ptr & to_node,
                       const std::string & to_eventin)
            throw (unsupported_interface, field_value_type_mismatch);
        void delete_route(const std::string & from_eventout,
                          const node_ptr & to_node,
                          const std::string & to_eventin)
            throw ();

        const routes_t & routes() const;

        virtual void render(openvrml::viewer & viewer,
                            rendering_context context);

    protected:
        node(const node_type & type, const scope_ptr & scope) throw ();

        void emit_event(const std::string & id,
                        const field_value & value,
                        double timestamp)
            throw (std::bad_cast, std::bad_alloc);

    private:
        // Not copyable.
        node(const node &);
        node & operator=(const node &);

        virtual void do_initialize(double timestamp) throw (std::bad_alloc);
        virtual void do_relocate() throw (std::bad_alloc);

        virtual void do_field(const std::string & id,
                              const field_value & value)
            throw (unsupported_interface, std::bad_cast, std::bad_alloc) = 0;
        virtual const field_value & do_field(const std::string & id) const
            throw (unsupported_interface) = 0;
        virtual void do_process_event(const std::string & id,
                                      const field_value & value,
                                      double timestamp)
            throw (unsupported_interface, std::bad_cast, std::bad_alloc) = 0;
        virtual const field_value &
        do_eventout(const std::string & id) const
            throw (unsupported_interface) = 0;
        virtual void do_shutdown(double timestamp) throw ();
    };

    inline const scope_ptr & node::scope() const throw ()
    {
        return this->scope_;
    }

    inline openvrml::scene * node::scene() const throw ()
    {
        return this->scene_;
    }

    inline bool operator==(const node::route & lhs, const node::route & rhs)
        throw ()
    {
        return lhs.from_eventout == rhs.from_eventout
            && lhs.to_node == rhs.to_node
            && lhs.to_eventin == rhs.to_eventin;
    }

    inline bool operator!=(const node::route & lhs, const node::route & rhs)
        throw ()
    {
        return !(lhs == rhs);
    }


    class appearance_node : public virtual node {
    public:
        virtual ~appearance_node() throw () = 0;

        virtual const appearance_node * to_appearance() const throw ();
        virtual appearance_node * to_appearance() throw ();

        virtual const node_ptr & material() const throw () = 0;
        virtual const node_ptr & texture() const throw () = 0;
        virtual const node_ptr & texture_transform() const throw () = 0;

    protected:
        appearance_node(const node_type & type, const scope_ptr & scope)
            throw ();
    };


    class child_node : public virtual node {
    public:
        virtual ~child_node() throw () = 0;

        virtual const child_node * to_child() const throw ();
        virtual child_node * to_child() throw ();

    protected:
        child_node(const node_type & type, const scope_ptr & scope) throw ();
    };


    class color_node : public virtual node {
    public:
        virtual ~color_node() throw () = 0;

        virtual const color_node * to_color() const throw ();
        virtual color_node * to_color() throw ();

        virtual const std::vector<openvrml::color> & color() const
            throw () = 0;

    protected:
        color_node(const node_type & type, const scope_ptr & scope) throw ();
    };


    class coordinate_node : public virtual node {
    public:
        virtual ~coordinate_node() throw () = 0;

        virtual const coordinate_node * to_coordinate() const throw ();
        virtual coordinate_node * to_coordinate() throw ();

        virtual const std::vector<vec3f> & point() const throw () = 0;

    protected:
        coordinate_node(const node_type & type, const scope_ptr & scope)
            throw ();
    };


    class font_style_node : public virtual node {
    public:
        virtual ~font_style_node() throw () = 0;

        virtual const font_style_node * to_font_style() const throw ();
        virtual font_style_node * to_font_style() throw ();

        virtual const std::vector<std::string> & family() const
            throw () = 0;
        virtual bool horizontal() const throw () = 0;
        virtual const std::vector<std::string> & justify() const
            throw () = 0;
        virtual const std::string & language() const throw () = 0;
        virtual bool left_to_right() const throw () = 0;
        virtual float size() const throw () = 0;
        virtual float spacing() const throw () = 0;
        virtual const std::string & style() const throw () = 0;
        virtual bool top_to_bottom() const throw () = 0;

    protected:
        font_style_node(const node_type & type, const scope_ptr & scope)
            throw ();
    };


    class geometry_node : public virtual node {
    public:
        virtual ~geometry_node() throw () = 0;

        virtual const geometry_node * to_geometry() const throw ();
        virtual geometry_node * to_geometry() throw ();

        virtual viewer::object_t
        insert_geometry(openvrml::viewer & v,
                        rendering_context context) = 0;
        virtual const color_node * color() const throw ();

    protected:
        geometry_node(const node_type & type, const scope_ptr & scope)
            throw ();
    };


    class grouping_node : public virtual child_node {
    public:
        virtual ~grouping_node() throw () = 0;

        virtual const grouping_node * to_grouping() const throw ();
        virtual grouping_node * to_grouping() throw ();

        virtual const std::vector<node_ptr> & children() const throw () = 0;
        virtual void activate(double timestamp, bool over, bool active,
                              double *p) = 0;

    protected:
        grouping_node(const node_type & type, const scope_ptr & scope)
            throw ();
    };


    class material_node : public virtual node {
    public:
        virtual ~material_node() throw () = 0;

        virtual const material_node * to_material() const throw ();
        virtual material_node * to_material() throw ();

        virtual float ambient_intensity() const throw () = 0;
        virtual const color & diffuse_color() const throw () = 0;
        virtual const color & emissive_color() const throw () = 0;
        virtual float shininess() const throw () = 0;
        virtual const color & specular_color() const throw () = 0;
        virtual float transparency() const throw () = 0;

    protected:
        material_node(const node_type & type, const scope_ptr & scope)
            throw ();
    };


    class normal_node : public virtual node {
    public:
        virtual ~normal_node() throw () = 0;

        virtual const normal_node * to_normal() const throw ();
        virtual normal_node * to_normal() throw ();

        virtual const std::vector<vec3f> & vector() const throw () = 0;

    protected:
        normal_node(const node_type & type, const scope_ptr & scope) throw ();
    };


    class sound_source_node : public virtual node {
    public:
        virtual ~sound_source_node() throw () = 0;
        virtual const sound_source_node * to_sound_source() const throw ();
        virtual sound_source_node * to_sound_source() throw ();

    protected:
        sound_source_node(const node_type & type, const scope_ptr & scope)
            throw ();
    };


    class texture_node : public virtual node {
    public:
        virtual ~texture_node() throw () = 0;

        virtual const texture_node * to_texture() const throw ();
        virtual texture_node * to_texture() throw ();

        virtual size_t components() const throw () = 0;
        virtual size_t width() const throw () = 0;
        virtual size_t height() const throw () = 0;
        virtual size_t frames() const throw () = 0;
        virtual const unsigned char * pixels() const throw () = 0;
        virtual bool repeat_s() const throw () = 0;
        virtual bool repeat_t() const throw () = 0;

    protected:
        texture_node(const node_type & type, const scope_ptr & scope) throw ();
    };


    class texture_coordinate_node : public virtual node {
    public:
        virtual ~texture_coordinate_node() throw () = 0;

        virtual const texture_coordinate_node * to_texture_coordinate() const
            throw ();
        virtual texture_coordinate_node * to_texture_coordinate() throw ();

        virtual const std::vector<vec2f> & point() const throw () = 0;

    protected:
        texture_coordinate_node(const node_type & type,
                                const scope_ptr & scope)
            throw ();
    };


    class texture_transform_node : public virtual node {
    public:
        virtual ~texture_transform_node() throw () = 0;

        virtual const texture_transform_node * to_texture_transform() const
            throw ();
        virtual texture_transform_node * to_texture_transform() throw ();

    protected:
        texture_transform_node(const node_type & type,
                               const scope_ptr & scope)
            throw ();
    };


    class transform_node : public virtual grouping_node {
    public:
        virtual ~transform_node() throw () = 0;

        virtual const transform_node * to_transform() const throw ();
        virtual transform_node * to_transform() throw ();

        virtual const mat4f & transform() const throw () = 0;

    protected:
        transform_node(const node_type & type, const scope_ptr & scope)
            throw ();
    };


    class viewpoint_node : public virtual child_node {
    public:
        virtual ~viewpoint_node() throw () = 0;

        virtual const viewpoint_node * to_viewpoint() const throw ();
        virtual viewpoint_node * to_viewpoint() throw ();

        virtual const mat4f & transformation() const throw () = 0;
        virtual const mat4f & user_view_transform() const throw () = 0;
        virtual void user_view_transform(const mat4f & transform)
            throw () = 0;
        virtual const std::string & description() const throw () = 0;
        virtual float field_of_view() const throw () = 0;

    protected:
        viewpoint_node(const node_type & type, const scope_ptr & scope)
            throw ();
    };


    class node_traverser {
        std::set<node *> traversed_nodes;
        bool halt;

    public:
        node_traverser() throw (std::bad_alloc);
        virtual ~node_traverser() throw () = 0;

        void traverse(node & n);
        void traverse(const node_ptr & node);
        void traverse(const std::vector<node_ptr> & nodes);

    protected:
        void halt_traversal() throw ();

    private:
        // Noncopyable.
        node_traverser(const node_traverser &);
        node_traverser & operator=(const node_traverser &);

        virtual void on_entering(node & n);
        virtual void on_leaving(node & n);

        void do_traversal(node & n);
    };
}

# endif
