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
#   include <boost/thread/mutex.hpp>
#   include <boost/thread/recursive_mutex.hpp>
#   include <openvrml/field_value.h>
#   include <openvrml/viewer.h>
#   include <openvrml/rendering_context.h>
#   include <openvrml/scope.h>

namespace openvrml {

    class OPENVRML_API node_interface {
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

    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           node_interface::type_id type);
    OPENVRML_API std::istream & operator>>(std::istream & in,
                                           node_interface::type_id & type);

    OPENVRML_API bool operator==(const node_interface & lhs,
                                 const node_interface & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API bool operator!=(const node_interface & lhs,
                                 const node_interface & rhs)
        OPENVRML_NOTHROW;

    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           const node_interface & interface);
    OPENVRML_API std::istream & operator>>(std::istream & in,
                                           node_interface & interface);

    class node_type;

    class OPENVRML_API unsupported_interface : public std::logic_error {
    public:
        explicit unsupported_interface(const node_interface & interface)
            OPENVRML_NOTHROW;
        unsupported_interface(const node_type & type,
                              const std::string & interface_id)
            OPENVRML_NOTHROW;
        unsupported_interface(const node_type & type,
                              node_interface::type_id interface_type,
                              const std::string & interface_id)
            OPENVRML_NOTHROW;
        virtual ~unsupported_interface() throw ();
    };


    struct OPENVRML_API node_interface_matches_eventin :
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


    struct OPENVRML_API node_interface_matches_eventout :
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


    struct OPENVRML_API node_interface_matches_exposedfield :
        std::binary_function<node_interface, std::string, bool> {
        result_type
        operator()(const first_argument_type & interface,
                   const second_argument_type & exposedfield_id) const
        {
            return interface.type == node_interface::exposedfield_id
                && interface.id == exposedfield_id;
        }
    };


    struct OPENVRML_API node_interface_matches_field :
        std::binary_function<node_interface, std::string, bool> {
        result_type operator()(const first_argument_type & interface,
                               const second_argument_type & field_id) const
        {
            return (interface.type == node_interface::field_id
                    || interface.type == node_interface::exposedfield_id)
                && interface.id == field_id;
        }
    };


    struct OPENVRML_API node_interface_compare :
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

    OPENVRML_API const node_interface_set::const_iterator
    find_interface(const node_interface_set & interfaces,
                   const std::string & id)
        OPENVRML_NOTHROW;


    class OPENVRML_API node_metatype_id {
        friend OPENVRML_API bool operator==(const node_metatype_id & lhs,
                                            const node_metatype_id & rhs)
            OPENVRML_NOTHROW;

        std::string id_;

    public:
        node_metatype_id(const char * id)
            OPENVRML_THROW2(std::invalid_argument, std::bad_alloc);
        node_metatype_id(const std::string & id)
            OPENVRML_THROW2(std::invalid_argument, std::bad_alloc);
        operator std::string() const;
    };

    OPENVRML_API bool operator==(const node_metatype_id & lhs,
                                 const node_metatype_id & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API bool operator!=(const node_metatype_id & lhs,
                                 const node_metatype_id & rhs)
        OPENVRML_NOTHROW;


    class browser;
    class viewpoint_node;
    class node_type;
    class proto_node;

    class OPENVRML_API node_metatype : boost::noncopyable {
        const node_metatype_id id_;
        openvrml::browser * const browser_;

    public:
        virtual ~node_metatype() OPENVRML_NOTHROW = 0;

        const node_metatype_id & id() const OPENVRML_NOTHROW;
        openvrml::browser & browser() const OPENVRML_NOTHROW;
        void initialize(viewpoint_node * initial_viewpoint, double time)
            OPENVRML_NOTHROW;
        void render(viewer & v) const OPENVRML_NOTHROW;
        const boost::shared_ptr<node_type>
        create_type(const std::string & id,
                    const node_interface_set & interfaces)
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
        void shutdown(double time) OPENVRML_NOTHROW;

    protected:
        explicit node_metatype(const node_metatype_id & id, openvrml::browser & b)
            OPENVRML_NOTHROW;

    private:
        virtual void do_initialize(viewpoint_node * initial_viewpoint,
                                   double time)
            OPENVRML_NOTHROW;
        virtual void do_render(viewer & v) const OPENVRML_NOTHROW;
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc) = 0;
        virtual void do_shutdown(double time) OPENVRML_NOTHROW;
    };


    typedef std::map<std::string, boost::shared_ptr<field_value> >
        initial_value_map;


    class OPENVRML_API node_type : boost::noncopyable {
        const node_metatype & metatype_;
        const std::string id_;

    public:
        virtual ~node_type() OPENVRML_NOTHROW = 0;

