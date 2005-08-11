// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2002, 2003, 2004, 2005  Braden McDaniel
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
#   include <list>
#   include <set>
#   include <utility>
#   include <boost/thread/recursive_mutex.hpp>
#   include <openvrml/field_value.h>
#   include <openvrml/viewer.h>
#   include <openvrml/rendering_context.h>
#   include <openvrml/scope.h>

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

    bool operator==(const node_interface & lhs, const node_interface & rhs)
        throw ();
    bool operator!=(const node_interface & lhs, const node_interface & rhs)
        throw ();

    std::ostream & operator<<(std::ostream & out,
                              const node_interface & interface);
    std::istream & operator>>(std::istream & in, node_interface & interface);

    class node_type;

    class unsupported_interface : public std::logic_error {
    public:
        explicit unsupported_interface(const node_interface & interface)
            throw ();
        unsupported_interface(const node_type & type,
                              const std::string & interface_id)
            throw ();
        unsupported_interface(const node_type & type,
                              node_interface::type_id interface_type,
                              const std::string & interface_id)
            throw ();
        virtual ~unsupported_interface() throw ();
    };


    struct node_interface_matches_eventin :
        std::binary_function<node_interface, std::string, bool> {
        result_type operator()(const first_argument_type & interface,
                               const second_argument_type & eventin_id) const
        {
            static const char eventin_prefix[] = "set_";
            return (interface.type == node_interface::eventin_id
                    && (eventin_id == interface.id
                        || eventin_prefix + eventin_id == interface.id))
                || (interface.type == node_interface::exposedfield_id
                    && (eventin_id == interface.id
                        || eventin_id == eventin_prefix + interface.id));
        }
    };


    struct node_interface_matches_eventout :
        std::binary_function<node_interface, std::string, bool> {
        result_type operator()(const first_argument_type & interface,
                               const second_argument_type & eventout_id) const
        {
            static const char eventout_suffix[] = "_changed";
            return (interface.type == node_interface::eventout_id
                    && (eventout_id == interface.id
                        || eventout_id + eventout_suffix == interface.id))
                || (interface.type == node_interface::exposedfield_id
                    && (eventout_id == interface.id
                        || eventout_id == interface.id + eventout_suffix));
        }
    };


    struct node_interface_matches_exposedfield :
        std::binary_function<node_interface, std::string, bool> {
        result_type
        operator()(const first_argument_type & interface,
                   const second_argument_type & exposedfield_id) const
        {
            return interface.type == node_interface::exposedfield_id
                && interface.id == exposedfield_id;
        }
    };


    struct node_interface_matches_field :
        std::binary_function<node_interface, std::string, bool> {
        result_type operator()(const first_argument_type & interface,
                               const second_argument_type & field_id) const
        {
            return (interface.type == node_interface::field_id
                    || interface.type == node_interface::exposedfield_id)
                && interface.id == field_id;
        }
    };


    struct node_interface_compare :
        std::binary_function<node_interface, node_interface, bool> {

        result_type operator()(const first_argument_type & lhs,
                               const second_argument_type & rhs) const
        {
            static const char eventin_prefix[] = "set_";
            static const char eventout_suffix[] = "_changed";

            if (lhs.type == node_interface::exposedfield_id) {
                if (rhs.type == node_interface::eventin_id) {
                    return eventin_prefix + lhs.id < rhs.id;
                } else if (rhs.type == node_interface::eventout_id) {
                    return lhs.id + eventout_suffix < rhs.id;
                }
            } else if (rhs.type == node_interface::exposedfield_id) {
                if (lhs.type == node_interface::eventin_id) {
                    return lhs.id < eventin_prefix + rhs.id;
                } else if (lhs.type == node_interface::eventout_id) {
                    return lhs.id < rhs.id + eventout_suffix;
                }
            }
            return lhs.id < rhs.id;
        }
    };


    typedef std::set<node_interface, node_interface_compare>
        node_interface_set;

    const node_interface_set::const_iterator
    find_interface(const node_interface_set & interfaces,
                   const std::string & id)
        throw ();


    class browser;
    class viewpoint_node;
    class node_type;

    class node_class : boost::noncopyable {
        openvrml::browser * browser_;

    public:
        virtual ~node_class() throw () = 0;

        openvrml::browser & browser() const throw ();
        void initialize(viewpoint_node * initial_viewpoint, double time)
            throw ();
        void render(viewer & v) const throw ();
        const boost::shared_ptr<node_type>
        create_type(const std::string & id,
                    const node_interface_set & interfaces)
            throw (unsupported_interface, std::bad_alloc);

    protected:
        explicit node_class(openvrml::browser & b) throw ();

    private:
        virtual void do_initialize(viewpoint_node * initial_viewpoint,
                                   double time)
            throw ();
        virtual void do_render(viewer & v) const throw ();
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            throw (unsupported_interface, std::bad_alloc) = 0;
    };


    typedef std::map<std::string, boost::shared_ptr<field_value> >
        initial_value_map;


    class node_type : boost::noncopyable {
        const openvrml::node_class & node_class_;
        const std::string id_;

    public:
        virtual ~node_type() throw () = 0;

        const openvrml::node_class & node_class() const throw ();
        const std::string & id() const throw ();
        const node_interface_set & interfaces() const throw ();
        const node_ptr create_node(const boost::shared_ptr<scope> & scope,
                                   const initial_value_map & initial_values =
                                   initial_value_map()) const
            throw (unsupported_interface, std::bad_cast, std::bad_alloc);

    protected:
        node_type(const openvrml::node_class & c, const std::string & id)
            throw (std::bad_alloc);

    private:
        virtual const node_interface_set & do_interfaces() const throw () = 0;
        virtual const node_ptr
        do_create_node(const boost::shared_ptr<scope> & scope,
                       const initial_value_map & initial_values) const
            throw (unsupported_interface, std::bad_cast, std::bad_alloc) = 0;
    };


    class field_value_type_mismatch : public std::logic_error {
    public:
        field_value_type_mismatch();
        virtual ~field_value_type_mismatch() throw ();
    };


    typedef std::deque<node *> node_path;


    class bounding_volume;
    class script_node;
    class appearance_node;
    class bounded_volume_node;
    class child_node;
    class color_node;
    class coordinate_node;
    class font_style_node;
    class geometry_node;
    class grouping_node;
    class light_node;
    class material_node;
    class navigation_info_node;
    class normal_node;
    class pointing_device_sensor_node;
    class scoped_light_node;
    class sound_source_node;
    class texture_node;
    class texture_coordinate_node;
    class texture_transform_node;
    class transform_node;
    class viewpoint_node;

    namespace vrml97_node {
        class audio_clip_node;
        class movie_texture_node;
        class time_sensor_node;
    }

    class scene;

    std::ostream & operator<<(std::ostream & out, const node & n);

    template <typename To>
    To node_cast(node * n) throw ();

    class event_listener;
    class event_emitter;

    template <typename FieldValue> class field_value_listener;
    template <typename FieldValue> class exposedfield;

    class node : boost::noncopyable {
        friend std::ostream & operator<<(std::ostream & out, const node & n);
        friend script_node * node_cast<script_node *>(node * n) throw ();
        friend appearance_node * node_cast<appearance_node *>(node * n)
            throw ();
        friend bounded_volume_node *
        node_cast<bounded_volume_node *>(node * n) throw ();
        friend child_node * node_cast<child_node *>(node * n) throw ();
        friend color_node * node_cast<color_node *>(node * n) throw ();
        friend coordinate_node * node_cast<coordinate_node *>(node * n)
            throw ();
        friend font_style_node * node_cast<font_style_node *>(node * n)
            throw ();
        friend geometry_node * node_cast<geometry_node *>(node * n) throw ();
        friend grouping_node * node_cast<grouping_node *>(node * n) throw ();
        friend light_node * node_cast<light_node *>(node * n) throw ();
        friend material_node * node_cast<material_node *>(node * n) throw ();
        friend navigation_info_node *
        node_cast<navigation_info_node *>(node * n) throw ();
        friend normal_node * node_cast<normal_node *>(node * n) throw ();
        friend pointing_device_sensor_node *
        node_cast<pointing_device_sensor_node *>(node * n) throw ();
        friend scoped_light_node * node_cast<scoped_light_node *>(node * n)
            throw ();
        friend sound_source_node * node_cast<sound_source_node *>(node * n)
            throw ();
        friend texture_node * node_cast<texture_node *>(node * n) throw ();
        friend texture_coordinate_node *
        node_cast<texture_coordinate_node *>(node * n) throw ();
        friend texture_transform_node *
        node_cast<texture_transform_node *>(node * n) throw ();
        friend transform_node * node_cast<transform_node *>(node * n) throw ();
        friend viewpoint_node * node_cast<viewpoint_node *>(node * n) throw ();

        friend class field_value_listener<sfbool>;
        friend class field_value_listener<sfcolor>;
        friend class field_value_listener<sffloat>;
        friend class field_value_listener<sfimage>;
        friend class field_value_listener<sfint32>;
        friend class field_value_listener<sfnode>;
        friend class field_value_listener<sfrotation>;
        friend class field_value_listener<sfstring>;
        friend class field_value_listener<sftime>;
        friend class field_value_listener<sfvec2f>;
        friend class field_value_listener<sfvec3f>;
        friend class field_value_listener<mfcolor>;
        friend class field_value_listener<mffloat>;
        friend class field_value_listener<mfint32>;
        friend class field_value_listener<mfnode>;
        friend class field_value_listener<mfrotation>;
        friend class field_value_listener<mfstring>;
        friend class field_value_listener<mftime>;
        friend class field_value_listener<mfvec2f>;
        friend class field_value_listener<mfvec3f>;

        friend class exposedfield<sfbool>;
        friend class exposedfield<sfcolor>;
        friend class exposedfield<sffloat>;
        friend class exposedfield<sfimage>;
        friend class exposedfield<sfint32>;
        friend class exposedfield<sfnode>;
        friend class exposedfield<sfrotation>;
        friend class exposedfield<sfstring>;
        friend class exposedfield<sftime>;
        friend class exposedfield<sfvec2f>;
        friend class exposedfield<sfvec3f>;
        friend class exposedfield<mfcolor>;
        friend class exposedfield<mffloat>;
        friend class exposedfield<mfint32>;
        friend class exposedfield<mfnode>;
        friend class exposedfield<mfrotation>;
        friend class exposedfield<mfstring>;
        friend class exposedfield<mftime>;
        friend class exposedfield<mfvec2f>;
        friend class exposedfield<mfvec3f>;

        mutable boost::recursive_mutex mutex_;
        const node_type & type_;
        boost::shared_ptr<openvrml::scope> scope_;
        openvrml::scene * scene_;
        bool modified_;

    public:
        virtual ~node() throw () = 0;

        const node_type & type() const throw ();

        const std::string & id() const throw ();
        void id(const std::string & node_id) throw (std::bad_alloc);

        const boost::shared_ptr<openvrml::scope> & scope() const throw ();

        openvrml::scene * scene() const throw ();

        std::ostream & print(std::ostream & out, size_t indent) const;

        void initialize(openvrml::scene & scene, double timestamp)
            throw (std::bad_alloc);
        const field_value & field(const std::string & id) const
            throw (unsupported_interface);
        openvrml::event_listener & event_listener(const std::string & id)
            throw (unsupported_interface);
        openvrml::event_emitter & event_emitter(const std::string & id)
            throw (unsupported_interface);
        void shutdown(double timestamp) throw ();

        virtual vrml97_node::audio_clip_node * to_audio_clip() const;
        virtual vrml97_node::movie_texture_node * to_movie_texture() const;
        virtual vrml97_node::time_sensor_node * to_time_sensor() const;

        virtual bool modified() const;
        void modified(bool value);

    protected:
        static void emit_event(openvrml::event_emitter & emitter,
                               double timestamp)
            throw (std::bad_alloc);

        node(const node_type & type,
             const boost::shared_ptr<openvrml::scope> & scope)
            throw ();

        boost::recursive_mutex & mutex() const throw ();

    private:
        virtual void do_initialize(double timestamp) throw (std::bad_alloc);
        virtual const field_value & do_field(const std::string & id) const
            throw (unsupported_interface) = 0;
        virtual openvrml::event_listener &
        do_event_listener(const std::string & id)
            throw (unsupported_interface) = 0;
        virtual openvrml::event_emitter &
        do_event_emitter(const std::string & id)
            throw (unsupported_interface) = 0;
        virtual void do_shutdown(double timestamp) throw ();

        virtual script_node * to_script() throw ();
        virtual appearance_node * to_appearance() throw ();
        virtual bounded_volume_node * to_bounded_volume() throw ();
        virtual child_node * to_child() throw ();
        virtual color_node * to_color() throw ();
        virtual coordinate_node * to_coordinate() throw ();
        virtual font_style_node * to_font_style() throw () ;
        virtual geometry_node * to_geometry() throw ();
        virtual grouping_node * to_grouping() throw ();
        virtual light_node * to_light() throw ();
        virtual material_node * to_material() throw ();
        virtual navigation_info_node * to_navigation_info() throw ();
        virtual normal_node * to_normal() throw ();
        virtual pointing_device_sensor_node * to_pointing_device_sensor()
            throw ();
        virtual scoped_light_node * to_scoped_light() throw ();
        virtual sound_source_node * to_sound_source() throw ();
        virtual texture_node * to_texture() throw ();
        virtual texture_coordinate_node * to_texture_coordinate() throw ();
        virtual texture_transform_node * to_texture_transform() throw ();
        virtual transform_node * to_transform() throw ();
        virtual viewpoint_node * to_viewpoint() throw ();
    };

    inline const boost::shared_ptr<scope> & node::scope() const throw ()
    {
        return this->scope_;
    }

    inline openvrml::scene * node::scene() const throw ()
    {
        return this->scene_;
    }

    inline boost::recursive_mutex & node::mutex() const throw ()
    {
        return this->mutex_;
    }


    bool add_route(node & from, const std::string & eventout,
                   node & to, const std::string & eventin)
        throw (std::bad_alloc, unsupported_interface,
               field_value_type_mismatch);

    bool delete_route(node & from, const std::string & eventout,
                      node & to, const std::string & eventin)
        throw (unsupported_interface);

    template <>
    inline script_node * node_cast<script_node *>(node * n) throw ()
    {
        return n
            ? n->to_script()
            : 0;
    }

    template <>
    inline appearance_node * node_cast<appearance_node *>(node * n) throw ()
    {
        return n
            ? n->to_appearance()
            : 0;
    }

    template <>
    inline bounded_volume_node *
    node_cast<bounded_volume_node *>(node * n) throw ()
    {
        return n
            ? n->to_bounded_volume()
            : 0;
    }

    template <>
    inline child_node * node_cast<child_node *>(node * n) throw ()
    {
        return n
            ? n->to_child()
            : 0;
    }

    template <>
    inline color_node * node_cast<color_node *>(node * n) throw ()
    {
        return n
            ? n->to_color()
            : 0;
    }

    template <>
    inline coordinate_node * node_cast<coordinate_node *>(node * n) throw ()
    {
        return n
            ? n->to_coordinate()
            : 0;
    }

    template <>
    inline font_style_node * node_cast<font_style_node *>(node * n) throw ()
    {
        return n
            ? n->to_font_style()
            : 0;
    }

    template <>
    inline geometry_node * node_cast<geometry_node *>(node * n) throw ()
    {
        return n
            ? n->to_geometry()
            : 0;
    }

    template <>
    inline grouping_node * node_cast<grouping_node *>(node * n) throw ()
    {
        return n
            ? n->to_grouping()
            : 0;
    }

    template <>
    inline light_node * node_cast<light_node *>(node * n) throw ()
    {
        return n
            ? n->to_light()
            : 0;
    }

    template <>
    inline material_node * node_cast<material_node *>(node * n) throw ()
    {
        return n
            ? n->to_material()
            : 0;
    }

    template <>
    inline navigation_info_node * node_cast<navigation_info_node *>(node * n)
        throw ()
    {
        return n
            ? n->to_navigation_info()
            : 0;
    }

    template <>
    inline normal_node * node_cast<normal_node *>(node * n) throw ()
    {
        return n
            ? n->to_normal()
            : 0;
    }

    template <>
    inline pointing_device_sensor_node *
    node_cast<pointing_device_sensor_node *>(node * n) throw ()
    {
        return n
            ? n->to_pointing_device_sensor()
            : 0;
    }

    template <>
    inline scoped_light_node * node_cast<scoped_light_node *>(node * n)
        throw ()
    {
        return n
            ? n->to_scoped_light()
            : 0;
    }

    template <>
    inline sound_source_node * node_cast<sound_source_node *>(node * n)
        throw ()
    {
        return n
            ? n->to_sound_source()
            : 0;
    }

    template <>
    inline texture_node * node_cast<texture_node *>(node * n) throw ()
    {
        return n
            ? n->to_texture()
            : 0;
    }

    template <>
    inline texture_coordinate_node *
    node_cast<texture_coordinate_node *>(node * n) throw ()
    {
        return n
            ? n->to_texture_coordinate()
            : 0;
    }

    template <>
    inline texture_transform_node *
    node_cast<texture_transform_node *>(node * n) throw ()
    {
        return n
            ? n->to_texture_transform()
            : 0;
    }

    template <>
    inline transform_node * node_cast<transform_node *>(node * n) throw ()
    {
        return n
            ? n->to_transform()
            : 0;
    }

    template <>
    inline viewpoint_node * node_cast<viewpoint_node *>(node * n) throw ()
    {
        return n
            ? n->to_viewpoint()
            : 0;
    }


    class appearance_node : public virtual node {
    public:
        virtual ~appearance_node() throw () = 0;

        void render_appearance(viewer & v, rendering_context context);

        virtual const node_ptr & material() const throw () = 0;
        virtual const node_ptr & texture() const throw () = 0;
        virtual const node_ptr & texture_transform() const throw () = 0;

    protected:
        appearance_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope)
            throw ();

    private:
        virtual appearance_node * to_appearance() throw ();

        virtual void do_render_appearance(viewer & v,
                                          rendering_context context);
    };


    class bounded_volume_node : public virtual node {
        mutable bool bounding_volume_dirty_;

    public:
        virtual ~bounded_volume_node() throw ();

        const openvrml::bounding_volume & bounding_volume() const;

    protected:
        bounded_volume_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope)
            throw ();

        void bounding_volume_dirty(bool value);
        bool bounding_volume_dirty() const;

        virtual const openvrml::bounding_volume & do_bounding_volume() const;

    private:
        virtual bounded_volume_node * to_bounded_volume() throw ();
    };


    class child_node : public virtual bounded_volume_node {
    public:
        virtual ~child_node() throw () = 0;

        void relocate() throw (std::bad_alloc);
        void render_child(viewer & v, rendering_context context);

    protected:
        child_node(const node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope)
            throw ();

    private:
        virtual child_node * to_child() throw ();

        virtual void do_relocate() throw (std::bad_alloc);
        virtual void do_render_child(viewer & v,
                                     rendering_context context);
    };


    class color_node : public virtual node {
    public:
        virtual ~color_node() throw () = 0;

        virtual const std::vector<openvrml::color> & color() const
            throw () = 0;

    protected:
        color_node(const node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope)
            throw ();

    private:
        virtual color_node * to_color() throw ();
    };


    class coordinate_node : public virtual node {
    public:
        virtual ~coordinate_node() throw () = 0;

        virtual const std::vector<vec3f> & point() const throw () = 0;

    protected:
        coordinate_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope)
            throw ();

    private:
        virtual coordinate_node * to_coordinate() throw ();
    };


    class font_style_node : public virtual node {
    public:
        virtual ~font_style_node() throw () = 0;

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
        font_style_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope)
            throw ();

    private:
        virtual font_style_node * to_font_style() throw ();
    };


    class geometry_node : public virtual bounded_volume_node {
        viewer::object_t geometry_reference;

    public:
        virtual ~geometry_node() throw () = 0;

        viewer::object_t render_geometry(viewer & v,
                                         rendering_context context);
        bool emissive() const throw ();
        virtual const color_node * color() const throw ();

    protected:
        geometry_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope)
            throw ();

    private:
        virtual viewer::object_t
        do_render_geometry(viewer & v, rendering_context context);
        virtual bool do_emissive() const throw ();

        virtual geometry_node * to_geometry() throw ();
    };


    class grouping_node : public virtual child_node {
    public:
        virtual ~grouping_node() throw () = 0;

        const std::vector<node_ptr> & children() const throw ();
        void activate_pointing_device_sensors(double timestamp,
                                              bool over,
                                              bool active,
                                              const double (&p)[3]);

    protected:
        grouping_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope)
            throw ();

    private:
        virtual grouping_node * to_grouping() throw ();
        virtual const std::vector<node_ptr> & do_children() const throw () = 0;
    };


    class light_node : public virtual child_node {
    public:
        virtual ~light_node() throw () = 0;

    protected:
        light_node(const node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope)
            throw ();

    private:
        virtual light_node * to_light() throw ();
    };


    class material_node : public virtual node {
    public:
        virtual ~material_node() throw () = 0;

        virtual float ambient_intensity() const throw () = 0;
        virtual const color & diffuse_color() const throw () = 0;
        virtual const color & emissive_color() const throw () = 0;
        virtual float shininess() const throw () = 0;
        virtual const color & specular_color() const throw () = 0;
        virtual float transparency() const throw () = 0;

    protected:
        material_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope)
            throw ();

    private:
        virtual material_node * to_material() throw ();
    };


    class navigation_info_node : public virtual child_node {
    public:
        virtual ~navigation_info_node() throw () = 0;

        virtual const std::vector<float> & avatar_size() const throw () = 0;
        virtual bool headlight() const throw () = 0;
        virtual float speed() const throw () = 0;
        virtual const std::vector<std::string> & type() const throw () = 0;
        virtual float visibility_limit() const throw () = 0;

    protected:
        navigation_info_node(const node_type & type,
                             const boost::shared_ptr<openvrml::scope> & scope)
            throw ();

    private:
        virtual navigation_info_node * to_navigation_info() throw ();
    };


    class normal_node : public virtual node {
    public:
        virtual ~normal_node() throw () = 0;

        virtual const std::vector<vec3f> & vector() const throw () = 0;

    protected:
        normal_node(const node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope) throw ();

    private:
        virtual normal_node * to_normal() throw ();
    };


    class pointing_device_sensor_node : public virtual child_node {
    public:
        virtual ~pointing_device_sensor_node() throw () = 0;

        void activate(double timestamp, bool over, bool active,
                      const double (&point)[3]);

    protected:
        pointing_device_sensor_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);

    private:
        virtual void do_activate(double timestamp, bool over, bool active,
                                 const double (&point)[3]) = 0;
        virtual pointing_device_sensor_node * to_pointing_device_sensor()
            throw ();
    };


    class scoped_light_node : public virtual light_node {
    public:
        virtual ~scoped_light_node() throw () = 0;

        void render_scoped_light(viewer & v);

    protected:
        scoped_light_node(const node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope)
            throw ();

    private:
        virtual scoped_light_node * to_scoped_light() throw ();
        virtual void do_render_scoped_light(viewer & v) = 0;
    };


    class sound_source_node : public virtual node {
    public:
        virtual ~sound_source_node() throw () = 0;

    protected:
        sound_source_node(const node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope)
            throw ();

    private:
        virtual sound_source_node * to_sound_source() throw ();
    };


    class texture_node : public virtual node {
        viewer::texture_object_t texture_reference;

    public:
        virtual ~texture_node() throw () = 0;

        viewer::texture_object_t render_texture(viewer & v);

        virtual const openvrml::image & image() const throw () = 0;
        virtual bool repeat_s() const throw () = 0;
        virtual bool repeat_t() const throw () = 0;

    protected:
        texture_node(const node_type & type,
                     const boost::shared_ptr<openvrml::scope> & scope)
            throw ();

    private:
        virtual texture_node * to_texture() throw ();

        virtual viewer::texture_object_t do_render_texture(viewer & v);
    };


    class texture_coordinate_node : public virtual node {
    public:
        virtual ~texture_coordinate_node() throw () = 0;

        virtual const std::vector<vec2f> & point() const throw () = 0;

    protected:
        texture_coordinate_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope)
            throw ();

    private:
        virtual texture_coordinate_node * to_texture_coordinate() throw ();
    };


    class texture_transform_node : public virtual node {
    public:
        virtual ~texture_transform_node() throw () = 0;

        void render_texture_transform(viewer & v);

    protected:
        texture_transform_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope)
            throw ();

    private:
        virtual texture_transform_node * to_texture_transform() throw ();

        virtual void do_render_texture_transform(viewer & v);
    };


    class transform_node : public virtual grouping_node {
    public:
        virtual ~transform_node() throw () = 0;

        virtual const mat4f & transform() const throw () = 0;

    protected:
        transform_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope)
            throw ();

    private:
        virtual transform_node * to_transform() throw ();
    };


    class viewpoint_node : public virtual child_node {
    public:
        virtual ~viewpoint_node() throw () = 0;

        virtual const mat4f & transformation() const throw () = 0;
        virtual const mat4f & user_view_transform() const throw () = 0;
        virtual void user_view_transform(const mat4f & transform)
            throw () = 0;
        virtual const std::string & description() const throw () = 0;
        virtual float field_of_view() const throw () = 0;

    protected:
        viewpoint_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope)
            throw ();

    private:
        virtual viewpoint_node * to_viewpoint() throw ();
    };


    class node_traverser : boost::noncopyable {
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
        bool halted() throw ();

    private:
        virtual void on_entering(node & n);
        virtual void on_leaving(node & n);

        void do_traversal(node & n);
    };
}

# endif