        const node_metatype & metatype() const OPENVRML_NOTHROW;
        const std::string & id() const OPENVRML_NOTHROW;
        const node_interface_set & interfaces() const OPENVRML_NOTHROW;
        const boost::intrusive_ptr<node>
        create_node(const boost::shared_ptr<scope> & scope,
                    const initial_value_map & initial_values =
                    initial_value_map()) const
            OPENVRML_THROW3(unsupported_interface, std::bad_cast,
                            std::bad_alloc);

    protected:
        node_type(const node_metatype & c, const std::string & id)
            OPENVRML_THROW1(std::bad_alloc);

    private:
        virtual const node_interface_set & do_interfaces() const
            OPENVRML_NOTHROW = 0;
        virtual const boost::intrusive_ptr<node>
        do_create_node(const boost::shared_ptr<scope> & scope,
                       const initial_value_map & initial_values) const
            OPENVRML_THROW3(unsupported_interface, std::bad_cast,
                            std::bad_alloc) = 0;
    };

    OPENVRML_API bool operator==(const node_type & lhs, const node_type & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API bool operator!=(const node_type & lhs, const node_type & rhs)
        OPENVRML_NOTHROW;


    class OPENVRML_API field_value_type_mismatch : public std::logic_error {
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
    class color_rgba_node;
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
    class time_dependent_node;
    class transform_node;
    class viewpoint_node;
    class scene;

    OPENVRML_API std::ostream & operator<<(std::ostream & out, const node & n);

    template <typename To>
    To node_cast(node * n) OPENVRML_NOTHROW;

    class event_listener;
    class event_emitter;

    template <typename FieldValue> class field_value_listener;
    template <typename FieldValue> class field_value_emitter;
    template <typename FieldValue> class exposedfield;

    class OPENVRML_API node : boost::noncopyable {
        friend class proto_node;
        friend class externproto_node;

        friend OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                                      const node & n);

        friend script_node * node_cast<script_node *>(node * n)
            OPENVRML_NOTHROW;
        friend appearance_node * node_cast<appearance_node *>(node * n)
            OPENVRML_NOTHROW;
        friend bounded_volume_node *
        node_cast<bounded_volume_node *>(node * n) OPENVRML_NOTHROW;
        friend child_node * node_cast<child_node *>(node * n) OPENVRML_NOTHROW;
        friend color_node * node_cast<color_node *>(node * n) OPENVRML_NOTHROW;
        friend color_rgba_node * node_cast<color_rgba_node *>(node * n)
            OPENVRML_NOTHROW;
        friend coordinate_node * node_cast<coordinate_node *>(node * n)
            OPENVRML_NOTHROW;
        friend font_style_node * node_cast<font_style_node *>(node * n)
            OPENVRML_NOTHROW;
        friend geometry_node * node_cast<geometry_node *>(node * n)
            OPENVRML_NOTHROW;
        friend grouping_node * node_cast<grouping_node *>(node * n)
            OPENVRML_NOTHROW;
        friend light_node * node_cast<light_node *>(node * n)
            OPENVRML_NOTHROW;
        friend material_node * node_cast<material_node *>(node * n)
            OPENVRML_NOTHROW;
        friend navigation_info_node *
        node_cast<navigation_info_node *>(node * n) OPENVRML_NOTHROW;
        friend normal_node * node_cast<normal_node *>(node * n)
            OPENVRML_NOTHROW;
        friend pointing_device_sensor_node *
        node_cast<pointing_device_sensor_node *>(node * n) OPENVRML_NOTHROW;
        friend scoped_light_node * node_cast<scoped_light_node *>(node * n)
            OPENVRML_NOTHROW;
        friend sound_source_node * node_cast<sound_source_node *>(node * n)
            OPENVRML_NOTHROW;
        friend texture_node * node_cast<texture_node *>(node * n)
            OPENVRML_NOTHROW;
        friend texture_coordinate_node *
        node_cast<texture_coordinate_node *>(node * n) OPENVRML_NOTHROW;
        friend texture_transform_node *
        node_cast<texture_transform_node *>(node * n) OPENVRML_NOTHROW;
        friend time_dependent_node * node_cast<time_dependent_node *>(node * n)
            OPENVRML_NOTHROW;
        friend transform_node * node_cast<transform_node *>(node * n)
            OPENVRML_NOTHROW;
        friend viewpoint_node * node_cast<viewpoint_node *>(node * n)
            OPENVRML_NOTHROW;

        friend class field_value_listener<sfbool>;
        friend class field_value_listener<sfcolor>;
        friend class field_value_listener<sfcolorrgba>;
        friend class field_value_listener<sffloat>;
        friend class field_value_listener<sfdouble>;
        friend class field_value_listener<sfimage>;
        friend class field_value_listener<sfint32>;
        friend class field_value_listener<sfnode>;
        friend class field_value_listener<sfrotation>;
        friend class field_value_listener<sfstring>;
        friend class field_value_listener<sftime>;
        friend class field_value_listener<sfvec2f>;
        friend class field_value_listener<sfvec2d>;
        friend class field_value_listener<sfvec3f>;
        friend class field_value_listener<sfvec3d>;
        friend class field_value_listener<mfbool>;
        friend class field_value_listener<mfcolor>;
        friend class field_value_listener<mfcolorrgba>;
        friend class field_value_listener<mffloat>;
        friend class field_value_listener<mfdouble>;
        friend class field_value_listener<mfimage>;
        friend class field_value_listener<mfint32>;
        friend class field_value_listener<mfnode>;
        friend class field_value_listener<mfrotation>;
        friend class field_value_listener<mfstring>;
        friend class field_value_listener<mftime>;
        friend class field_value_listener<mfvec2f>;
        friend class field_value_listener<mfvec2d>;
        friend class field_value_listener<mfvec3f>;
        friend class field_value_listener<mfvec3d>;

        friend class exposedfield<sfbool>;
        friend class exposedfield<sfcolor>;
        friend class exposedfield<sfcolorrgba>;
        friend class exposedfield<sffloat>;
        friend class exposedfield<sfdouble>;
        friend class exposedfield<sfimage>;
        friend class exposedfield<sfint32>;
        friend class exposedfield<sfnode>;
        friend class exposedfield<sfrotation>;
        friend class exposedfield<sfstring>;
        friend class exposedfield<sftime>;
        friend class exposedfield<sfvec2f>;
        friend class exposedfield<sfvec2d>;
        friend class exposedfield<sfvec3f>;
        friend class exposedfield<sfvec3d>;
        friend class exposedfield<mfbool>;
        friend class exposedfield<mfcolor>;
        friend class exposedfield<mfcolorrgba>;
        friend class exposedfield<mffloat>;
        friend class exposedfield<mfdouble>;
        friend class exposedfield<mfimage>;
        friend class exposedfield<mfint32>;
        friend class exposedfield<mfnode>;
        friend class exposedfield<mfrotation>;
        friend class exposedfield<mfstring>;
        friend class exposedfield<mftime>;
        friend class exposedfield<mfvec2f>;
        friend class exposedfield<mfvec2d>;
        friend class exposedfield<mfvec3f>;
        friend class exposedfield<mfvec3d>;

        mutable boost::mutex ref_count_mutex_;
        mutable size_t ref_count_;

        mutable boost::recursive_mutex mutex_;
        const node_type & type_;
        boost::shared_ptr<openvrml::scope> scope_;
        openvrml::scene * scene_;
        bool modified_;

    public:
        static const boost::intrusive_ptr<node> self_tag;

        virtual ~node() OPENVRML_NOTHROW = 0;

        void add_ref() const OPENVRML_NOTHROW;
        void remove_ref() const OPENVRML_NOTHROW;
        void release() const OPENVRML_NOTHROW;
        size_t use_count() const OPENVRML_NOTHROW;

        const node_type & type() const OPENVRML_NOTHROW;

        const std::string & id() const OPENVRML_NOTHROW;
        void id(const std::string & node_id) OPENVRML_THROW1(std::bad_alloc);

        const openvrml::scope & scope() const OPENVRML_NOTHROW;

        openvrml::scene * scene() const OPENVRML_NOTHROW;

        std::ostream & print(std::ostream & out, size_t indent) const;

        void initialize(openvrml::scene & scene, double timestamp)
            OPENVRML_THROW1(std::bad_alloc);

        std::auto_ptr<field_value> field(const std::string & id) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);

        template <typename FieldValue>
        const FieldValue field(const std::string & id) const
            OPENVRML_THROW2(unsupported_interface, std::bad_cast);

        openvrml::event_listener & event_listener(const std::string & id)
            OPENVRML_THROW1(unsupported_interface);

        template <typename FieldValue>
        field_value_listener<FieldValue> &
        event_listener(const std::string & id)
            OPENVRML_THROW2(unsupported_interface, std::bad_cast);

        openvrml::event_emitter & event_emitter(const std::string & id)
            OPENVRML_THROW1(unsupported_interface);

        template <typename FieldValue>
        field_value_emitter<FieldValue> & event_emitter(const std::string & id)
            OPENVRML_THROW2(unsupported_interface, std::bad_cast);
        void shutdown(double timestamp) OPENVRML_NOTHROW;

        virtual bool modified() const;
        void modified(bool value);

    protected:
        static void emit_event(openvrml::event_emitter & emitter,
                               double timestamp)
            OPENVRML_THROW1(std::bad_alloc);

        node(const node_type & type,
             const boost::shared_ptr<openvrml::scope> & scope)
            OPENVRML_NOTHROW;

        boost::recursive_mutex & mutex() const OPENVRML_NOTHROW;

    private:
        virtual void do_initialize(double timestamp)
            OPENVRML_THROW1(std::bad_alloc);
        virtual const field_value & do_field(const std::string & id) const
            OPENVRML_THROW1(unsupported_interface) = 0;
        virtual openvrml::event_listener &
        do_event_listener(const std::string & id)
            OPENVRML_THROW1(unsupported_interface) = 0;
        virtual openvrml::event_emitter &
        do_event_emitter(const std::string & id)
            OPENVRML_THROW1(unsupported_interface) = 0;
        virtual void do_shutdown(double timestamp) OPENVRML_NOTHROW;

        virtual script_node * to_script() OPENVRML_NOTHROW;
        virtual appearance_node * to_appearance() OPENVRML_NOTHROW;
        virtual bounded_volume_node * to_bounded_volume() OPENVRML_NOTHROW;
        virtual child_node * to_child() OPENVRML_NOTHROW;
        virtual color_node * to_color() OPENVRML_NOTHROW;
        virtual color_rgba_node * to_color_rgba() OPENVRML_NOTHROW;
        virtual coordinate_node * to_coordinate() OPENVRML_NOTHROW;
        virtual font_style_node * to_font_style() OPENVRML_NOTHROW ;
        virtual geometry_node * to_geometry() OPENVRML_NOTHROW;
        virtual grouping_node * to_grouping() OPENVRML_NOTHROW;
        virtual light_node * to_light() OPENVRML_NOTHROW;
        virtual material_node * to_material() OPENVRML_NOTHROW;
        virtual navigation_info_node * to_navigation_info() OPENVRML_NOTHROW;
        virtual normal_node * to_normal() OPENVRML_NOTHROW;
        virtual pointing_device_sensor_node * to_pointing_device_sensor()
            OPENVRML_NOTHROW;
        virtual scoped_light_node * to_scoped_light() OPENVRML_NOTHROW;
        virtual sound_source_node * to_sound_source() OPENVRML_NOTHROW;
        virtual texture_node * to_texture() OPENVRML_NOTHROW;
        virtual texture_coordinate_node * to_texture_coordinate()
            OPENVRML_NOTHROW;
        virtual texture_transform_node * to_texture_transform()
            OPENVRML_NOTHROW;
        virtual time_dependent_node * to_time_dependent() OPENVRML_NOTHROW;
        virtual transform_node * to_transform() OPENVRML_NOTHROW;
        virtual viewpoint_node * to_viewpoint() OPENVRML_NOTHROW;
    };

    inline void node::add_ref() const OPENVRML_NOTHROW
    {
        boost::mutex::scoped_lock lock(this->ref_count_mutex_);
        ++this->ref_count_;
    }

    inline void intrusive_ptr_add_ref(const node * n) OPENVRML_NOTHROW
    {
        assert(n);
        n->add_ref();
    }

    inline void node::remove_ref() const OPENVRML_NOTHROW
    {
        boost::mutex::scoped_lock lock(this->ref_count_mutex_);
        assert(this->ref_count_ > 0);
        --this->ref_count_;
    }

    inline void node::release() const OPENVRML_NOTHROW
    {
        bool delete_me;
        {
            boost::mutex::scoped_lock lock(this->ref_count_mutex_);
            delete_me = (--this->ref_count_ == 0);
        }
        if (delete_me) { delete this; }
    }

    inline void intrusive_ptr_release(const node * n) OPENVRML_NOTHROW
    {
        assert(n);
        n->release();
    }

    inline const scope & node::scope() const OPENVRML_NOTHROW
    {
        assert(this->scope_);
        return *this->scope_;
    }

    inline openvrml::scene * node::scene() const OPENVRML_NOTHROW
    {
        return this->scene_;
    }

    inline boost::recursive_mutex & node::mutex() const OPENVRML_NOTHROW
    {
        return this->mutex_;
    }

    template <typename FieldValue>
    const FieldValue
    node::field(const std::string & id) const
        OPENVRML_THROW2(unsupported_interface, std::bad_cast)
    {
        boost::function_requires<FieldValueConcept<FieldValue> >();

        boost::recursive_mutex::scoped_lock lock(this->mutex_);
        return dynamic_cast<const FieldValue &>(this->do_field(id));
    }

    template <typename FieldValue>
    field_value_listener<FieldValue> &
    node::event_listener(const std::string & id)
        OPENVRML_THROW2(unsupported_interface, std::bad_cast)
    {
        //
        // No need to lock here; the set of event listeners for a node instance
        // cannot change.
        //
        return dynamic_cast<field_value_listener<FieldValue> &>(
            this->do_event_listener(id));
    }

    template <typename FieldValue>
    field_value_emitter<FieldValue> &
    node::event_emitter(const std::string & id)
        OPENVRML_THROW2(unsupported_interface, std::bad_cast)
    {
        //
        // No need to lock here; the set of event emitters for a node instance
        // cannot change.
        //
        return dynamic_cast<field_value_emitter<FieldValue> &>(
            this->do_event_emitter(id));
    }

    OPENVRML_API bool add_route(node & from, const std::string & eventout,
                                node & to, const std::string & eventin)
        OPENVRML_THROW3(std::bad_alloc, unsupported_interface,
                        field_value_type_mismatch);

    OPENVRML_API bool delete_route(node & from, const std::string & eventout,
                                   node & to, const std::string & eventin)
        OPENVRML_THROW1(unsupported_interface);

    template <>
    inline script_node * node_cast<script_node *>(node * n) OPENVRML_NOTHROW
    {
        return n
            ? n->to_script()
            : 0;
    }

    template <>
    inline appearance_node * node_cast<appearance_node *>(node * n)
        OPENVRML_NOTHROW
    {
        return n
            ? n->to_appearance()
            : 0;
    }

    template <>
    inline bounded_volume_node * node_cast<bounded_volume_node *>(node * n)
        OPENVRML_NOTHROW
    {
        return n
            ? n->to_bounded_volume()
            : 0;
    }

    template <>
    inline child_node * node_cast<child_node *>(node * n) OPENVRML_NOTHROW
    {
        return n
            ? n->to_child()
            : 0;
    }

    template <>
    inline color_node * node_cast<color_node *>(node * n) OPENVRML_NOTHROW
    {
        return n
            ? n->to_color()
            : 0;
    }

    template <>
    inline color_rgba_node * node_cast<color_rgba_node *>(node * n)
        OPENVRML_NOTHROW
    {
        return n
            ? n->to_color_rgba()
            : 0;
    }

    template <>
    inline coordinate_node * node_cast<coordinate_node *>(node * n)
        OPENVRML_NOTHROW
    {
        return n
            ? n->to_coordinate()
            : 0;
    }

    template <>
    inline font_style_node * node_cast<font_style_node *>(node * n)
        OPENVRML_NOTHROW
    {
        return n
            ? n->to_font_style()
            : 0;
    }

    template <>
    inline geometry_node * node_cast<geometry_node *>(node * n)
        OPENVRML_NOTHROW
    {
        return n
            ? n->to_geometry()
            : 0;
    }

    template <>
    inline grouping_node * node_cast<grouping_node *>(node * n)
        OPENVRML_NOTHROW
    {
        return n
            ? n->to_grouping()
            : 0;
    }

    template <>
    inline light_node * node_cast<light_node *>(node * n) OPENVRML_NOTHROW
    {
        return n
            ? n->to_light()
            : 0;
    }

    template <>
    inline material_node * node_cast<material_node *>(node * n)
        OPENVRML_NOTHROW
    {
        return n
            ? n->to_material()
            : 0;
    }

    template <>
    inline navigation_info_node * node_cast<navigation_info_node *>(node * n)
        OPENVRML_NOTHROW
    {
        return n
            ? n->to_navigation_info()
            : 0;
    }

    template <>
    inline normal_node * node_cast<normal_node *>(node * n) OPENVRML_NOTHROW
    {
        return n
            ? n->to_normal()
            : 0;
    }

    template <>
    inline pointing_device_sensor_node *
    node_cast<pointing_device_sensor_node *>(node * n) OPENVRML_NOTHROW
    {
        return n
            ? n->to_pointing_device_sensor()
            : 0;
    }

    template <>
    inline scoped_light_node * node_cast<scoped_light_node *>(node * n)
        OPENVRML_NOTHROW
    {
        return n
            ? n->to_scoped_light()
            : 0;
    }

    template <>
    inline sound_source_node * node_cast<sound_source_node *>(node * n)
        OPENVRML_NOTHROW
    {
        return n
            ? n->to_sound_source()
            : 0;
    }

    template <>
    inline texture_node * node_cast<texture_node *>(node * n) OPENVRML_NOTHROW
    {
        return n
            ? n->to_texture()
            : 0;
    }

    template <>
    inline texture_coordinate_node *
    node_cast<texture_coordinate_node *>(node * n) OPENVRML_NOTHROW
    {
        return n
            ? n->to_texture_coordinate()
            : 0;
    }

    template <>
    inline texture_transform_node *
    node_cast<texture_transform_node *>(node * n) OPENVRML_NOTHROW
    {
        return n
            ? n->to_texture_transform()
            : 0;
    }

    template <>
    inline time_dependent_node * node_cast<time_dependent_node *>(node * n)
        OPENVRML_NOTHROW
    {
        return n
            ? n->to_time_dependent()
            : 0;
    }

    template <>
    inline transform_node * node_cast<transform_node *>(node * n)
        OPENVRML_NOTHROW
    {
        return n
            ? n->to_transform()
            : 0;
    }

    template <>
    inline viewpoint_node * node_cast<viewpoint_node *>(node * n)
        OPENVRML_NOTHROW
    {
        return n
            ? n->to_viewpoint()
            : 0;
    }


    class OPENVRML_API appearance_node : public virtual node {
    public:
        virtual ~appearance_node() OPENVRML_NOTHROW = 0;

        void render_appearance(viewer & v, rendering_context context);

        virtual const boost::intrusive_ptr<node> & material() const
            OPENVRML_NOTHROW = 0;
        virtual const boost::intrusive_ptr<node> & texture() const
            OPENVRML_NOTHROW = 0;
        virtual const boost::intrusive_ptr<node> & texture_transform() const
            OPENVRML_NOTHROW = 0;

    protected:
        appearance_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope)
            OPENVRML_NOTHROW;

    private:
        virtual appearance_node * to_appearance() OPENVRML_NOTHROW;

        virtual void do_render_appearance(viewer & v,
                                          rendering_context context);
    };


    class OPENVRML_API bounded_volume_node : public virtual node {
        mutable bool bounding_volume_dirty_;

    public:
        virtual ~bounded_volume_node() OPENVRML_NOTHROW;

        const openvrml::bounding_volume & bounding_volume() const;

    protected:
        bounded_volume_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope)
            OPENVRML_NOTHROW;

        void bounding_volume_dirty(bool value);
        bool bounding_volume_dirty() const;

        virtual const openvrml::bounding_volume & do_bounding_volume() const;

    private:
        virtual bounded_volume_node * to_bounded_volume() OPENVRML_NOTHROW;
    };


    class OPENVRML_API child_node : public virtual bounded_volume_node {
    public:
        virtual ~child_node() OPENVRML_NOTHROW = 0;

        void relocate() OPENVRML_THROW1(std::bad_alloc);
        void render_child(viewer & v, rendering_context context);

    protected:
        child_node(const node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope)
            OPENVRML_NOTHROW;

    private:
        virtual child_node * to_child() OPENVRML_NOTHROW;

        virtual void do_relocate() OPENVRML_THROW1(std::bad_alloc);
        virtual void do_render_child(viewer & v,
                                     rendering_context context);
    };


    class OPENVRML_API color_node : public virtual node {
    public:
        virtual ~color_node() OPENVRML_NOTHROW = 0;

        virtual const std::vector<openvrml::color> & color() const
            OPENVRML_NOTHROW = 0;

    protected:
        color_node(const node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope)
            OPENVRML_NOTHROW;

    private:
        virtual color_node * to_color() OPENVRML_NOTHROW;
    };


    class OPENVRML_API color_rgba_node : public virtual node {
    public:
        virtual ~color_rgba_node() OPENVRML_NOTHROW = 0;

        virtual const std::vector<openvrml::color_rgba> & color_rgba() const
            OPENVRML_NOTHROW = 0;

    protected:
        color_rgba_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope)
            OPENVRML_NOTHROW;

    private:
        virtual color_rgba_node * to_color_rgba() OPENVRML_NOTHROW;
    };


    class OPENVRML_API coordinate_node : public virtual node {
    public:
        virtual ~coordinate_node() OPENVRML_NOTHROW = 0;

        virtual const std::vector<vec3f> & point() const OPENVRML_NOTHROW = 0;

    protected:
        coordinate_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope)
            OPENVRML_NOTHROW;

    private:
        virtual coordinate_node * to_coordinate() OPENVRML_NOTHROW;
    };


    class OPENVRML_API font_style_node : public virtual node {
    public:
        virtual ~font_style_node() OPENVRML_NOTHROW = 0;

        virtual const std::vector<std::string> & family() const
            OPENVRML_NOTHROW = 0;
        virtual bool horizontal() const OPENVRML_NOTHROW = 0;
        virtual const std::vector<std::string> & justify() const
            OPENVRML_NOTHROW = 0;
        virtual const std::string & language() const OPENVRML_NOTHROW = 0;
        virtual bool left_to_right() const OPENVRML_NOTHROW = 0;
        virtual float size() const OPENVRML_NOTHROW = 0;
        virtual float spacing() const OPENVRML_NOTHROW = 0;
        virtual const std::string & style() const OPENVRML_NOTHROW = 0;
        virtual bool top_to_bottom() const OPENVRML_NOTHROW = 0;

    protected:
        font_style_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope)
            OPENVRML_NOTHROW;

    private:
        virtual font_style_node * to_font_style() OPENVRML_NOTHROW;
    };


    class OPENVRML_API geometry_node : public virtual bounded_volume_node {
        viewer::object_t geometry_reference;

    public:
        virtual ~geometry_node() OPENVRML_NOTHROW = 0;

        viewer::object_t render_geometry(viewer & v,
                                         rendering_context context);
        bool emissive() const OPENVRML_NOTHROW;
        virtual const color_node * color() const OPENVRML_NOTHROW;

    protected:
        geometry_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope)
            OPENVRML_NOTHROW;

    private:
        virtual viewer::object_t
        do_render_geometry(viewer & v, rendering_context context);
        virtual bool do_emissive() const OPENVRML_NOTHROW;

        virtual geometry_node * to_geometry() OPENVRML_NOTHROW;
    };


    class OPENVRML_API grouping_node : public virtual child_node {
    public:
        virtual ~grouping_node() OPENVRML_NOTHROW = 0;

        const std::vector<boost::intrusive_ptr<node> > & children() const
            OPENVRML_NOTHROW;
        void activate_pointing_device_sensors(double timestamp,
                                              bool over,
                                              bool active,
                                              const double (&p)[3]);

    protected:
        grouping_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope)
            OPENVRML_NOTHROW;

    private:
        virtual grouping_node * to_grouping() OPENVRML_NOTHROW;
        virtual const std::vector<boost::intrusive_ptr<node> > &
        do_children() const OPENVRML_NOTHROW = 0;
    };


    class OPENVRML_API light_node : public virtual child_node {
    public:
        virtual ~light_node() OPENVRML_NOTHROW = 0;

    protected:
        light_node(const node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope)
            OPENVRML_NOTHROW;

    private:
        virtual light_node * to_light() OPENVRML_NOTHROW;
    };


    class OPENVRML_API material_node : public virtual node {
    public:
        virtual ~material_node() OPENVRML_NOTHROW = 0;

        virtual float ambient_intensity() const OPENVRML_NOTHROW = 0;
        virtual const color & diffuse_color() const OPENVRML_NOTHROW = 0;
        virtual const color & emissive_color() const OPENVRML_NOTHROW = 0;
        virtual float shininess() const OPENVRML_NOTHROW = 0;
        virtual const color & specular_color() const OPENVRML_NOTHROW = 0;
        virtual float transparency() const OPENVRML_NOTHROW = 0;

    protected:
        material_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope)
            OPENVRML_NOTHROW;

    private:
        virtual material_node * to_material() OPENVRML_NOTHROW;
    };


    class OPENVRML_API navigation_info_node : public virtual child_node {
    public:
        virtual ~navigation_info_node() OPENVRML_NOTHROW = 0;

        virtual const std::vector<float> & avatar_size() const
            OPENVRML_NOTHROW = 0;
        virtual bool headlight() const OPENVRML_NOTHROW = 0;
        virtual float speed() const OPENVRML_NOTHROW = 0;
        virtual const std::vector<std::string> & type() const
            OPENVRML_NOTHROW = 0;
        virtual float visibility_limit() const OPENVRML_NOTHROW = 0;

    protected:
        navigation_info_node(const node_type & type,
                             const boost::shared_ptr<openvrml::scope> & scope)
            OPENVRML_NOTHROW;

    private:
        virtual navigation_info_node * to_navigation_info() OPENVRML_NOTHROW;
    };


    class OPENVRML_API normal_node : public virtual node {
    public:
        virtual ~normal_node() OPENVRML_NOTHROW = 0;

        virtual const std::vector<vec3f> & vector() const OPENVRML_NOTHROW = 0;

    protected:
        normal_node(const node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope)
            OPENVRML_NOTHROW;

    private:
        virtual normal_node * to_normal() OPENVRML_NOTHROW;
    };


    class OPENVRML_API pointing_device_sensor_node :
        public virtual child_node {
    public:
        virtual ~pointing_device_sensor_node() OPENVRML_NOTHROW = 0;

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
            OPENVRML_NOTHROW;
    };


    class OPENVRML_API scoped_light_node : public virtual light_node {
    public:
        virtual ~scoped_light_node() OPENVRML_NOTHROW = 0;

        void render_scoped_light(viewer & v);

    protected:
        scoped_light_node(const node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope)
            OPENVRML_NOTHROW;

    private:
        virtual scoped_light_node * to_scoped_light() OPENVRML_NOTHROW;
        virtual void do_render_scoped_light(viewer & v) = 0;
    };


    class OPENVRML_API sound_source_node : public virtual node {
    public:
        virtual ~sound_source_node() OPENVRML_NOTHROW = 0;

    protected:
        sound_source_node(const node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope)
            OPENVRML_NOTHROW;

    private:
        virtual sound_source_node * to_sound_source() OPENVRML_NOTHROW;
    };


    class OPENVRML_API texture_node : public virtual node {
        viewer::texture_object_t texture_reference;

    public:
        virtual ~texture_node() OPENVRML_NOTHROW = 0;

        viewer::texture_object_t render_texture(viewer & v);

        virtual const openvrml::image & image() const OPENVRML_NOTHROW = 0;
        virtual bool repeat_s() const OPENVRML_NOTHROW = 0;
        virtual bool repeat_t() const OPENVRML_NOTHROW = 0;

    protected:
        texture_node(const node_type & type,
                     const boost::shared_ptr<openvrml::scope> & scope)
            OPENVRML_NOTHROW;

    private:
        virtual texture_node * to_texture() OPENVRML_NOTHROW;

        virtual viewer::texture_object_t do_render_texture(viewer & v);
    };


    class OPENVRML_API texture_coordinate_node : public virtual node {
    public:
        virtual ~texture_coordinate_node() OPENVRML_NOTHROW = 0;

        virtual const std::vector<vec2f> & point() const OPENVRML_NOTHROW = 0;

    protected:
        texture_coordinate_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope)
            OPENVRML_NOTHROW;

    private:
        virtual texture_coordinate_node * to_texture_coordinate()
            OPENVRML_NOTHROW;
    };


    class OPENVRML_API texture_transform_node : public virtual node {
    public:
        virtual ~texture_transform_node() OPENVRML_NOTHROW = 0;

        void render_texture_transform(viewer & v);

    protected:
        texture_transform_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope)
            OPENVRML_NOTHROW;

    private:
        virtual texture_transform_node * to_texture_transform()
            OPENVRML_NOTHROW;

        virtual void do_render_texture_transform(viewer & v);
    };


    class OPENVRML_API time_dependent_node : public virtual node {
    public:
        virtual ~time_dependent_node() OPENVRML_NOTHROW = 0;

        void update(double time);

    protected:
        time_dependent_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope)
            OPENVRML_NOTHROW;

    private:
        virtual time_dependent_node * to_time_dependent() OPENVRML_NOTHROW;

        virtual void do_update(double time) = 0;
    };


    class OPENVRML_API transform_node : public virtual grouping_node {
    public:
        virtual ~transform_node() OPENVRML_NOTHROW = 0;

        virtual const mat4f & transform() const OPENVRML_NOTHROW = 0;

    protected:
        transform_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope)
            OPENVRML_NOTHROW;

    private:
        virtual transform_node * to_transform() OPENVRML_NOTHROW;
    };


    class OPENVRML_API viewpoint_node : public virtual child_node {
    public:
        virtual ~viewpoint_node() OPENVRML_NOTHROW = 0;

        virtual const mat4f & transformation() const OPENVRML_NOTHROW = 0;
        virtual const mat4f & user_view_transform() const OPENVRML_NOTHROW = 0;
        virtual void user_view_transform(const mat4f & transform)
            OPENVRML_NOTHROW = 0;
        virtual const std::string & description() const OPENVRML_NOTHROW = 0;
        virtual float field_of_view() const OPENVRML_NOTHROW = 0;

    protected:
        viewpoint_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope)
            OPENVRML_NOTHROW;

    private:
        virtual viewpoint_node * to_viewpoint() OPENVRML_NOTHROW;
    };


    class OPENVRML_API node_traverser : boost::noncopyable {
        std::set<node *> traversed_nodes;
        bool halt;

    public:
        node_traverser() OPENVRML_THROW1(std::bad_alloc);
        virtual ~node_traverser() OPENVRML_NOTHROW = 0;

        void traverse(node & n);
        void traverse(const boost::intrusive_ptr<node> & node);
        void traverse(const std::vector<boost::intrusive_ptr<node> > & nodes);

    protected:
        void halt_traversal() OPENVRML_NOTHROW;
        bool halted() OPENVRML_NOTHROW;
        bool traversed(node & n) OPENVRML_NOTHROW;

    private:
        virtual void on_entering(node & n);
        virtual void on_leaving(node & n);

        OPENVRML_LOCAL void do_traversal(node & n);
    };
}

# endif
