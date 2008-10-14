// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007  Braden McDaniel
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

# include "browser.h"
# include "scene.h"
# include "scope.h"
# include "x3d_vrml_grammar.h"
# include "vrml97node.h"
# include "x3d_core.h"
# include "x3d_networking.h"
# include "x3d_grouping.h"
# include "x3d_rendering.h"
# include "x3d_shape.h"
# include "x3d_geometry2d.h"
# include "x3d_texturing.h"
# include "x3d_interpolation.h"
# include "x3d_key_device_sensor.h"
# include "x3d_event_utilities.h"
# include "x3d_dis.h"
# include "x3d_environmental_effects.h"
# include "x3d_geospatial.h"
# include "x3d_hanim.h"
# include "x3d_nurbs.h"
# include "x3d_cad_geometry.h"
# include <openvrml/local/uri.h>
# include <openvrml/local/xml_reader.h>
# include <openvrml/local/field_value_types.h>
# include <openvrml/local/proto.h>
# include <private.h>
# include <boost/algorithm/string/predicate.hpp>
# include <boost/bind.hpp>
# include <boost/enable_shared_from_this.hpp>
# include <boost/filesystem.hpp>
# include <boost/functional.hpp>
# include <boost/lexical_cast.hpp>
# include <boost/mpl/for_each.hpp>
# include <boost/multi_index/detail/scope_guard.hpp>
# include <boost/ptr_container/ptr_map.hpp>
# include <boost/thread/thread.hpp>
# include <algorithm>
# include <functional>
# include <cerrno>
# ifdef _WIN32
#   include <sys/timeb.h>
#   include <direct.h>
#   include <time.h>
# else
#   include <sys/time.h>
# endif

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

using namespace boost::multi_index::detail;  // for scope_guard

/**
 * @file openvrml/browser.h
 *
 * @brief Classes that manage the scene.
 */

namespace openvrml {

    class OPENVRML_LOCAL null_node_metatype : public node_metatype {
    public:
        explicit null_node_metatype(openvrml::browser & browser)
            OPENVRML_NOTHROW;
        virtual ~null_node_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_NOTHROW;
    };


    class OPENVRML_LOCAL null_node_type : public node_type {
    public:
        explicit null_node_type(null_node_metatype & nodeClass)
            OPENVRML_NOTHROW;
        virtual ~null_node_type() OPENVRML_NOTHROW;

    private:
        virtual const node_interface_set & do_interfaces() const
            OPENVRML_NOTHROW;
        virtual const boost::intrusive_ptr<node>
        do_create_node(const boost::shared_ptr<openvrml::scope> & scope,
                       const initial_value_map & initial_values) const
            OPENVRML_NOTHROW;
    };
} // namespace openvrml


/**
 * @brief @c node_metatype for @c EXTERNPROTO%s.
 */
class OPENVRML_LOCAL openvrml::browser::externproto_node_metatype :
    public boost::enable_shared_from_this<externproto_node_metatype>,
    public openvrml::node_metatype {

    friend class externproto_node_type;

    struct load_proto;

    mutable boost::mutex mutex_;
    boost::weak_ptr<openvrml::local::proto_node_metatype> proto_node_metatype_;

    typedef std::vector<boost::weak_ptr<externproto_node_type> >
        externproto_node_types;

    mutable externproto_node_types externproto_node_types_;
    bool externproto_node_types_cleared_;

    boost::thread * const load_proto_thread_;

public:
    externproto_node_metatype(
        const openvrml::node_metatype_id & id,
        const openvrml::scene & scene,
        const std::vector<std::string> & uris,
        boost::thread_group & load_proto_thread_group)
        OPENVRML_THROW2(boost::thread_resource_error, std::bad_alloc);
    virtual ~externproto_node_metatype() OPENVRML_NOTHROW;

private:
    virtual const boost::shared_ptr<openvrml::node_type>
        do_create_type(const std::string & id,
                       const openvrml::node_interface_set & interfaces) const
        OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc);

    virtual void do_shutdown(double time) OPENVRML_NOTHROW;

    void set_proto_node_metatype(
        const boost::weak_ptr<openvrml::local::proto_node_metatype> & proto_node_metatype = boost::weak_ptr<openvrml::local::proto_node_metatype>())
        OPENVRML_THROW1(std::bad_alloc);

    void clear_externproto_node_types() OPENVRML_NOTHROW;
};


class OPENVRML_LOCAL openvrml::browser::externproto_node_type :
    public openvrml::node_type {

    const boost::shared_ptr<const externproto_node_metatype> node_metatype_;

    openvrml::node_interface_set interfaces_;

    mutable boost::mutex mutex_;
    boost::shared_ptr<openvrml::node_type> proto_node_type_;

    typedef std::vector<boost::intrusive_ptr<externproto_node> >
        externproto_nodes;

    mutable externproto_nodes externproto_nodes_;

public:
    externproto_node_type(
        const boost::shared_ptr<const externproto_node_metatype> & c,
        const std::string & id,
        const openvrml::node_interface_set & interfaces)
        OPENVRML_THROW1(std::bad_alloc);

    virtual ~externproto_node_type() OPENVRML_NOTHROW;

    void set_proto_node_type(openvrml::local::proto_node_metatype & proto_node_metatype)
        OPENVRML_THROW1(std::bad_alloc);

    void clear_externproto_nodes() OPENVRML_NOTHROW;

private:
    virtual const openvrml::node_interface_set & do_interfaces() const
        OPENVRML_NOTHROW;

    virtual const boost::intrusive_ptr<openvrml::node>
        do_create_node(
            const boost::shared_ptr<openvrml::scope> & scope,
            const openvrml::initial_value_map & initial_values) const
        OPENVRML_THROW3(openvrml::unsupported_interface, std::bad_cast,
                        std::bad_alloc);
};


class OPENVRML_LOCAL openvrml::externproto_node :
    public openvrml::local::abstract_proto_node {

    template <typename FieldValue>
    class externproto_exposedfield : public FieldValue,
                                     public proto_eventin<FieldValue>,
                                     public proto_eventout<FieldValue> {
    public:
        explicit externproto_exposedfield(
            externproto_node & node,
            const FieldValue & initial_value = FieldValue())
            OPENVRML_NOTHROW;
        virtual ~externproto_exposedfield() OPENVRML_NOTHROW;

    private:
        virtual std::auto_ptr<openvrml::field_value> do_clone() const
            OPENVRML_THROW1(std::bad_alloc);
        virtual void do_process_event(const FieldValue & value,
                                      double timestamp)
            OPENVRML_THROW1(std::bad_alloc);
    };

    struct externproto_exposedfield_creator {
        externproto_exposedfield_creator(
            externproto_node & node,
            const openvrml::field_value::type_id type,
            boost::shared_ptr<openvrml::field_value> & exposedfield):
            node_(&node),
            type_(type),
            exposedfield_(&exposedfield)
        {}

        template <typename T>
        void operator()(T) const
        {
            if (T::field_value_type_id == this->type_) {
                this->exposedfield_->reset(
                    new externproto_exposedfield<T>(*this->node_));
            }
        }

    private:
        externproto_node * node_;
        openvrml::field_value::type_id type_;
        boost::shared_ptr<openvrml::field_value> * exposedfield_;
    };

    static const boost::shared_ptr<openvrml::field_value>
        create_exposedfield(externproto_node & node,
                            openvrml::field_value::type_id type)
        OPENVRML_THROW1(std::bad_alloc);

    typedef std::map<std::string, boost::shared_ptr<openvrml::field_value> >
        field_map;

    struct externproto_field_equal_to :
        std::unary_function<field_map::value_type, bool> {

        explicit externproto_field_equal_to(
            const openvrml::field_value & field_value):
            field_value_(&field_value)
        {}

        bool operator()(const field_map::value_type & arg) const
        {
            return this->field_value_ == arg.second.get();
        }

    private:
        const openvrml::field_value * field_value_;
    };

    initial_value_map initial_values_;
    std::set<openvrml::field_value *> received_event_;
    field_map field_map_;

    boost::intrusive_ptr<openvrml::node> proto_node_;

public:
    externproto_node(const browser::externproto_node_type & type,
                     const boost::shared_ptr<openvrml::scope> & scope,
                     const openvrml::initial_value_map & initial_values)
        OPENVRML_THROW1(std::bad_alloc);

    virtual ~externproto_node() OPENVRML_NOTHROW;

    virtual bool modified() const;

    void set_proto_node(node_type & type)
        OPENVRML_THROW1(std::bad_alloc);

private:
    virtual void do_initialize(double timestamp)
        OPENVRML_THROW1(std::bad_alloc);

    virtual const openvrml::field_value &
        do_field(const std::string & id) const
        OPENVRML_THROW1(openvrml::unsupported_interface);

    virtual openvrml::event_listener &
        do_event_listener(const std::string & id)
        OPENVRML_THROW1(openvrml::unsupported_interface);

    virtual openvrml::event_emitter &
        do_event_emitter(const std::string & id)
        OPENVRML_THROW1(openvrml::unsupported_interface);

    virtual void do_shutdown(double timestamp) OPENVRML_NOTHROW;

    virtual openvrml::script_node * to_script() OPENVRML_NOTHROW;
    virtual openvrml::appearance_node * to_appearance() OPENVRML_NOTHROW;
    virtual openvrml::bounded_volume_node * to_bounded_volume()
        OPENVRML_NOTHROW;
    virtual openvrml::child_node * to_child() OPENVRML_NOTHROW;
    virtual openvrml::color_node * to_color() OPENVRML_NOTHROW;
    virtual openvrml::color_rgba_node * to_color_rgba() OPENVRML_NOTHROW;
    virtual openvrml::coordinate_node * to_coordinate() OPENVRML_NOTHROW;
    virtual openvrml::font_style_node * to_font_style() OPENVRML_NOTHROW ;
    virtual openvrml::geometry_node * to_geometry() OPENVRML_NOTHROW;
    virtual openvrml::grouping_node * to_grouping() OPENVRML_NOTHROW;
    virtual openvrml::light_node * to_light() OPENVRML_NOTHROW;
    virtual openvrml::material_node * to_material() OPENVRML_NOTHROW;
    virtual openvrml::navigation_info_node * to_navigation_info()
        OPENVRML_NOTHROW;
    virtual openvrml::normal_node * to_normal() OPENVRML_NOTHROW;
    virtual openvrml::pointing_device_sensor_node *
        to_pointing_device_sensor() OPENVRML_NOTHROW;
    virtual openvrml::scoped_light_node * to_scoped_light()
        OPENVRML_NOTHROW;
    virtual openvrml::sound_source_node * to_sound_source()
        OPENVRML_NOTHROW;
    virtual openvrml::texture_node * to_texture() OPENVRML_NOTHROW;
    virtual openvrml::texture_coordinate_node * to_texture_coordinate()
        OPENVRML_NOTHROW;
    virtual openvrml::texture_transform_node * to_texture_transform()
        OPENVRML_NOTHROW;
    virtual openvrml::time_dependent_node * to_time_dependent()
        OPENVRML_NOTHROW;
    virtual openvrml::transform_node * to_transform() OPENVRML_NOTHROW;
    virtual openvrml::viewpoint_node * to_viewpoint() OPENVRML_NOTHROW;
};

namespace {

    class OPENVRML_LOCAL default_navigation_info :
        public openvrml::navigation_info_node {
    public:
        explicit default_navigation_info(const openvrml::null_node_type & type)
            OPENVRML_NOTHROW;
        virtual ~default_navigation_info() OPENVRML_NOTHROW;

        virtual const std::vector<float> & avatar_size() const
            OPENVRML_NOTHROW;
        virtual bool headlight() const OPENVRML_NOTHROW;
        virtual float speed() const OPENVRML_NOTHROW;
        virtual const std::vector<std::string> & type() const OPENVRML_NOTHROW;
        virtual float visibility_limit() const OPENVRML_NOTHROW;

    private:
        virtual void do_field(const std::string & id,
                              const openvrml::field_value & value)
            OPENVRML_NOTHROW;
        virtual const openvrml::field_value &
        do_field(const std::string & id) const
            OPENVRML_NOTHROW;
        virtual void do_process_event(const std::string & id,
                                      const openvrml::field_value & value,
                                      double timestamp)
            OPENVRML_NOTHROW;
        virtual const openvrml::field_value &
        do_eventout(const std::string & id) const
            OPENVRML_NOTHROW;

        virtual openvrml::event_listener &
        do_event_listener(const std::string & id)
            OPENVRML_THROW1(openvrml::unsupported_interface);
        virtual openvrml::event_emitter &
        do_event_emitter(const std::string & id)
            OPENVRML_THROW1(openvrml::unsupported_interface);
    };

    const boost::shared_ptr<openvrml::scope> null_scope_ptr;

    /**
     * @brief Construct.
     *
     * @param[in] t @c node_type.
     */
    default_navigation_info::
    default_navigation_info(const openvrml::null_node_type & t)
        OPENVRML_NOTHROW:
        openvrml::node(t, null_scope_ptr),
        openvrml::bounded_volume_node(t, null_scope_ptr),
        openvrml::child_node(t, null_scope_ptr),
        openvrml::navigation_info_node(t, null_scope_ptr)
    {}

    /**
     * @brief Destroy
     */
    default_navigation_info::~default_navigation_info() OPENVRML_NOTHROW
    {}

    /**
     * @brief The avatar dimensions.
     *
     * @return [0.25, 1.6, 0.75]
     */
    const std::vector<float> & default_navigation_info::avatar_size() const
        OPENVRML_NOTHROW
    {
        static const float array[] = { 0.25f, 1.6f, 0.75f };
        static const std::vector<float> vec(array, array + 3);
        return vec;
    }

    /**
     * @brief The headlight state.
     *
     * @return @c true
     */
    bool default_navigation_info::headlight() const OPENVRML_NOTHROW
    {
        return true;
    }

    /**
     * @brief The speed of the user view.
     *
     * @return 1.0
     */
    float default_navigation_info::speed() const OPENVRML_NOTHROW
    {
        return 1.0;
    }

    /**
     * @brief The navigation type.
     *
     * @return ["WALK", "ANY"]
     */
    const std::vector<std::string> & default_navigation_info::type() const
        OPENVRML_NOTHROW
    {
        static const char * array[] = { "WALK", "ANY" };
        static const std::vector<std::string> vec(array, array + 2);
        return vec;
    }

    /**
     * @brief The visibility limit.
     *
     * @return 0.0
     */
    float default_navigation_info::visibility_limit() const OPENVRML_NOTHROW
    {
        return 0.0;
    }

    void default_navigation_info::do_field(const std::string &,
                                           const openvrml::field_value &)
        OPENVRML_NOTHROW
    {
        assert(false);
    }

    const openvrml::field_value &
    default_navigation_info::do_field(const std::string &) const
        OPENVRML_NOTHROW
    {
        assert(false);
        static const openvrml::sfbool value;
        return value;
    }

    void
    default_navigation_info::do_process_event(const std::string &,
                                              const openvrml::field_value &,
                                              double)
        OPENVRML_NOTHROW
    {
        assert(false);
    }

    const openvrml::field_value &
    default_navigation_info::do_eventout(const std::string &) const
        OPENVRML_NOTHROW
    {
        assert(false);
        static const openvrml::sfbool value;
        return value;
    }

    openvrml::event_listener &
    default_navigation_info::do_event_listener(const std::string & id)
        OPENVRML_THROW1(openvrml::unsupported_interface)
    {
        assert(false);
        throw openvrml::unsupported_interface(this->node::type(), id);
        return *static_cast<openvrml::event_listener *>(0);
    }

    openvrml::event_emitter &
    default_navigation_info::do_event_emitter(const std::string & id)
        OPENVRML_THROW1(openvrml::unsupported_interface)
    {
        assert(false);
        throw openvrml::unsupported_interface(this->node::type(), id);
        return *static_cast<openvrml::event_emitter *>(0);
    }


    class OPENVRML_LOCAL default_viewpoint : public openvrml::viewpoint_node {
        openvrml::mat4f userViewTransform;

    public:
        explicit default_viewpoint(const openvrml::null_node_type & type)
            OPENVRML_NOTHROW;
        virtual ~default_viewpoint() OPENVRML_NOTHROW;

        virtual const openvrml::mat4f & transformation() const
            OPENVRML_NOTHROW;
        virtual const openvrml::mat4f & user_view_transform() const
            OPENVRML_NOTHROW;
        virtual void user_view_transform(const openvrml::mat4f & transform)
            OPENVRML_NOTHROW;
        virtual const std::string & description() const OPENVRML_NOTHROW;
        virtual float field_of_view() const OPENVRML_NOTHROW;

    private:
        virtual void do_field(const std::string & id,
                              const openvrml::field_value & value)
            OPENVRML_NOTHROW;
        virtual const openvrml::field_value &
        do_field(const std::string & id) const
            OPENVRML_NOTHROW;
        virtual void do_process_event(const std::string & id,
                                      const openvrml::field_value & value,
                                      double timestamp)
            OPENVRML_NOTHROW;
        virtual const openvrml::field_value &
        do_eventout(const std::string & id) const
            OPENVRML_NOTHROW;

        virtual openvrml::event_listener &
        do_event_listener(const std::string & id)
            OPENVRML_THROW1(openvrml::unsupported_interface);
        virtual openvrml::event_emitter &
        do_event_emitter(const std::string & id)
            OPENVRML_THROW1(openvrml::unsupported_interface);
    };

    /**
     * @brief Construct.
     *
     * @param[in] type  the <code>browser</code>'s @c null_node_type instance.
     */
    default_viewpoint::default_viewpoint(const openvrml::null_node_type & type)
        OPENVRML_NOTHROW:
        openvrml::node(type, null_scope_ptr),
        openvrml::bounded_volume_node(type, null_scope_ptr),
        openvrml::child_node(type, null_scope_ptr),
        openvrml::viewpoint_node(type, null_scope_ptr),
        userViewTransform(openvrml::make_mat4f())
    {}

    /**
     * @brief Destroy.
     */
    default_viewpoint::~default_viewpoint() OPENVRML_NOTHROW
    {}

    const openvrml::mat4f & default_viewpoint::transformation() const
        OPENVRML_NOTHROW
    {
        using openvrml::mat4f;
        using openvrml::rotation;
        using openvrml::make_rotation;
        using openvrml::vec3f;
        using openvrml::make_vec3f;
        static const vec3f position = make_vec3f(0.0, 0.0, 10.0);
        static const rotation orientation = make_rotation();
        static const vec3f scale = make_vec3f(1.0, 1.0, 1.0);
        static const rotation scaleOrientation = make_rotation();
        static const vec3f center = make_vec3f();
        static const mat4f t(make_transformation_mat4f(position,
                                                       orientation,
                                                       scale,
                                                       scaleOrientation,
                                                       center));
        return t;
    }

    const openvrml::mat4f & default_viewpoint::user_view_transform() const
        OPENVRML_NOTHROW
    {
        return this->userViewTransform;
    }

    void
    default_viewpoint::user_view_transform(const openvrml::mat4f & transform)
        OPENVRML_NOTHROW
    {
        this->userViewTransform = transform;
    }

    const std::string & default_viewpoint::description() const OPENVRML_NOTHROW
    {
        static const std::string desc;
        return desc;
    }

    float default_viewpoint::field_of_view() const OPENVRML_NOTHROW
    {
        static const float fieldOfView = 0.785398f;
        return fieldOfView;
    }

    void default_viewpoint::do_field(const std::string &,
                                     const openvrml::field_value &)
        OPENVRML_NOTHROW
    {
        assert(false);
    }

    const openvrml::field_value &
    default_viewpoint::do_field(const std::string &) const
        OPENVRML_NOTHROW
    {
        assert(false);
        static const openvrml::sfbool value;
        return value;
    }

    void default_viewpoint::do_process_event(const std::string &,
                                             const openvrml::field_value &,
                                             double)
        OPENVRML_NOTHROW
    {
        assert(false);
    }

    const openvrml::field_value &
    default_viewpoint::do_eventout(const std::string &) const OPENVRML_NOTHROW
    {
        assert(false);
        static const openvrml::sfbool value;
        return value;
    }

    openvrml::event_listener &
    default_viewpoint::do_event_listener(const std::string & id)
        OPENVRML_THROW1(openvrml::unsupported_interface)
    {
        assert(false);
        throw openvrml::unsupported_interface(this->type(), id);
        return *static_cast<openvrml::event_listener *>(0);
    }

    openvrml::event_emitter &
    default_viewpoint::do_event_emitter(const std::string & id)
        OPENVRML_THROW1(openvrml::unsupported_interface)
    {
        assert(false);
        throw openvrml::unsupported_interface(this->type(), id);
        return *static_cast<openvrml::event_emitter *>(0);
    }


    const std::string anonymous_stream_id_prefix_ =
        "urn:X-openvrml:stream:";

    OPENVRML_LOCAL bool anonymous_stream_id(const openvrml::local::uri & id)
    {
        const std::string str(id);
        return str.length() > anonymous_stream_id_prefix_.length()
            && std::equal(anonymous_stream_id_prefix_.begin(),
                          anonymous_stream_id_prefix_.end(),
                          str.begin());
    }


    class OPENVRML_LOCAL profile {
        typedef std::map<std::string, int> map_t;
        map_t components_;

    protected:
        void add_component(const std::string & name, int level)
            OPENVRML_THROW1(std::bad_alloc);

    public:
        std::auto_ptr<openvrml::scope>
            create_root_scope(const openvrml::browser & browser,
                              const std::string & uri) const
            OPENVRML_THROW1(std::bad_alloc);
        std::auto_ptr<openvrml::node_type_decls>
            create_node_type_desc_map() const
            OPENVRML_THROW1(std::bad_alloc);
    };


    class OPENVRML_LOCAL vrml97_profile : public profile {
    public:
        static const char * const id;

        vrml97_profile();
    };


    class OPENVRML_LOCAL x3d_core_profile : public profile {
    public:
        static const char * const id;

        x3d_core_profile();
    };


    class OPENVRML_LOCAL x3d_interchange_profile : public profile {
    public:
        static const char * const id;

        x3d_interchange_profile();
    };


    class OPENVRML_LOCAL x3d_interactive_profile : public profile {
    public:
        static const char * const id;

        x3d_interactive_profile();
    };


    class OPENVRML_LOCAL x3d_mpeg4_profile : public profile {
    public:
        static const char * const id;

        x3d_mpeg4_profile();
    };


    class OPENVRML_LOCAL x3d_immersive_profile : public profile {
    public:
        static const char * const id;

        x3d_immersive_profile();
    };


    class OPENVRML_LOCAL x3d_full_profile : public profile {
    public:
        static const char * const id;

        x3d_full_profile();
    };
}

namespace {

    class OPENVRML_LOCAL component {
        struct node_type_decl {
            openvrml::node_interface_set interfaces;
            std::string metatype_id;
        };

        class xml_reader;

        class level : std::map<std::string, node_type_decl> {
            friend class xml_reader;

        public:
            typedef std::map<std::string, size_t> dependencies_t;

        private:
            dependencies_t dependencies_;

        public:
            typedef std::map<std::string, node_type_decl> base_t;
            using base_t::value_type;
            using base_t::iterator;
            using base_t::const_iterator;
            using base_t::begin;
            using base_t::end;
            using base_t::find;
            using base_t::insert;

            const dependencies_t & requires() const;
        };

        class xml_reader : boost::noncopyable {
            enum parse_state {
                none,
                component,
                level,
                requires,
                node,
                field
            };

            ::component & component_;
            parse_state state_;
            std::vector<component::level>::value_type * current_level_;
            level::value_type * current_node_;

        public:
            explicit xml_reader(::component & c);

            void read(const std::string & filename)
                OPENVRML_THROW1(std::runtime_error);

        private:
            void process_node(openvrml::local::xml_reader & reader);
            void start_element(openvrml::local::xml_reader & reader);
            void end_element(openvrml::local::xml_reader & reader);
        };

        std::string id_;
        std::vector<level> levels_;

    public:
        explicit component(const std::string & filename)
            OPENVRML_THROW1(std::runtime_error);

        const std::string & id() const OPENVRML_NOTHROW;
        size_t support_level() const OPENVRML_NOTHROW;
        void add_to_scope(const openvrml::browser & b,
                          openvrml::scope & scope,
                          size_t level) const
            OPENVRML_THROW2(std::invalid_argument, std::bad_alloc);
        void add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                       size_t level) const
            OPENVRML_THROW2(std::invalid_argument, std::bad_alloc);

    private:
        static bool
            add_type_desc(openvrml::node_type_decls & type_descs,
                          const std::string & node_type_id,
                          const openvrml::node_interface_set & interfaces)
            OPENVRML_THROW1(std::bad_alloc);

        void add_scope_entry(
            const openvrml::browser & b,
            const char * node_name,
            const openvrml::node_interface_set & interface_set,
            const char * urn,
            openvrml::scope & scope) const
            OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc);
    };

    const class OPENVRML_LOCAL profile_registry : boost::ptr_map<std::string,
                                                                 profile> {
    public:
        profile_registry();

        using boost::ptr_map<std::string, profile>::at;
    } profile_registry_;

    profile_registry::profile_registry()
    {
        std::string key;
        bool succeeded;

        key = vrml97_profile::id;
        succeeded = this->insert(key, new vrml97_profile).second;
        assert(succeeded);

        key = x3d_core_profile::id;
        succeeded = this->insert(key, new x3d_core_profile).second;
        assert(succeeded);

        key = x3d_interchange_profile::id;
        succeeded = this->insert(key, new x3d_interchange_profile).second;
        assert(succeeded);

        key = x3d_interactive_profile::id;
        succeeded = this->insert(key, new x3d_interactive_profile).second;
        assert(succeeded);

        key = x3d_mpeg4_profile::id;
        succeeded = this->insert(key, new x3d_mpeg4_profile).second;
        assert(succeeded);

        key = x3d_immersive_profile::id;
        succeeded = this->insert(key, new x3d_immersive_profile).second;
        assert(succeeded);

        key = x3d_full_profile::id;
        succeeded = this->insert(key, new x3d_full_profile).second;
        assert(succeeded);
    }
}

/**
 * @brief Get a @c node_type_decls map corresponding to a particular profile.
 *
 * @param[in] profile_id    a profile identifier.
 *
 * @return a @c node_type_decls map corresponding to @p profile_id.
 *
 * @exception std::invalid_argument if @p profile_id does not correspond to a
 *                                  known profile.
 * @exception std::bad_alloc        if memory allocation fails.
 */
std::auto_ptr<openvrml::node_type_decls>
openvrml::profile(const std::string & profile_id)
    OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
{
    try {
        return ::profile_registry_.at(profile_id).create_node_type_desc_map();
    } catch (boost::bad_ptr_container_operation &) {
        throw std::invalid_argument("unknown profile identifier \""
                                    + profile_id + '"');
    }
}

namespace {

    const class OPENVRML_LOCAL component_registry : boost::ptr_map<std::string,
                                                                   component> {
    public:
        component_registry()
            OPENVRML_THROW2(boost::filesystem::filesystem_error,
                            std::bad_alloc);

        using boost::ptr_map<std::string, component>::at;
    } component_registry_;
}

/**
 * @brief Add the nodes corresponding to a component level to a
 *        @c node_type_decls map.
 *
 * @param[in,out] node_types    a @c node_type_decls map.
 * @param[in] component_id      a component identifier.
 * @param[in] level             a component level.
 *
 * @exception std::invalid_argument if @p component_id is not a recognized
 *                                  component identifier; or if @p level does
 *                                  not correspond to a supported level of
 *                                  @p component_id.
 * @exception std::bad_alloc        if memory allocation fails.
 */
void openvrml::add_component(node_type_decls & node_types,
                             const std::string & component_id,
                             const size_t level)
    OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
{
    try {
        const component & comp = ::component_registry_.at(component_id);
        comp.add_to_node_type_desc_map(node_types, level);
    } catch (boost::bad_ptr_container_operation &) {
        throw std::invalid_argument("unknown component identifier \""
                                    + component_id + '"');
    }
}


struct OPENVRML_LOCAL openvrml::browser::vrml97_parse_actions {
    vrml97_parse_actions(
        const std::string & uri,
        const openvrml::scene & scene,
        std::vector<boost::intrusive_ptr<openvrml::node> > & nodes):
        on_scene_start(*this),
        on_scene_finish(*this),
        on_externproto(*this),
        on_proto_start(*this),
        on_proto_interface(*this),
        on_proto_default_value_start(*this),
        on_proto_default_value_finish(*this),
        on_proto_body_start(*this),
        on_proto_finish(*this),
        on_node_start(*this),
        on_node_finish(*this),
        on_script_interface_decl(*this),
        on_route(*this),
        on_use(*this),
        on_is_mapping(*this),
        on_field_start(*this),
        on_sfnode(*this),
        on_mfnode(*this),
        on_sfbool(*this),
        on_sfcolor(*this),
        on_mfcolor(*this),
        on_sffloat(*this),
        on_mffloat(*this),
        on_sfimage(*this),
        on_sfint32(*this),
        on_mfint32(*this),
        on_sfrotation(*this),
        on_mfrotation(*this),
        on_sfstring(*this),
        on_mfstring(*this),
        on_sftime(*this),
        on_mftime(*this),
        on_sfvec2f(*this),
        on_mfvec2f(*this),
        on_sfvec3f(*this),
        on_mfvec3f(*this),
        uri_(uri),
        scene_(scene),
        nodes_(nodes)
    {}

    struct on_scene_start_t {
        explicit on_scene_start_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()() const
        {
            using std::vector;
            using boost::intrusive_ptr;

            this->actions_.ps.push(parse_scope());

            const ::profile & p = ::profile_registry_.at(vrml97_profile::id);
            std::auto_ptr<scope>
                root_scope(p.create_root_scope(this->actions_.scene_.browser(),
                                               this->actions_.uri_));
            this->actions_.ps.top().scope = root_scope;
            this->actions_.ps.top().children.push(
                parse_scope::children_t());
        }

    private:
        vrml97_parse_actions & actions_;
    } on_scene_start;

    struct on_scene_finish_t {
        explicit on_scene_finish_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()() const
        {
            using std::vector;

            assert(this->actions_.ps.top().children.size() == 1);
            this->actions_.nodes_ =
                this->actions_.ps.top().children.top();
            this->actions_.ps.top().children.pop();

            assert(this->actions_.ps.size() == 1);

            parse_scope & ps = this->actions_.ps.top();

            for (vector<route>::const_iterator r = ps.routes.begin();
                 r != ps.routes.end();
                 ++r) {
                node * const from = ps.scope->find_node(r->from);
                assert(from);
                node * const to = ps.scope->find_node(r->to);
                assert(to);
                //
                // Redundant routing is ignored (4.10.2), so we don't care if
                // add_route returns false.
                //
                add_route(*from, r->eventout, *to, r->eventin);
            }
            this->actions_.ps.pop();
        }

    private:
        vrml97_parse_actions & actions_;
    } on_scene_finish;

    struct on_externproto_t {
        explicit on_externproto_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::string & node_type_id,
                        const node_interface_set & interfaces,
                        const std::vector<std::string> & uri_list) const
        {
            using std::string;
            using std::vector;
            using boost::shared_ptr;
            using local::uri;

            //
            // If we are parsing an "anonymous" stream--that is, one passed to
            // browser::create_vrml_from_stream--we cannot use the URI
            // associated with the stream to resolve relative URIs.  So in
            // this instance, we get the base URI from browser::world_url.  If
            // browser::world_url is an empty string, we call create_file_url
            // with an empty (relative) uri.
            //
            const uri base_uri =
                anonymous_stream_id(uri(this->actions_.uri_))
                ? this->actions_.scene_.browser().world_url().empty()
                    ? create_file_url(uri())
                    : uri(this->actions_.scene_.browser().world_url())
                : uri(this->actions_.uri_);

            shared_ptr<node_type> node_type;
            for (vector<string>::const_iterator resource_id = uri_list.begin();
                 resource_id != uri_list.end();
                 ++resource_id) {
                const uri absolute_uri = relative(uri(*resource_id))
                    ? resolve_against(uri(*resource_id), base_uri)
                    : uri(*resource_id);
                const shared_ptr<openvrml::node_metatype> node_metatype =
                    this->actions_.scene_.browser().node_metatype(
                        node_metatype_id(absolute_uri));
                if (node_metatype) {
                    node_type = node_metatype->create_type(node_type_id,
                                                           interfaces);
                    break;
                }
            }

            if (!node_type) {
                const std::string metatype_id =
                    path(*this->actions_.ps.top().scope) + '#'
                    + node_type_id;
                const shared_ptr<openvrml::node_metatype>
                    externproto_class(
                        new externproto_node_metatype(
                            metatype_id,
                            this->actions_.scene_,
                            uri_list,
                            this->actions_.scene_.browser()
                                .load_proto_thread_group_));

                this->actions_.scene_.browser().add_node_metatype(
                    externproto_class->id(),
                    externproto_class);
                for (vector<string>::const_iterator resource_id =
                         uri_list.begin();
                     resource_id != uri_list.end();
                     ++resource_id) {
                    const uri absolute_uri = relative(uri(*resource_id))
                        ? resolve_against(uri(*resource_id), base_uri)
                        : uri(*resource_id);
                    this->actions_.scene_.browser()
                        .add_node_metatype(node_metatype_id(absolute_uri),
                                           externproto_class);
                }

                node_type = externproto_class->create_type(node_type_id,
                                                           interfaces);
            }

            assert(node_type);

            const bool succeeded =
                this->actions_.ps.top().scope->add_type(node_type)
                .second;
            assert(succeeded);
        }

    private:
        vrml97_parse_actions & actions_;
    } on_externproto;

    struct on_proto_start_t {
        explicit on_proto_start_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::string & node_type_id) const
        {
            const boost::shared_ptr<scope> outer_scope =
                this->actions_.ps.top().scope;
            assert(outer_scope);

            this->actions_.ps.push(parse_scope());

            parse_scope & ps = this->actions_.ps.top();

            ps.proto_node_type_id = node_type_id;

            //
            // We don't want to create the new scope until we start the PROTO
            // body.  So, for now we set the parse_scope::scope to the outer
            // scope.
            //
            ps.scope = outer_scope;

            //
            // We push a node_data onto the stack to hold any field default
            // values.
            //
            ps.node_data_.push(node_data());
        }

    private:
        vrml97_parse_actions & actions_;
    } on_proto_start;

    struct on_proto_interface_t {
        explicit on_proto_interface_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const node_interface & interface_) const
        {
            parse_scope & ps = this->actions_.ps.top();

            const bool succeeded =
                ps.proto_interfaces.insert(interface_).second;
            assert(succeeded);

            assert(!ps.node_data_.empty());
            node_data & nd = ps.node_data_.top();

            if (interface_.type == node_interface::field_id
                || interface_.type == node_interface::exposedfield_id) {
                using std::auto_ptr;
                using std::pair;
                auto_ptr<field_value> value =
                    field_value::create(interface_.field_type);
                pair<initial_value_map::iterator, bool> insert_result =
                    nd.initial_values.insert(
                        make_pair(interface_.id,
                                  boost::shared_ptr<field_value>(value)));
                assert(insert_result.second);
                nd.current_field_value = &(*insert_result.first);
            }
        }

    private:
        vrml97_parse_actions & actions_;
    } on_proto_interface;

    struct on_proto_default_value_start_t {
        explicit on_proto_default_value_start_t(
            vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()() const
        {
            parse_scope & ps = this->actions_.ps.top();

            boost::shared_ptr<scope> outer_scope = ps.scope;
            const std::string & proto_node_type_id = ps.proto_node_type_id;
            assert(!proto_node_type_id.empty());
            const std::string & field_id =
                ps.node_data_.top().current_field_value->first;
            const field_value::type_id field_type =
                ps.node_data_.top().current_field_value->second->type();

            if (field_type == field_value::sfnode_id
                || field_type == field_value::mfnode_id) {
                using std::vector;
                using boost::intrusive_ptr;

                const node_data old_nd = ps.node_data_.top();

                const std::string scope_id =
                    proto_node_type_id + '.' + field_id;
                actions_.ps.push(parse_scope());

                parse_scope & new_ps = this->actions_.ps.top();

                new_ps.scope.reset(new scope(scope_id, outer_scope));
                new_ps.node_data_.push(old_nd);
                new_ps.children.push(parse_scope::children_t());
            }
        }

    private:
        vrml97_parse_actions & actions_;
    } on_proto_default_value_start;

    struct on_proto_default_value_finish_t {
        explicit on_proto_default_value_finish_t(
            vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()() const
        {
            assert(!actions_.ps.empty());

            parse_scope & ps = this->actions_.ps.top();

            const field_value::type_id field_type =
                ps.node_data_.top().current_field_value->second->type();

            if (field_type == field_value::sfnode_id
                || field_type == field_value::mfnode_id) {
                using std::vector;

                assert(actions_.ps.size() > 1);

                for (vector<route>::const_iterator r = ps.routes.begin();
                     r != ps.routes.end();
                     ++r) {
                    node * const from = ps.scope->find_node(r->from);
                    assert(from);
                    node * const to = ps.scope->find_node(r->to);
                    assert(to);
                    //
                    // Redundant routing is ignored (4.10.2), so we don't care
                    // if add_route returns false.
                    //
                    add_route(*from, r->eventout, *to, r->eventin);
                }

                parse_scope old_ps = ps;
                this->actions_.ps.pop();
                parse_scope & new_ps = this->actions_.ps.top();
                new_ps.node_data_.top().current_field_value->second->assign(
                    *old_ps.node_data_.top().current_field_value->second);
            }
        }

    private:
        vrml97_parse_actions & actions_;
    } on_proto_default_value_finish;

    struct on_proto_body_start_t {
        explicit on_proto_body_start_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()() const
        {
            assert(!actions_.ps.empty());
            const boost::shared_ptr<scope> parent =
                actions_.ps.top().scope;
            actions_.ps.top().scope
                .reset(new scope(actions_.ps.top().proto_node_type_id,
                                 parent));
            actions_.ps.top().proto_default_values =
                actions_.ps.top().node_data_.top().initial_values;
            actions_.ps.top().node_data_.pop();
            assert(actions_.ps.top().children.empty());
            actions_.ps.top().children.push(
                parse_scope::children_t());
        }

    private:
        vrml97_parse_actions & actions_;
    } on_proto_body_start;

    struct on_proto_finish_t {
        explicit on_proto_finish_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()() const
        {
            using std::vector;
            using boost::shared_ptr;
            using boost::dynamic_pointer_cast;
            using local::proto_node_metatype;
            parse_scope & ps = this->actions_.ps.top();
            assert(ps.children.size() == 1);
            assert(ps.node_data_.empty());
            proto_node_metatype::routes_t proto_routes;
            for (vector<route>::const_iterator r = ps.routes.begin();
                 r != ps.routes.end();
                 ++r) {
                node * const from = ps.scope->find_node(r->from);
                assert(from);
                node * const to = ps.scope->find_node(r->to);
                assert(to);
                proto_routes.push_back(
                    proto_node_metatype::route(*from, r->eventout,
                                               *to, r->eventin));
            }
            const shared_ptr<openvrml::node_metatype> node_metatype(
                new proto_node_metatype(
                    path(*ps.scope),
                    this->actions_.scene_.browser(),
                    ps.proto_interfaces,
                    ps.proto_default_values,
                    ps.children.top(),
                    ps.proto_is_map,
                    proto_routes));

            //
            // Add the new node_metatype (prototype definition) to the
            // browser's node_metatype_map.
            //
            this->actions_.scene_.browser()
                .add_node_metatype(node_metatype->id(), node_metatype);

            if (!dynamic_pointer_cast<proto_node_metatype>(
                    this->actions_.scene_.browser().node_metatype(
                        node_metatype_id(this->actions_.uri_)))) {
                this->actions_.scene_.browser()
                    .add_node_metatype(node_metatype_id(this->actions_.uri_),
                                       node_metatype);
            }

            //
            // PROTOs implicitly introduce a new node type as well.
            //
            const shared_ptr<node_type> node_type =
                node_metatype->create_type(
                    ps.proto_node_type_id,
                    this->actions_.ps.top().proto_interfaces);
            assert(node_type);

            //
            // We want to add the type to the PROTO's enclosing scope, so pop
            // off the current scope.
            //
            assert(this->actions_.ps.size() > 1);
            this->actions_.ps.pop();

            const bool succeeded =
                this->actions_.ps.top().scope->add_type(node_type)
                .second;
            assert(succeeded);
        }

    private:
        vrml97_parse_actions & actions_;
    } on_proto_finish;

    struct on_node_start_t {
        explicit on_node_start_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::string & node_name_id,
                        const std::string & node_type_id) const
        {
            parse_scope & ps = this->actions_.ps.top();

            ps.node_data_.push(node_data());
            ps.node_data_.top().node_name_id = node_name_id;

            assert(ps.scope);
            if (node_type_id != "Script") {
                ps.node_data_.top().type = ps.scope->find_type(node_type_id);
                assert(ps.node_data_.top().type);
            }
        }

    private:
        vrml97_parse_actions & actions_;
    } on_node_start;

    struct on_node_finish_t {
        explicit on_node_finish_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()() const
        {
            using boost::intrusive_ptr;

            parse_scope & ps = this->actions_.ps.top();

            assert(!ps.children.empty());
            assert(!ps.node_data_.empty());

            const node_data & nd = ps.node_data_.top();

            const intrusive_ptr<node> n = nd.type
                ? nd.type->create_node(ps.scope, nd.initial_values)
                : intrusive_ptr<node>(
                    new script_node(
                        this->actions_.scene_.browser().script_node_metatype_,
                        ps.scope,
                        nd.script_interfaces,
                        nd.initial_values));

            if (!nd.node_name_id.empty()) { n->id(nd.node_name_id); }

            for (node_data::is_map_t::const_iterator entry = nd.is_map.begin();
                 entry != nd.is_map.end();
                 ++entry) {
                ps.proto_is_map.insert(
                    make_pair(entry->second,
                              local::proto_node_metatype::is_target(
                                  *n, entry->first)));
            }

            ps.children.top().push_back(n);
            ps.node_data_.pop();
        }

    private:
        vrml97_parse_actions & actions_;
    } on_node_finish;

    struct on_script_interface_decl_t {
        explicit on_script_interface_decl_t(
            vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const node_interface & interface_) const
        {
            parse_scope & ps = this->actions_.ps.top();

            assert(!ps.node_data_.empty());

            node_data & nd = ps.node_data_.top();

            const bool succeeded =
                nd.script_interfaces.insert(interface_).second;
            assert(succeeded);

            this->actions_.on_field_start(interface_.id, interface_.field_type);
            if ((interface_.type == node_interface::eventin_id
                 || interface_.type == node_interface::eventout_id)
                && (interface_.field_type == field_value::sfnode_id
                    || interface_.field_type == field_value::mfnode_id)) {
                ps.children.pop();
            }
        }

    private:
        vrml97_parse_actions & actions_;
    } on_script_interface_decl;

    struct on_route_t {
        explicit on_route_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::string & from_node_name_id,
                        const node_interface & from_node_interface,
                        const std::string & to_node_name_id,
                        const node_interface & to_node_interface) const
        {
            parse_scope & ps = this->actions_.ps.top();
            const route r(from_node_name_id, from_node_interface.id,
                          to_node_name_id, to_node_interface.id);
            ps.routes.push_back(r);
        }

    private:
        vrml97_parse_actions & actions_;
    } on_route;

    struct on_use_t {
        explicit on_use_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::string & node_name_id) const
        {
            using boost::intrusive_ptr;
            parse_scope & ps = this->actions_.ps.top();
            node_data & nd = ps.node_data_.top();

            //
            // Check for self-reference.  This is legitimate in Script nodes.
            //
            node * const n = (node_name_id == nd.node_name_id)
                           ? node::self_tag.get()
                           : ps.scope->find_node(node_name_id);
            assert(n);
            assert(!ps.children.empty());
            ps.children.top().push_back(intrusive_ptr<node>(n));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_use;

    struct on_is_mapping_t {
        explicit on_is_mapping_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::string & proto_interface_id) const
        {
            node_data & nd =
                this->actions_.ps.top().node_data_.top();
            assert(nd.current_field_value);
            nd.is_map.insert(make_pair(nd.current_field_value->first,
                                       proto_interface_id));

            const field_value::type_id field_type =
                nd.current_field_value->second->type();
            if (field_type == field_value::sfnode_id
                || field_type == field_value::mfnode_id) {
                this->actions_.ps.top().children.pop();
            }

            //
            // We need to leave this around for Script nodes.
            //
            if (nd.type) {
                const initial_value_map::size_type erased =
                    nd.initial_values.erase(nd.current_field_value->first);
                assert(erased != 0);
            }
            nd.current_field_value = 0;
        }

    private:
        vrml97_parse_actions & actions_;
    } on_is_mapping;

    struct on_field_start_t {
        explicit on_field_start_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::string & field_name_id,
                        const field_value::type_id field_type) const
        {
            parse_scope & ps = this->actions_.ps.top();
            node_data & nd = ps.node_data_.top();

            using boost::shared_ptr;
            using std::pair;
            std::auto_ptr<field_value> value = field_value::create(field_type);
            pair<initial_value_map::iterator, bool> insert_result =
                nd.initial_values.insert(
                    make_pair(field_name_id, shared_ptr<field_value>(value)));
            assert(insert_result.second);
            nd.current_field_value = &(*insert_result.first);

            if (field_type == field_value::sfnode_id
                || field_type == field_value::mfnode_id) {
                ps.children.push(parse_scope::children_t());
            }
        }

    private:
        vrml97_parse_actions & actions_;
    } on_field_start;

    struct on_sfnode_t {
        explicit on_sfnode_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(bool null) const
        {
            assert(!actions_.ps.empty());
            parse_scope & ps = this->actions_.ps.top();

            if (!null) {
                assert(!ps.node_data_.empty());
                assert(!ps.children.top().empty());
                ps.node_data_.top()
                    .current_field_value->second->assign(
                        sfnode(ps.children.top().front()));
            }
            actions_.ps.top().children.pop();
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfnode;

    struct on_mfnode_t {
        explicit on_mfnode_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()() const
        {
            assert(!this->actions_.ps.empty());
            assert(!this->actions_.ps.top().node_data_.empty());
            assert(!this->actions_.ps.top().children.empty());
            this->actions_.ps.top().node_data_.top()
                .current_field_value->second
                ->assign(mfnode(this->actions_.ps.top().children.top()));
            this->actions_.ps.top().children.pop();
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfnode;

    struct on_sfbool_t {
        explicit on_sfbool_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const bool val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfbool(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfbool;

    struct on_sfcolor_t {
        explicit on_sfcolor_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const color & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfcolor(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfcolor;

    struct on_mfcolor_t {
        explicit on_mfcolor_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<color> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mfcolor(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfcolor;

    struct on_sffloat_t {
        explicit on_sffloat_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const float val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sffloat(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sffloat;

    struct on_mffloat_t {
        explicit on_mffloat_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<float> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mffloat(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mffloat;

    struct on_sfimage_t {
        explicit on_sfimage_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const image & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfimage(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfimage;

    struct on_sfint32_t {
        explicit on_sfint32_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const int32 val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfint32(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfint32;

    struct on_mfint32_t {
        explicit on_mfint32_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<int32> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mfint32(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfint32;

    struct on_sfrotation_t {
        explicit on_sfrotation_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const rotation & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfrotation(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfrotation;

    struct on_mfrotation_t {
        explicit on_mfrotation_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<rotation> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mfrotation(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfrotation;

    struct on_sfstring_t {
        explicit on_sfstring_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::string & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfstring(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfstring;

    struct on_mfstring_t {
        explicit on_mfstring_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<std::string> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mfstring(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfstring;

    struct on_sftime_t {
        explicit on_sftime_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const double val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sftime(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sftime;

    struct on_mftime_t {
        explicit on_mftime_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<double> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mftime(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mftime;

    struct on_sfvec2f_t {
        explicit on_sfvec2f_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const vec2f & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfvec2f(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfvec2f;

    struct on_mfvec2f_t {
        explicit on_mfvec2f_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<vec2f> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mfvec2f(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfvec2f;

    struct on_sfvec3f_t {
        explicit on_sfvec3f_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const vec3f & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfvec3f(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfvec3f;

    struct on_mfvec3f_t {
        explicit on_mfvec3f_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<vec3f> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mfvec3f(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfvec3f;

    struct route {
        std::string from;
        std::string eventout;
        std::string to;
        std::string eventin;

        route(const std::string & from, const std::string & eventout,
              const std::string & to, const std::string & eventin):
            from(from),
            eventout(eventout),
            to(to),
            eventin(eventin)
        {}
    };

    struct node_data {
        //
        // This is a multimap because an exposedField might have multiple
        // entries (i.e., for the corresponding eventIn, field, and eventOut).
        //
        typedef std::multimap<std::string, std::string> is_map_t;

        boost::shared_ptr<node_type> type;
        std::string node_name_id;
        initial_value_map initial_values;
        initial_value_map::value_type * current_field_value;
        node_interface_set script_interfaces;
        is_map_t is_map;

        node_data():
            current_field_value(0)
        {}

        node_data(const node_data & nd):
            type(nd.type),
            node_name_id(nd.node_name_id),
            initial_values(nd.initial_values),
            current_field_value(0),
            script_interfaces(nd.script_interfaces),
            is_map(nd.is_map)
        {
            if (nd.current_field_value) {
                initial_value_map::iterator pos =
                    this->initial_values.find(nd.current_field_value->first);
                assert(pos != this->initial_values.end());
                this->current_field_value = &(*pos);
            }
        }

        node_data & operator=(const node_data & nd)
        {
            node_data temp(nd);
            this->swap(temp);
            return *this;
        }

        void swap(node_data & nd) OPENVRML_NOTHROW
        {
            this->type.swap(nd.type);
            this->node_name_id.swap(nd.node_name_id);
            this->initial_values.swap(nd.initial_values);
            std::swap(this->current_field_value, nd.current_field_value);
            this->script_interfaces.swap(nd.script_interfaces);
            this->is_map.swap(nd.is_map);
        }
    };

    struct parse_scope {
        typedef std::vector<boost::intrusive_ptr<node> > children_t;

        std::string proto_node_type_id;
        boost::shared_ptr<openvrml::scope> scope;
        node_interface_set proto_interfaces;
        local::proto_node_metatype::default_value_map_t proto_default_values;
        local::proto_node_metatype::is_map_t proto_is_map;
        std::vector<route> routes;
        std::stack<children_t> children;
        std::stack<node_data> node_data_;
    };

    //
    // We push a parse_scope onto the stack
    // * at the scene root
    std::stack<parse_scope> ps;

private:
    const std::string uri_;
    const openvrml::scene & scene_;
    std::vector<boost::intrusive_ptr<openvrml::node> > & nodes_;
};

struct OPENVRML_LOCAL openvrml::browser::x3d_vrml_parse_actions :
    openvrml::browser::vrml97_parse_actions {

    x3d_vrml_parse_actions(
        const std::string & uri,
        const openvrml::scene & scene,
        std::vector<boost::intrusive_ptr<openvrml::node> > & nodes,
        std::map<std::string, std::string> & meta):
        openvrml::browser::vrml97_parse_actions(uri, scene, nodes),
        on_profile_statement(*this),
        on_component_statement(*this),
        on_meta_statement(*this),
        on_sfcolorrgba(*this),
        on_sfdouble(*this),
        on_sfvec2d(*this),
        on_sfvec3d(*this),
        on_mfbool(*this),
        on_mfcolorrgba(*this),
        on_mfimage(*this),
        on_mfvec2d(*this),
        on_mfvec3d(*this),
        meta_(meta)
    {}

    struct on_profile_statement_t {
        explicit on_profile_statement_t(x3d_vrml_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::string & /* profile_id */) const
        {}

    private:
        x3d_vrml_parse_actions & actions_;
    } on_profile_statement;

    struct on_component_statement_t {
        explicit on_component_statement_t(x3d_vrml_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::string & /* component_id */,
                        int32 /* level */) const
        {}

    private:
        x3d_vrml_parse_actions & actions_;
    } on_component_statement;

    struct on_meta_statement_t {
        explicit on_meta_statement_t(x3d_vrml_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::string & name,
                        const std::string & value) const
        {
            this->actions_.meta_[name] = value;
        }

    private:
        x3d_vrml_parse_actions & actions_;
    } on_meta_statement;

    struct on_import_statement_t {
        void operator()(const std::string & /* inline_node_name_id */,
                        const std::string & /* exported_node_name_id */,
                        const std::string & /* imported_node_name_id */) const
        {}
    } on_import_statement;

    struct on_export_statement_t {
        void operator()(const std::string & /* node_name_id */,
                        const std::string & /* exported_node_name_id */) const
        {}
    } on_export_statement;

    struct on_sfcolorrgba_t {
        explicit on_sfcolorrgba_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const color_rgba & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfcolorrgba(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfcolorrgba;

    struct on_sfdouble_t {
        explicit on_sfdouble_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const double val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfdouble(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfdouble;

    struct on_sfvec2d_t {
        explicit on_sfvec2d_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const vec2d & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfvec2d(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfvec2d;

    struct on_sfvec3d_t {
        explicit on_sfvec3d_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const vec3d & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(sfvec3d(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_sfvec3d;

    struct on_mfbool_t {
        explicit on_mfbool_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<bool> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mfbool(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfbool;

    struct on_mfcolorrgba_t {
        explicit on_mfcolorrgba_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<color_rgba> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mfcolorrgba(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfcolorrgba;

    struct on_mfimage_t {
        explicit on_mfimage_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<image> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mfimage(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfimage;

    struct on_mfvec2d_t {
        explicit on_mfvec2d_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<vec2d> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mfvec2d(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfvec2d;

    struct on_mfvec3d_t {
        explicit on_mfvec3d_t(vrml97_parse_actions & actions):
            actions_(actions)
        {}

        void operator()(const std::vector<vec3d> & val) const
        {
            assert(!actions_.ps.empty());
            assert(!actions_.ps.top().node_data_.empty());
            actions_.ps.top().node_data_.top()
                .current_field_value->second->assign(mfvec3d(val));
        }

    private:
        vrml97_parse_actions & actions_;
    } on_mfvec3d;

private:
    std::map<std::string, std::string> & meta_;
};

namespace {

    struct OPENVRML_LOCAL parse_error {
        size_t line, column;
        std::string message;

        parse_error(): line(0), column(0) {}
    };

    struct OPENVRML_LOCAL error_handler {
        error_handler(openvrml::browser & b, parse_error & error):
            browser_(b),
            error_(error)
        {}

        template <typename ScannerT, typename ErrorT>
        boost::spirit::error_status<> operator()(const ScannerT & scan,
                                                 const ErrorT & err) const
        {
            using std::endl;
            using std::string;
            using boost::spirit::error_status;
            using boost::spirit::file_position;

            const file_position fpos = err.where.get_position();

            //
            // Warnings we want to spew directly to the browser.
            //
            if (   err.descriptor == openvrml::rotation_axis_not_normalized
                || err.descriptor == openvrml::eventin_deprecated
                || err.descriptor == openvrml::eventout_deprecated
                || err.descriptor == openvrml::exposedfield_deprecated
                || err.descriptor == openvrml::field_deprecated) {
                std::ostringstream warn;
                warn << fpos.file << ':' << fpos.line << ':' << fpos.column
                     << ": warning: "
                     << x3d_vrml_parse_error_msg(err.descriptor);
                this->browser_.err(warn.str());
                scan.first = err.where;
                return error_status<>(error_status<>::accept, 0);
            }

            //
            // Errors presumably halt parsing and result in a failure.  We
            // need enough information to create an invalid_vrml exception in
            // parse_vrml.
            //
            this->error_.line = fpos.line;
            this->error_.column = fpos.column;
            this->error_.message = x3d_vrml_parse_error_msg(err.descriptor);

            return error_status<>(error_status<>::fail);
        }

    private:
        openvrml::browser & browser_;
        parse_error & error_;
    };
}

/**
 * @internal
 *
 * @brief Parse a VRML stream.
 *
 * @param[in,out] in    input stream.
 * @param[in]     uri   URI associated with @p in.
 * @param[in]     type  MIME media type of the data to be read from @p in.
 * @param[in]     scene a @c scene.
 * @param[out]    nodes the root @c node%s.
 * @param[out]    meta  the @c scene metadata.
 *
 * @exception openvrml::bad_media_type
 * @exception openvrml::invalid_vrml
 */
OPENVRML_LOCAL void
openvrml::browser::
parse_vrml(std::istream & in,
           const std::string & uri,
           const std::string & type,
           const openvrml::scene & scene,
           std::vector<boost::intrusive_ptr<openvrml::node> > & nodes,
           std::map<std::string, std::string> & meta)
{
    using std::istream;
    using std::istreambuf_iterator;
    using boost::algorithm::iequals;
    using boost::spirit::multi_pass;
    using boost::spirit::make_multi_pass;
    using boost::spirit::position_iterator;

    typedef multi_pass<istreambuf_iterator<char> > multi_pass_iterator_t;
    typedef istream::char_type char_t;

    vrml97_skip_grammar skip_g;

    if (iequals(type, vrml_media_type) || iequals(type, x_vrml_media_type)) {
        multi_pass_iterator_t
            in_begin(make_multi_pass(istreambuf_iterator<char_t>(in))),
            in_end(make_multi_pass(istreambuf_iterator<char_t>()));

        typedef position_iterator<multi_pass_iterator_t> iterator_t;

        iterator_t first(in_begin, in_end, uri), last;

        parse_error error;
        error_handler handler(scene.browser(), error);
        vrml97_parse_actions actions(uri, scene, nodes);
        vrml97_grammar<vrml97_parse_actions, error_handler>
            g(actions, handler);

        BOOST_SPIRIT_DEBUG_NODE(skip_g);
        BOOST_SPIRIT_DEBUG_NODE(g);

        if (!parse(first, last, g, skip_g).full) {
            throw openvrml::invalid_vrml(uri,
                                         error.line,
                                         error.column,
                                         error.message);
        }
    } else if (iequals(type, x3d_vrml_media_type)) {
        multi_pass_iterator_t
            in_begin(make_multi_pass(istreambuf_iterator<char_t>(in))),
            in_end(make_multi_pass(istreambuf_iterator<char_t>()));

        typedef position_iterator<multi_pass_iterator_t> iterator_t;

        iterator_t first(in_begin, in_end, uri), last;

        parse_error error;
        error_handler handler(scene.browser(), error);
        x3d_vrml_parse_actions actions(uri, scene, nodes, meta);
        x3d_vrml_grammar<x3d_vrml_parse_actions, error_handler>
            g(actions, handler);

        BOOST_SPIRIT_DEBUG_NODE(skip_g);
        BOOST_SPIRIT_DEBUG_NODE(g);

        if (!parse(first, last, g, skip_g).full) {
            throw openvrml::invalid_vrml(uri,
                                         error.line,
                                         error.column,
                                         error.message);
        }
    } else {
        throw bad_media_type(type);
    }
}

struct OPENVRML_LOCAL openvrml::browser::externproto_node_metatype::load_proto {
    load_proto(externproto_node_metatype & externproto_class,
               const openvrml::scene & scene,
               const std::vector<std::string> & alt_uris):
        externproto_node_metatype_(&externproto_class),
        scene_(&scene),
        alt_uris_(alt_uris)
    {}

    void operator()() const OPENVRML_NOTHROW
    {
        try {
            using openvrml::unreachable_url;
            try {
                using namespace openvrml;
                using std::auto_ptr;
                using std::ostringstream;
                using std::string;
                using std::vector;
                using boost::dynamic_pointer_cast;
                using boost::shared_ptr;
                using local::uri;

                scope_guard guard =
                    make_obj_guard(
                        *this->externproto_node_metatype_,
                        &externproto_node_metatype::
                            clear_externproto_node_types);
                boost::ignore_unused_variable_warning(guard);

                auto_ptr<resource_istream> in =
                    this->scene_->get_resource(this->alt_uris_);
                if (!(*in)) { throw unreachable_url(); }

                //
                // We don't actually do anything with these; but the parser
                // wants them.
                //
                vector<boost::intrusive_ptr<node> > nodes;
                std::map<string, string> meta;

                parse_vrml(*in, in->url(), in->type(),
                           *this->scene_, nodes, meta);

                shared_ptr<openvrml::local::proto_node_metatype>
                    proto_node_metatype;
                for (vector<string>::const_iterator alt_uri =
                         this->alt_uris_.begin();
                     (alt_uri != this->alt_uris_.end())
                         && !proto_node_metatype;
                     ++alt_uri) {
                    const uri absolute_uri = !relative(uri(*alt_uri))
                        ? uri(*alt_uri)
                        : this->scene_->url().empty()
                            ? create_file_url(uri(*alt_uri))
                            : resolve_against(uri(*alt_uri), 
                                              uri(this->scene_->url()));

                    const shared_ptr<openvrml::node_metatype> node_metatype =
                        this->scene_->browser().node_metatype(
                            node_metatype_id(absolute_uri));

                    proto_node_metatype =
                        dynamic_pointer_cast<
                            openvrml::local::proto_node_metatype>(
                                node_metatype);
                }

                if (!proto_node_metatype) {
                    ostringstream err_msg;
                    err_msg << "no PROTO definition at <" << in->url()
                            << ">";
                    this->scene_->browser().err(err_msg.str());
                    return;
                }

                this->externproto_node_metatype_
                    ->set_proto_node_metatype(proto_node_metatype);

            } catch (std::exception & ex) {
                this->scene_->browser().err(ex.what());
                throw unreachable_url();
            } catch (...) {
                //
                // The implementation of resource_istream is provided by
                // the user; and unfortunately, operations on it could
                // throw anything.
                //
                throw unreachable_url();
            }
        } catch (std::exception & ex) {
            this->scene_->browser().err(ex.what());
        }
    }

private:
    externproto_node_metatype * externproto_node_metatype_;
    const openvrml::scene * scene_;
    std::vector<std::string> alt_uris_;
};

/**
 * @brief Construct.
 *
 * @param[in] id                            the @c node_metatype_id for the
 *                                          @c EXTERNPROTO.
 * @param[in] scene                         the @c scene in which the
 *                                          @c EXTERNPROTO occurs.
 * @param[in] uris                          the list of alternative
 *                                          implementation identifiers.
 * @param[in,out] load_proto_thread_group   the thread group used for loading
 *                                          @c EXTERNPROTO implementations.
 *
 * @exception boost::thread_resource_error  if a new thread of execution
 *                                          cannot be started.
 * @exception std::bad_alloc                if memory allocation fails.
 */
openvrml::browser::externproto_node_metatype::
externproto_node_metatype(const openvrml::node_metatype_id & id,
                          const openvrml::scene & scene,
                          const std::vector<std::string> & uris,
                          boost::thread_group & load_proto_thread_group)
    OPENVRML_THROW2(boost::thread_resource_error, std::bad_alloc):
    node_metatype(id, scene.browser()),
                  externproto_node_types_cleared_(false),
                  load_proto_thread_(
                      load_proto_thread_group.create_thread(
                          boost::function0<void>(
                              load_proto(*this, scene, uris))))
{}

/**
 * @brief Destroy.
 */
openvrml::browser::externproto_node_metatype::~externproto_node_metatype()
    OPENVRML_NOTHROW
{}

const boost::shared_ptr<openvrml::node_type>
openvrml::browser::externproto_node_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    boost::mutex::scoped_lock lock(this->mutex_);

    using boost::shared_ptr;

    shared_ptr<openvrml::local::proto_node_metatype> shared;
    if ((shared = this->proto_node_metatype_.lock())) {
        return shared->create_type(id, interfaces);
    }

    const shared_ptr<externproto_node_type> node_type(
        new externproto_node_type(this->shared_from_this(),
                                  id,
                                  interfaces));

    this->externproto_node_types_.push_back(node_type);

    return node_type;
}

void openvrml::browser::externproto_node_metatype::do_shutdown(double)
    OPENVRML_NOTHROW
{
    this->load_proto_thread_->join();
}

void
openvrml::browser::externproto_node_metatype::
set_proto_node_metatype(
    const boost::weak_ptr<openvrml::local::proto_node_metatype> & proto_node_metatype)
    OPENVRML_THROW1(std::bad_alloc)
{
    boost::mutex::scoped_lock lock(this->mutex_);

    using boost::shared_ptr;
    using boost::static_pointer_cast;

    this->proto_node_metatype_ = proto_node_metatype;

    //
    // Now that we have a proto_node_metatype, we need to tell all the
    // externproto_node_types we've created so that they can in turn
    // tell the externproto_nodes they've created.
    //
    const shared_ptr<openvrml::local::proto_node_metatype> shared_proto_node_metatype =
        proto_node_metatype.lock();
    assert(shared_proto_node_metatype);
    for (externproto_node_types::const_iterator node_type =
             this->externproto_node_types_.begin();
         node_type != this->externproto_node_types_.end();
         ++node_type) {
        assert(!node_type->expired());
        node_type->lock()->set_proto_node_type(*shared_proto_node_metatype);
    }
}

void openvrml::browser::externproto_node_metatype::clear_externproto_node_types()
    OPENVRML_NOTHROW
{
    boost::mutex::scoped_lock lock(this->mutex_);

    using boost::shared_ptr;

    for (externproto_node_types::const_iterator node_type =
             this->externproto_node_types_.begin();
         node_type != this->externproto_node_types_.end();
         ++node_type) {
        assert(!node_type->expired());
        node_type->lock()->clear_externproto_nodes();
    }

    this->externproto_node_types_.clear();
    this->externproto_node_types_cleared_ = true;
}


openvrml::browser::externproto_node_type::externproto_node_type(
    const boost::shared_ptr<const externproto_node_metatype> & c,
    const std::string & id,
    const openvrml::node_interface_set & interfaces)
    OPENVRML_THROW1(std::bad_alloc):
    node_type(*c, id),
    node_metatype_(c),
    interfaces_(interfaces)
{}

openvrml::browser::externproto_node_type::~externproto_node_type()
    OPENVRML_NOTHROW
{}

void
openvrml::browser::externproto_node_type::
set_proto_node_type(openvrml::local::proto_node_metatype & proto_node_metatype)
    OPENVRML_THROW1(std::bad_alloc)
{
    boost::mutex::scoped_lock lock(this->mutex_);

    assert(!this->proto_node_type_);

    using boost::static_pointer_cast;

    this->proto_node_type_ =
        proto_node_metatype.create_type(this->id(), this->interfaces_);

    for (externproto_nodes::const_iterator node =
             this->externproto_nodes_.begin();
         node != this->externproto_nodes_.end();
         ++node) {
        (*node)->set_proto_node(*this->proto_node_type_);
    }
}

void openvrml::browser::externproto_node_type::clear_externproto_nodes()
    OPENVRML_NOTHROW
{
    boost::mutex::scoped_lock lock(this->mutex_);
    this->externproto_nodes_.clear();
}

const openvrml::node_interface_set &
openvrml::browser::externproto_node_type::do_interfaces() const throw()
{
    return this->interfaces_;
}

const boost::intrusive_ptr<openvrml::node>
openvrml::browser::externproto_node_type::do_create_node(
    const boost::shared_ptr<openvrml::scope> & scope,
    const openvrml::initial_value_map & initial_values) const
    OPENVRML_THROW3(openvrml::unsupported_interface, std::bad_cast,
                    std::bad_alloc)
{
    //
    // externproto_node_metatype::mutex_ must be locked first.
    // externproto_node_metatype::clear_externproto_node_types must not be
    // initiated while we're doing this.
    // externproto_node_metatype::externproto_node_types_cleared_ is checked
    // later in this function and attempting to lock
    // externproto_node_metatype::mutex_ at that point creates a race
    // condition.
    //
    boost::mutex::scoped_lock
        lock_externproto_node_metatype(
            static_cast<const externproto_node_metatype &>(this->metatype())
            .mutex_),
        lock(this->mutex_);

    if (this->proto_node_type_) {
        return this->proto_node_type_->create_node(scope, initial_values);
    }

    const boost::intrusive_ptr<externproto_node> result(
        new externproto_node(*this, scope, initial_values));

    const externproto_node_metatype & node_metatype =
        static_cast<const externproto_node_metatype &>(this->metatype());
    if (!node_metatype.externproto_node_types_cleared_) {
        this->externproto_nodes_.push_back(result);
    } else {
        assert(this->externproto_nodes_.empty());
    }

    return result;
}


template <typename FieldValue>
openvrml::externproto_node::externproto_exposedfield<FieldValue>::
externproto_exposedfield(externproto_node & node,
                         const FieldValue & initial_value) OPENVRML_NOTHROW:
    node_event_listener(node),
    openvrml::event_emitter(static_cast<field_value &>(*this)),
    FieldValue(initial_value),
    proto_eventin<FieldValue>(node),
    proto_eventout<FieldValue>(node, initial_value)
{}

template <typename FieldValue>
openvrml::externproto_node::externproto_exposedfield<FieldValue>::
~externproto_exposedfield() OPENVRML_NOTHROW
{}

template <typename FieldValue>
std::auto_ptr<openvrml::field_value>
openvrml::externproto_node::externproto_exposedfield<FieldValue>::
do_clone() const
    OPENVRML_THROW1(std::bad_alloc)
{
    using boost::polymorphic_downcast;
    return std::auto_ptr<field_value>(
        new externproto_exposedfield<FieldValue>(
            *polymorphic_downcast<externproto_node *>(&this->node()),
            *this));
}

template <typename FieldValue>
void
openvrml::externproto_node::externproto_exposedfield<FieldValue>::
do_process_event(const FieldValue & value, const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    static_cast<externproto_node &>(this->node()).received_event_.insert(this);
    static_cast<FieldValue &>(*this) = value;
    this->proto_eventin<FieldValue>::do_process_event(value, timestamp);
    this->node().modified(true);
    node::emit_event(*this, timestamp);
}

const boost::shared_ptr<openvrml::field_value>
openvrml::externproto_node::
create_exposedfield(externproto_node & node, field_value::type_id type)
    OPENVRML_THROW1(std::bad_alloc)
{
    using boost::mpl::for_each;
    using openvrml::local::field_value_types;

    boost::shared_ptr<field_value> result;
    for_each<field_value_types>(externproto_exposedfield_creator(node,
                                                                 type,
                                                                 result));
    assert(result.get());
    return result;
}

openvrml::externproto_node::
externproto_node(const browser::externproto_node_type & type,
                 const boost::shared_ptr<openvrml::scope> & scope,
                 const initial_value_map & initial_values)
    OPENVRML_THROW1(std::bad_alloc):
    abstract_proto_node(type, scope),
    initial_values_(initial_values)
{
    for (node_interface_set::const_iterator interface_ =
             type.interfaces().begin();
         interface_ != type.interfaces().end();
         ++interface_)
    {
        using boost::shared_ptr;
        using boost::dynamic_pointer_cast;

        bool succeeded = false;
        std::auto_ptr<openvrml::field_value> field_auto_ptr;
        shared_ptr<openvrml::event_listener> interface_eventin;
        shared_ptr<openvrml::event_emitter> interface_eventout;
        shared_ptr<openvrml::field_value> interface_field;
        switch (interface_->type) {
        case node_interface::eventin_id:
            succeeded = this->eventin_map
                .insert(make_pair(interface_->id,
                                  create_eventin(interface_->field_type,
                                                 *this)))
                .second;
            break;
        case node_interface::eventout_id:
            succeeded = this->eventout_map
                .insert(make_pair(interface_->id,
                                  create_eventout(interface_->field_type,
                                                  *this)))
                .second;
            break;
        case node_interface::exposedfield_id:
            interface_field = create_exposedfield(*this,
                                                  interface_->field_type);
            succeeded = this->field_map_
                .insert(make_pair(interface_->id, interface_field)).second;
            assert(succeeded);

            interface_eventin =
                dynamic_pointer_cast<openvrml::event_listener>(
                    interface_field);
            succeeded = this->eventin_map
                .insert(make_pair("set_" + interface_->id,
                                  interface_eventin))
                .second;
            assert(succeeded);

            interface_eventout =
                dynamic_pointer_cast<openvrml::event_emitter>(
                    interface_eventin);
            succeeded = this->eventout_map
                .insert(make_pair(interface_->id + "_changed",
                                  interface_eventout))
                .second;
            break;
        case node_interface::field_id:
            field_auto_ptr = field_value::create(interface_->field_type);
            succeeded = this->field_map_
                .insert(
                    make_pair(interface_->id,
                              shared_ptr<field_value>(field_auto_ptr)))
                .second;
            break;
        case node_interface::invalid_type_id:
            assert(false
                   && "got node_interface::invalid_type_id for "
                   "interface_->type");
        }
        assert(succeeded);
    }

    //
    // Set the initial values.
    //
    for (initial_value_map::const_iterator map_entry =
             initial_values.begin();
         map_entry != initial_values.end();
         ++map_entry) {
        const field_map::const_iterator pos =
            this->field_map_.find(map_entry->first);
        if (pos == this->field_map_.end()) {
            throw unsupported_interface(this->type(), map_entry->first);
        }
        pos->second->assign(*map_entry->second);
    }
}

openvrml::externproto_node::~externproto_node() OPENVRML_NOTHROW
{}

bool openvrml::externproto_node::modified() const
{
    return this->proto_node_
        ? this->proto_node_->modified()
        : false;
}

void
openvrml::externproto_node::
set_proto_node(node_type & type)
    OPENVRML_THROW1(std::bad_alloc)
{
    using boost::static_pointer_cast;

    //
    // Any exposedFields that received events add to/override what's in the
    // initial_values_ map.
    //
    for (field_map::const_iterator field = this->field_map_.begin();
         field != this->field_map_.end();
         ++field) {
        std::set<openvrml::field_value *>::const_iterator pos =
            this->received_event_.find(field->second.get());
        if (pos != this->received_event_.end()) {
            this->initial_values_[field->first] = field->second;
        }
    }
    this->received_event_.clear();

    this->proto_node_ = type.create_node(this->scope_, this->initial_values_);
    this->initial_values_.clear(); // No longer need these.

    for (eventin_map_t::const_iterator map_entry =
             this->eventin_map.begin();
         map_entry != this->eventin_map.end();
         ++map_entry) {
        using boost::mpl::for_each;
        using openvrml::local::field_value_types;

        openvrml::event_listener & eventin =
            this->proto_node_->event_listener(map_entry->first);
        for_each<field_value_types>(eventin_is(map_entry->second->type(),
                                               eventin,
                                               *map_entry->second));
    }

    for (eventout_map_t::const_iterator map_entry =
             this->eventout_map.begin();
         map_entry != this->eventout_map.end();
         ++map_entry) {
        using boost::mpl::for_each;
        using openvrml::local::field_value_types;

        openvrml::event_emitter & eventout =
            this->proto_node_->event_emitter(map_entry->first);
        for_each<field_value_types>(
            eventout_is(map_entry->second->value().type(),
                        eventout,
                        *map_entry->second));
    }

    if (this->scene()) {
        this->proto_node_->initialize(*this->scene(),
                                      browser::current_time());
    }
}

void openvrml::externproto_node::do_initialize(const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    if (this->proto_node_) {
        this->proto_node_->initialize(*this->scene(), timestamp);
    }
}

const openvrml::field_value &
openvrml::externproto_node::do_field(const std::string & id) const
    OPENVRML_THROW1(unsupported_interface)
{
    if (this->proto_node_) {
        return this->proto_node_->do_field(id);
    }

    field_map::const_iterator pos = this->field_map_.find(id);
    if (pos == this->field_map_.end()) {
        throw unsupported_interface(this->type(),
                                    node_interface::field_id,
                                    id);
    }
    assert(pos->second);
    return *pos->second;
}

openvrml::event_listener &
openvrml::externproto_node::do_event_listener(const std::string & id)
    OPENVRML_THROW1(unsupported_interface)
{
    if (this->proto_node_) {
        return this->proto_node_->do_event_listener(id);
    }

    eventin_map_t::const_iterator pos = this->eventin_map.find(id);
    if (pos == this->eventin_map.end()) {
        throw unsupported_interface(this->type(),
                                    node_interface::eventin_id,
                                    id);
    }
    assert(pos->second);
    return *pos->second;
}

openvrml::event_emitter &
openvrml::externproto_node::do_event_emitter(const std::string & id)
    OPENVRML_THROW1(unsupported_interface)
{
    if (this->proto_node_) {
        return this->proto_node_->do_event_emitter(id);
    }

    eventout_map_t::const_iterator pos = this->eventout_map.find(id);
    if (pos == this->eventout_map.end()) {
        throw unsupported_interface(this->type(),
                                    node_interface::eventout_id,
                                    id);
    }
    assert(pos->second);
    return *pos->second;
}

void openvrml::externproto_node::do_shutdown(const double timestamp)
    OPENVRML_NOTHROW
{
    if (this->proto_node_) {
        this->proto_node_->shutdown(timestamp);
    }
}

/**
 * @brief Cast to a @c script_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c script_node, or 0 otherwise.
 */
openvrml::script_node * openvrml::externproto_node::to_script()
    OPENVRML_NOTHROW
{
    return node_cast<script_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to an @c appearance_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is an @c appearance_node, or 0 otherwise.
 */
openvrml::appearance_node *
openvrml::externproto_node::to_appearance() OPENVRML_NOTHROW
{
    return node_cast<appearance_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c bounded_volume_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c bounded_volume_node, or 0 otherwise.
 */
openvrml::bounded_volume_node *
openvrml::externproto_node::to_bounded_volume()
    OPENVRML_NOTHROW
{
    return node_cast<bounded_volume_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c child_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c child_node, or 0 otherwise.
 */
openvrml::child_node *
openvrml::externproto_node::to_child() OPENVRML_NOTHROW
{
    return node_cast<child_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c color_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c color_node, or 0 otherwise.
 */
openvrml::color_node *
openvrml::externproto_node::to_color() OPENVRML_NOTHROW
{
    return node_cast<color_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c color_rgba_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c color_rgba_node, or 0 otherwise.
 */
openvrml::color_rgba_node *
openvrml::externproto_node::to_color_rgba()
    OPENVRML_NOTHROW
{
    return node_cast<color_rgba_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c coordinate_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c coordinate_node, or 0 otherwise.
 */
openvrml::coordinate_node *
openvrml::externproto_node::to_coordinate() OPENVRML_NOTHROW
{
    return node_cast<coordinate_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c font_style_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c font_style_node, or 0 otherwise.
 */
openvrml::font_style_node *
openvrml::externproto_node::to_font_style() OPENVRML_NOTHROW
{
    return node_cast<font_style_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c geometry_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c geometry_node, or 0 otherwise.
 */
openvrml::geometry_node * openvrml::externproto_node::to_geometry()
    OPENVRML_NOTHROW
{
    return node_cast<geometry_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c grouping_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c grouping_node, or 0 otherwise.
 */
openvrml::grouping_node * openvrml::externproto_node::to_grouping()
    OPENVRML_NOTHROW
{
    return node_cast<grouping_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c light_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c light_node, or 0 otherwise.
 */
openvrml::light_node * openvrml::externproto_node::to_light()
    OPENVRML_NOTHROW
{
    return node_cast<light_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c material_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c material_node, or 0 otherwise.
 */
openvrml::material_node * openvrml::externproto_node::to_material()
    OPENVRML_NOTHROW
{
    return node_cast<material_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c navigation_info_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c navigation_info_node, or 0 otherwise.
 */
openvrml::navigation_info_node *
openvrml::externproto_node::to_navigation_info()
    OPENVRML_NOTHROW
{
    return node_cast<navigation_info_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c normal_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c normal_node, or 0 otherwise.
 */
openvrml::normal_node * openvrml::externproto_node::to_normal()
    OPENVRML_NOTHROW
{
    return node_cast<normal_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c pointing_device_sensor_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c pointing_device_sensor_node, or 0 otherwise.
 */
openvrml::pointing_device_sensor_node *
openvrml::externproto_node::to_pointing_device_sensor()
    OPENVRML_NOTHROW
{
    return node_cast<pointing_device_sensor_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c scoped_light_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c scoped_light_node, or 0 otherwise.
 */
openvrml::scoped_light_node *
openvrml::externproto_node::to_scoped_light() OPENVRML_NOTHROW
{
    return node_cast<scoped_light_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c sound_source_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c sound_source_node, or 0 otherwise.
 */
openvrml::sound_source_node *
openvrml::externproto_node::to_sound_source() OPENVRML_NOTHROW
{
    return node_cast<sound_source_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c texture_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c texture_node, or 0 otherwise.
 */
openvrml::texture_node * openvrml::externproto_node::to_texture()
    OPENVRML_NOTHROW
{
    return node_cast<texture_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c texture_coordinate_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c texture_coordinate_node, or 0 otherwise.
 */
openvrml::texture_coordinate_node *
openvrml::externproto_node::to_texture_coordinate()
    OPENVRML_NOTHROW
{
    return node_cast<texture_coordinate_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c texture_transform_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c texture_transform_node, or 0 otherwise.
 */
openvrml::texture_transform_node *
openvrml::externproto_node::to_texture_transform()
    OPENVRML_NOTHROW
{
    return node_cast<texture_transform_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c time_dependent_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c time_dependent_node, or 0 otherwise.
 */
openvrml::time_dependent_node *
openvrml::externproto_node::to_time_dependent()
    OPENVRML_NOTHROW
{
    return node_cast<time_dependent_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c transform_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c transform_node, or 0 otherwise.
 */
openvrml::transform_node * openvrml::externproto_node::to_transform()
    OPENVRML_NOTHROW
{
    return node_cast<transform_node *>(this->proto_node_.get());
}

/**
 * @brief Cast to a @c viewpoint_node.
 *
 * @return a pointer to the first node in the implementation if that node
 *         is a @c viewpoint_node, or 0 otherwise.
 */
openvrml::viewpoint_node * openvrml::externproto_node::to_viewpoint()
    OPENVRML_NOTHROW
{
    return node_cast<viewpoint_node *>(this->proto_node_.get());
}


// Max time in seconds between updates. Make this user
// setable to balance performance with cpu usage.
#ifndef DEFAULT_DELTA
#define DEFAULT_DELTA 0.5
#endif

/**
 * @namespace openvrml
 *
 * @brief The OpenVRML Runtime Library
 */

/**
 * @brief VRML MIME media type.
 */
const char openvrml::vrml_media_type[11] = "model/vrml";

/**
 * @brief Experimental VRML MIME media type.
 */
const char openvrml::x_vrml_media_type[15] = "x-world/x-vrml";

/**
 * @brief X3D VRML MIME media type.
 */
const char openvrml::x3d_vrml_media_type[15] = "model/x3d+vrml";

/**
 * @class openvrml::resource_istream openvrml/browser.h
 *
 * @brief An abstract input stream for network resources.
 *
 * @c resource_istream inherits @c std::istream, adding functions to get the
 * URI and the MIME content type associated with the stream.  Users of the
 * library must provide an implementation of this class, to be returned from
 * @c openvrml::resource_fetcher::do_get_resource.
 *
 * @sa openvrml::resource_fetcher::do_get_resource(const std::string &)
 */

/**
 * @brief Construct.
 *
 * @param[in] streambuf a stream buffer.
 */
openvrml::resource_istream::resource_istream(std::streambuf * streambuf):
    std::istream(streambuf)
{}

/**
 * @brief Destroy.
 */
openvrml::resource_istream::~resource_istream()
{}

/**
 * @brief Get the URL associated with the stream.
 *
 * This function delegates to @c #do_url.
 *
 * @return the URL associated with the stream.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
const std::string openvrml::resource_istream::url() const
    OPENVRML_THROW1(std::bad_alloc)
{
    return this->do_url();
}

/**
 * @fn const std::string openvrml::resource_istream::do_url() const
 *
 * @brief Get the URL associated with the stream.
 *
 * @return the URL associated with the stream.
 */

/**
 * @brief Get the MIME content type associated with the stream.
 *
 * This function delegates to @c #do_type.
 *
 * @return the MIME content type associated with the stream.
 */
const std::string openvrml::resource_istream::type() const
    OPENVRML_THROW1(std::bad_alloc)
{
    return this->do_type();
}

/**
 * @fn const std::string openvrml::resource_istream::do_type() const
 *
 * @brief Get the MIME content type associated with the stream.
 *
 * @return the MIME content type associated with the stream.
 */

/**
 * @brief Indicates whether data is available to be read from the stream.
 *
 * This function delegates to @c #do_data_available.
 *
 * @return @c true if there is data in the stream buffer to be read; @c false
 *         otherwise.
 */
bool openvrml::resource_istream::data_available() const OPENVRML_NOTHROW
{
    return this->do_data_available();
}

/**
 * @fn bool openvrml::resource_istream::do_data_available() const
 *
 * @brief Indicates whether data is available to be read from the stream.
 *
 * @return @c true if there is data in the stream buffer to be read; @c false
 *         otherwise.
 */


/**
 * @class openvrml::resource_fetcher
 *
 * @brief An abstract factory for @c resource_istream%s.
 *
 * A concrete implementation of this interface must be passed to
 * <code>browser</code>'s constructor.
 *
 * @sa openvrml::browser
 */

/**
 * @brief Destroy.
 */
openvrml::resource_fetcher::~resource_fetcher() OPENVRML_NOTHROW
{}

/**
 * @brief Fetch a network resource.
 *
 * @param[in] uri   a Uniform Resource Identifier.
 *
 * This function delegates to @c resource_fetcher::do_get_resource.
 *
 * @return the requested resource as a stream.
 *
 * @exception std::invalid_argument if @p uri is malformed or in an
 *                                  unsupported format.
 */
std::auto_ptr<openvrml::resource_istream>
openvrml::resource_fetcher::get_resource(const std::string & uri)
{
    return this->do_get_resource(uri);
}

/**
 * @fn std::auto_ptr<openvrml::resource_istream> openvrml::resource_fetcher::do_get_resource(const std::string & uri)
 *
 * @brief Fetch a network resource.
 *
 * Called by @c #get_resource, clients of OpenVRML are required to provide an
 * implementation for this function.  OpenVRML depends on the implementation
 * of this function for all of its input needs.  As such, what kinds of
 * resources OpenVRML is capable of resolving are entirely dependent on code
 * provided by the application.
 *
 * Implementations should throw @c std::invalid_argument if @p uri is
 * malformed or in an format that is not supported by the implementation.
 *
 * A trivial implementation designed to handle only @c file resources can use
 * @c std::filebuf:
 *
 * @code
 * std::auto_ptr<openvrml::resource_istream>
 * my_resource_fetcher::do_get_resource(const std::string & uri)
 * {
 *     using std::auto_ptr;
 *     using std::invalid_argument;
 *     using std::string;
 *     using openvrml::resource_istream;
 *
 *     class file_resource_istream : public resource_istream {
 *         std::string url_;
 *         std::filebuf buf_;
 *
 *     public:
 *         explicit file_resource_istream(const std::string & path):
 *             resource_istream(&this->buf_)
 *         {
 *             //
 *             // Note that the failbit is set in the constructor if no data
 *             // can be read from the stream.  This is important.  If the
 *             // failbit is not set on such a stream, OpenVRML will attempt
 *             // to read data from a stream that cannot provide it.
 *             //
 *             if (!this->buf_.open(path.c_str(),
 *                                  ios_base::in | ios_base::binary)) {
 *                 this->setstate(ios_base::badbit);
 *             }
 *         }
 *
 *         void url(const std::string & str)
 *         {
 *             this->url_ = str;
 *         }
 *
 *     private:
 *         virtual const std::string url() const
 *         {
 *             return this->url_;
 *         }
 *
 *         virtual const std::string type() const
 *         {
 *             //
 *             // A real application should use OS facilities for this;
 *             // however, that is beyond the scope of this example (which
 *             // is intended to be portable and stupid).
 *             //
 *             using std::find;
 *             using std::string;
 *             using boost::algorithm::iequals;
 *             using boost::next;
 *             string media_type = "application/octet-stream";
 *             const string::const_reverse_iterator dot_pos =
 *                 find(this->url_.rbegin(), this->url_.rend(), '.');
 *             if (dot_pos == this->url_.rend()
 *                 || next(dot_pos.base()) == this->url_.end()) {
 *                 return media_type;
 *             }
 *             const string::const_iterator hash_pos =
 *                 find(next(dot_pos.base()), this->url_.end(), '#');
 *             const string ext(dot_pos.base(), hash_pos);
 *             if (iequals(ext, "wrl")) {
 *                 media_type = "model/vrml";
 *             } else if (iequals(ext, "png")) {
 *                 media_type = "image/png";
 *             } else if (iequals(ext, "jpg") || iequals(ext, "jpeg")) {
 *                 media_type = "image/jpeg";
 *             }
 *             return media_type;
 *         }
 *
 *         virtual bool data_available() const
 *         {
 *             return !!(*this);
 *         }
 *     };
 *
 *     const string scheme = uri.substr(0, uri.find_first_of(':'));
 *     if (scheme != "file") {
 *         throw invalid_argument('\"' + scheme + "\" URI scheme not "
 *                                "supported");
 *     }
 *     //
 *     // file://
 *     //        ^
 *     // 01234567
 *     //
 *     string path = uri.substr(uri.find_first_of('/', 7));
 *
 *     auto_ptr<resource_istream> in(new file_resource_istream(path));
 *     static_cast<file_resource_istream *>(in.get())->url(uri);
 *
 *     return in;
 * }
 * @endcode
 *
 * The @p uri parameter is provided by OpenVRML and can be assumed to be an
 * absolute URI.  As such, it will always have a scheme through which the
 * client code can choose a resolution mechanism.  For more information on URI
 * syntax, see <a
 * href="ftp://ftp.rfc-editor.org/in-notes/std/std66.txt">Internet
 * STD&nbsp;66</a>.
 *
 * @param[in] uri   an absolute Uniform Resource Identifier.
 *
 * @return the requested resource as a stream.
 *
 * @exception std::invalid_argument if @p uri is malformed or in an
 *                                  unsupported format.
 *
 * @sa ftp://ftp.rfc-editor.org/in-notes/std/std66.txt
 */


/**
 * @class openvrml::stream_listener openvrml/browser.h
 *
 * @brief An interface to simplify asynchronously reading a
 *        @c resource_istream.
 */

/**
 * @brief Destroy.
 */
openvrml::stream_listener::~stream_listener() OPENVRML_NOTHROW
{}

/**
 * @brief Called once the stream is available for use.
 *
 * This function calls @c #do_stream_available.
 *
 * @param[in] uri           the URI associated with the stream.
 * @param[in] media_type    the MIME media type for the stream.
 */
void
openvrml::stream_listener::stream_available(const std::string & uri,
                                            const std::string & media_type)
{
    this->do_stream_available(uri, media_type);
}

/**
 * @fn void openvrml::stream_listener::do_stream_available(const std::string & uri, const std::string & media_type)
 *
 * @brief Called by @c #stream_available.
 *
 * Concrete @c stream_listener%s must override this function.
 *
 * @param[in] uri           the URI associated with the stream.
 * @param[in] media_type    the MIME media type for the stream.
 */

/**
 * @brief Called when data is available.
 *
 * This function calls @c #do_data_available.
 *
 * @param[in] data  the data.
 */
void
openvrml::stream_listener::
data_available(const std::vector<unsigned char> & data)
{
    this->do_data_available(data);
}

/**
 * @fn void openvrml::stream_listener::do_data_available(const std::vector<unsigned char> & data)
 *
 * @brief Called by @c #data_available.
 *
 * @param[in] data  the data.
 */

/**
 * @class openvrml::invalid_vrml openvrml/browser.h
 *
 * @brief Exception thrown when the parser fails due to errors in the VRML
 *        input.
 */

/**
 * @var const std::string openvrml::invalid_vrml::url
 *
 * @brief Resource identifier.
 */

/**
 * @var size_t openvrml::invalid_vrml::line
 *
 * @brief Line number.
 */

/**
 * @var size_t openvrml::invalid_vrml::column
 *
 * @brief Column number.
 */

/**
 * @brief Construct.
 *
 * @param[in] url       resource identifier of the stream.
 * @param[in] line      line number where the error was detected.
 * @param[in] column    column number where the error was detected.
 * @param[in] message   description of the error.
 */
openvrml::invalid_vrml::invalid_vrml(const std::string & url,
                                     const size_t line,
                                     const size_t column,
                                     const std::string & message):
    std::runtime_error(message),
    url(url),
    line(line),
    column(column)
{}

/**
 * @brief Destroy.
 */
openvrml::invalid_vrml::~invalid_vrml() throw ()
{}


/**
 * @class openvrml::viewer_in_use openvrml/browser.h
 *
 * @brief Exception thrown when attempting to associate a @c viewer
 *        with a @c browser when the @c viewer is already
 *        associated with a @c browser.
 */

/**
 * @brief Construct.
 */
openvrml::viewer_in_use::viewer_in_use():
    std::invalid_argument("viewer in use")
{}

/**
 * @brief Destroy.
 */
openvrml::viewer_in_use::~viewer_in_use() throw ()
{}


/**
 * @class openvrml::browser_event openvrml/browser.h
 *
 * @brief A <code>browser</code>-wide event.
 */

/**
 * @var openvrml::browser_event::browser
 *
 * @brief Only @c browser%s can construct @c browser_event%s.
 */

/**
 * @enum openvrml::browser_event::type_id
 *
 * @brief @c browser_event type identifier.
 */

/**
 * @var openvrml::browser_event::initialized
 *
 * @brief An @c initialized event is sent once the world has loaded and all
 *        nodes in the initial scene have been initialized.
 */

/**
 * @var openvrml::browser_event::shutdown
 *
 * @brief A @c shutdown event is sent once all of the nodes in the scene have
 *        been shut down.
 */

/**
 * @internal
 *
 * @var openvrml::browser * openvrml::browser_event::source_
 *
 * @brief The @c browser from which the event originated.
 */

/**
 * @internal
 *
 * @var openvrml::browser_event::type_id openvrml::browser_event::id_
 *
 * @brief Event type identifier.
 */

/**
 * @internal
 *
 * @brief Construct.
 *
 * @param[in] b     the @c browser from which the event originated.
 * @param[in] id    the event type.
 */
openvrml::browser_event::browser_event(browser & b, type_id id)
    OPENVRML_NOTHROW:
    source_(&b),
    id_(id)
{}

/**
 * @brief Event type identifier.
 *
 * @return the event type identifier.
 */
openvrml::browser_event::type_id openvrml::browser_event::id() const
    OPENVRML_NOTHROW
{
    return this->id_;
}

/**
 * @brief The @c browser that emitted the event.
 *
 * @return the @c browser that emitted the event.
 */
openvrml::browser & openvrml::browser_event::source() const OPENVRML_NOTHROW
{
    return *this->source_;
}


/**
 * @class openvrml::browser_listener openvrml/browser.h
 *
 * @brief This class should be inherited by classes that want to listen for
 *        @c browser_event%s.
 */

/**
 * @internal
 *
 * @var openvrml::browser_listener::browser
 *
 * @brief @c browser instances need to call @c #browser_changed.
 */

/**
 * @brief Destroy.
 */
openvrml::browser_listener::~browser_listener() OPENVRML_NOTHROW
{}

/**
 * @internal
 *
 * @brief Send a @c browser_event.
 *
 * This function delegates to @c #do_browser_changed.
 *
 * @param[in] event the @c browser_event to send.
 */
void openvrml::browser_listener::browser_changed(const browser_event & event)
{
    this->do_browser_changed(event);
}

/**
 * @fn void openvrml::browser_listener::do_browser_changed(const browser_event & event)
 *
 * @param[in] event the @c browser_event to send.
 */


/**
 * @class openvrml::browser openvrml/browser.h
 *
 * @brief Encapsulates a VRML browser.
 *
 * @c browser is the foundation of the OpenVRML runtime.  @c browser is
 * instantiated with an implementation of @c resource_fetcher, which is
 * provided by application code.  The @c resource_fetcher instance must have a
 * longer lifetime than the @c browser instance, since the @c resource_fetcher
 * instance could be used during destruction of the @c browser.  Note,
 * however, that <code>browser</code>'s destructor will block until all
 * threads that may use the @c resource_fetcher have completed.  So it is
 * sufficient to have the @c browser and the @c resource_fetcher destroyed
 * sequentially in the same thread.
 *
 * @sa openvrml::resource_fetcher
 */

/**
 * @example sdl_viewer.cpp
 *
 * Basic example using @c openvrml::browser.
 */

/**
 * @var class openvrml::browser::scene
 *
 * @brief The scene.
 */

/**
 * @internal
 *
 * @class openvrml::browser::node_metatype_map openvrml/browser.h
 *
 * @brief The map of @c node_metatype%s.
 */

/**
 * @var openvrml::read_write_mutex openvrml::browser::node_metatype_map::mutex_
 *
 * @brief Object mutex.
 */

/**
 * @typedef openvrml::browser::node_metatype_map::map_t
 *
 * @brief Map type.
 */

/**
 * @var openvrml::browser::node_metatype_map::map_t openvrml::browser::node_metatype_map::map_
 *
 * @brief Map.
 */

/**
 * @brief Construct.
 */
openvrml::browser::node_metatype_map::node_metatype_map()
{}

# ifndef NDEBUG
namespace {
    struct OPENVRML_LOCAL node_metatype_equals_ :
        public std::unary_function<std::pair<std::string,
                                             boost::shared_ptr<openvrml::node_metatype> >,
                                   bool> {
        explicit node_metatype_equals_(
            const boost::shared_ptr<openvrml::node_metatype> & node_metatype):
            node_metatype_(node_metatype)
        {}

        bool operator()(const argument_type & arg) const
        {
            return arg.second == this->node_metatype_;
        }

    private:
        boost::shared_ptr<openvrml::node_metatype> node_metatype_;
    };
}
# endif

/**
 * @brief Destroy.
 */
openvrml::browser::node_metatype_map::~node_metatype_map() OPENVRML_NOTHROW
{
# ifndef NDEBUG
    for (map_t::const_iterator entry = this->map_.begin();
         entry != this->map_.end();
         ++entry) {
        typedef std::iterator_traits<map_t::iterator>::difference_type
            difference_type;
        const difference_type count =
            std::count_if(this->map_.begin(),
                          this->map_.end(),
                          node_metatype_equals_(entry->second));
        assert(entry->second.use_count() == count
               && "shared_ptr<node_metatype> use_count does not match the "
               "number of entries in the browser's node_metatype_map");
    }
# endif
}

/**
 * @fn openvrml::browser::node_metatype_map::node_metatype_map(const node_metatype_map &)
 *
 * @brief Not implemented.
 */

/**
 * @brief Assign.
 *
 * @param[in] ncm   the value to assign.
 */
openvrml::browser::node_metatype_map &
openvrml::browser::node_metatype_map::operator=(const node_metatype_map & ncm)
{
    read_write_mutex::scoped_write_lock my_lock(this->mutex_);
    read_write_mutex::scoped_read_lock map_lock(ncm.mutex_);
    map_t temp(ncm.map_);
    swap(this->map_, temp);
    return *this;
}

namespace {
    typedef std::map<std::string, boost::shared_ptr<openvrml::node_metatype> >
        node_metatype_map_t;

    struct OPENVRML_LOCAL init_node_metatype :
        std::unary_function<void, node_metatype_map_t::value_type>
    {
        init_node_metatype(openvrml::viewpoint_node * initial_viewpoint,
                        const double time)
            OPENVRML_NOTHROW:
            initial_viewpoint_(initial_viewpoint),
            time_(time)
        {}

        void operator()(const node_metatype_map_t::value_type & value) const
            OPENVRML_NOTHROW
        {
            assert(value.second);
            value.second->initialize(this->initial_viewpoint_, this->time_);
        }

    private:
        openvrml::viewpoint_node * initial_viewpoint_;
        double time_;
    };
}

/**
 * @brief Initialize the @c node_metatype%s.
 *
 * @param[in] initial_viewpoint the @c viewpoint_node that should be initially
 *                              active.
 * @param[in] timestamp         the current time.
 */
void
openvrml::browser::node_metatype_map::init(viewpoint_node * initial_viewpoint,
                                           const double timestamp)
{
    read_write_mutex::scoped_read_lock lock(this->mutex_);
    std::for_each(this->map_.begin(), this->map_.end(),
                  init_node_metatype(initial_viewpoint, timestamp));
}

/**
 * @brief Insert a @c node_metatype.
 *
 * This operation will &ldquo;fail&rdquo; silently.  That is, if a
 * @c node_metatype corresponding to @p id already exists in the map, the
 * existing element will simply be returned.
 *
 * @param[in] id            the implementation identifier.
 * @param[in] node_metatype a @c node_metatype.
 *
 * @return the element in the node_metatype_map corresponding to @p id.
 */
const boost::shared_ptr<openvrml::node_metatype>
openvrml::browser::node_metatype_map::
insert(const std::string & id,
       const boost::shared_ptr<openvrml::node_metatype> & node_metatype)
{
    read_write_mutex::scoped_write_lock lock(this->mutex_);
    return this->map_.insert(make_pair(id, node_metatype)).first->second;
}

/**
 * @brief Remove a @c node_metatype.
 *
 * @param[in] id    the implementation identifier.
 *
 * @return @c true if a @c node_metatype is removed; @c false otherwise.
 */
bool openvrml::browser::node_metatype_map::remove(const std::string & id)
{
    read_write_mutex::scoped_write_lock lock(this->mutex_);
    return this->map_.erase(id) > 0;
}

/**
 * @brief Find a @c node_metatype.
 *
 * @param[in] id    an implementation identifier.
 *
 * @return the @c node_metatype corresponding to @p id, or a null
 *         pointer if no such @c node_metatype exists in the map.
 */
const boost::shared_ptr<openvrml::node_metatype>
openvrml::browser::node_metatype_map::find(const std::string & id) const
{
    read_write_mutex::scoped_read_lock lock(this->mutex_);
    const map_t::const_iterator pos = this->map_.find(id);
    return (pos != this->map_.end())
        ? pos->second
        : boost::shared_ptr<openvrml::node_metatype>();
}

/**
 * @brief The @c node_metatype identifiers associated with @p node_metatype.
 *
 * @param[in] node_metatype    a @c node_metatype.
 *
 * @return the @c node_metatype identifiers associated with @p node_metatype.
 */
const std::vector<openvrml::node_metatype_id>
openvrml::browser::node_metatype_map::
node_metatype_ids(const openvrml::node_metatype & node_metatype) const
    OPENVRML_THROW1(std::bad_alloc)
{
    read_write_mutex::scoped_read_lock lock(this->mutex_);
    std::vector<node_metatype_id> ids;
    for (map_t::const_iterator entry = this->map_.begin();
         entry != this->map_.end();
         ++entry) {
        if (entry->second.get() == &node_metatype) {
            ids.push_back(entry->first);
        }
    }
    return ids;
}

namespace {

    struct OPENVRML_LOCAL render_node_metatype :
            std::unary_function<void, node_metatype_map_t::value_type> {
        explicit render_node_metatype(openvrml::viewer & viewer):
            viewer(&viewer)
        {}

        void operator()(const node_metatype_map_t::value_type & value) const
        {
            value.second->render(*this->viewer);
        }

    private:
        openvrml::viewer * viewer;
    };
}

/**
 * @brief Render the @c node_metatype%s.
 *
 * @param[in,out] v a @c viewer.
 */
void openvrml::browser::node_metatype_map::render(openvrml::viewer & v)
{
    read_write_mutex::scoped_read_lock lock(this->mutex_);
    std::for_each(this->map_.begin(), this->map_.end(),
                  render_node_metatype(v));
}

namespace {

    struct OPENVRML_LOCAL shutdown_node_metatype :
            std::unary_function<void, node_metatype_map_t::value_type> {
        explicit shutdown_node_metatype(const double timestamp):
            timestamp_(timestamp)
        {}

        void operator()(const node_metatype_map_t::value_type & value) const
        {
            value.second->shutdown(this->timestamp_);
        }

    private:
        double timestamp_;
    };
}

/**
 * @brief Shut down the @c node_metatype%s.
 *
 * @param[in] timestamp the current time.
 */
void
openvrml::browser::node_metatype_map::shutdown(const double timestamp)
    OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock lock(this->mutex_);
    std::for_each(this->map_.begin(), this->map_.end(),
                  shutdown_node_metatype(timestamp));
}

/**
 * @internal
 *
 * @var std::auto_ptr<openvrml::null_node_metatype> openvrml::browser::null_node_metatype_
 *
 * @brief &ldquo;Null&rdquo; class object for default nodes (e.g.,
 *        @c default_viewpoint).
 */

/**
 * @internal
 *
 * @var std::auto_ptr<openvrml::null_node_type> openvrml::browser::null_node_type_
 *
 * @brief &ldquo;Null&rdquo; type object for default nodes (e.g., @c
 *        default_viewpoint).
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::load_root_scene_thread_mutex_
 *
 * @brief Mutex protecting @c #load_root_scene_thread_.
 */

/**
 * @internal
 *
 * @var boost::scoped_ptr<boost::thread> openvrml::browser::load_root_scene_thread_
 *
 * @brief The thread that loads the root scene.
 *
 * This thread is spawned by @c #load_url. It is joined there (in a subsequent
 * call to @c #load_url) or when the @c browser is destroyed.
 */

/**
 * @internal
 *
 * @var boost::thread_group openvrml::browser::load_proto_thread_group_
 *
 * @brief The threads that load @c EXTERNPROTO implementations.
 *
 * These threads @b must be joined by the @c browser before it is destroyed.
 */

/**
 * @internal
 *
 * @var openvrml::browser::node_metatype_map openvrml::browser::node_metatype_map_
 *
 * @brief A map of URIs to node implementations.
 */

/**
 * @internal
 *
 * @var openvrml::script_node_metatype openvrml::browser::script_node_metatype_
 *
 * @brief @c node_metatype for Script @c node%s in the @c browser.
 */

/**
 * @internal
 *
 * @var openvrml::resource_fetcher & openvrml::browser::fetcher_
 *
 * @brief A reference to the @c resource_fetcher associated with the
 *        @c browser.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::scene_mutex_
 *
 * @brief Mutex protecting @c #scene_.
 */

/**
 * @internal
 *
 * @var openvrml::scene * openvrml::browser::scene_
 *
 * @brief Pointer to the root @c scene.
 */

/**
 * @internal
 *
 * @var boost::intrusive_ptr<openvrml::node> openvrml::browser::default_viewpoint_
 *
 * @brief The &ldquo;default&rdquo; @c viewpoint_node used when no
 *        @c viewpoint_node in the scene is bound.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::active_viewpoint_mutex_
 *
 * @brief Mutex protecting @c #active_viewpoint_.
 */

/**
 * @internal
 *
 * @var openvrml::viewpoint_node * openvrml::browser::active_viewpoint_
 *
 * @brief The currently &ldquo;active&rdquo; @c viewpoint_node.
 */

/**
 * @internal
 *
 * @var boost::intrusive_ptr<openvrml::node> openvrml::browser::default_navigation_info_
 *
 * @brief The &ldquo;default&rdquo; @c navigation_info_node used when no @c
 *        navigation_info_node in the scene is bound.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::active_navigation_info_mutex_
 *
 * @brief Mutex protecting @c #active_navigation_info_.
 */

/**
 * @internal
 *
 * @var openvrml::navigation_info_node * openvrml::browser::active_navigation_info_
 *
 * @brief The currently &ldquo;active&rdquo; @c navigation_info_node.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::viewpoint_list_mutex_
 *
 * @brief Mutex protecting @c #viewpoint_list_.
 */

/**
 * @internal
 *
 * @var std::list<openvrml::viewpoint_node *> openvrml::browser::viewpoint_list_
 *
 * @brief A list of all the Viewpoint @c node%s in the @c browser.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::scoped_lights_mutex_
 *
 * @brief Mutex protecting @c #scoped_lights_.
 */

/**
 * @internal
 *
 * @var std::list<openvrml::node *> openvrml::browser::scoped_lights_
 *
 * @brief A list of all the scoped light @c node%s in the @c browser.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::scripts_mutex_
 *
 * @brief Mutex protecting @c #scripts_.
 */

/**
 * @internal
 *
 * @var std::list<openvrml::script_node *> openvrml::browser::scripts_
 *
 * @brief A list of all the Script @c node%s in the @c browser.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::timers_mutex_
 *
 * @brief Mutex protecting @c #timers_.
 */

/**
 * @internal
 *
 * @var std::list<openvrml::time_dependent_node *> openvrml::browser::timers_
 *
 * @brief A list of all the TimeSensor @c node%s in the @c browser.
 */

/**
 * @internal
 *
 * @var boost::mutex openvrml::browser::listeners_mutex_
 *
 * @brief Mutex to guard @c #listeners_.
 */

/**
 * @internal
 *
 * @var std::set<openvrml::browser_listener *> openvrml::browser::listeners_
 *
 * @brief The set of @c browser_listener%s that will receive
 *        @c browser_event%s.
 *
 * @sa #add_listener
 * @sa #remove_listener
 */

/**
 * @var bool openvrml::browser::modified_
 *
 * @brief Flag to indicate whether the @c browser has been modified.
 */

/**
 * @var openvrml::read_write_mutex openvrml::browser::modified_mutex_
 *
 * @brief Mutex protecting @c #modified_.
 */

/**
 * @internal
 *
 * @var bool openvrml::browser::new_view
 *
 * @brief Flag to indicate if the user has changed to a new view.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::delta_time_mutex_
 *
 * @brief Mutex protecting @c #delta_time.
 */

/**
 * @internal
 *
 * @var double openvrml::browser::delta_time
 *
 * @brief Time elapsed since the last update.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::viewer_mutex_
 *
 * @brief Mutex protecting @c #viewer_.
 */

/**
 * @internal
 *
 * @var openvrml::openvrml::viewer * openvrml::browser::viewer_
 *
 * @brief The current @c viewer.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex openvrml::browser::frame_rate_mutex_
 *
 * @brief Mutex protecting @c #frame_rate_.
 */

/**
 * @internal
 *
 * @var double openvrml::browser::frame_rate_
 *
 * @brief Frame rate.
 */

/**
 * @brief Get the current time.
 */
double openvrml::browser::current_time() OPENVRML_NOTHROW
{
# ifdef _WIN32
    _timeb timebuffer;
#   if defined(_MSC_VER) && (_MSC_VER < 1400)
    _ftime(&timebuffer);
#   else
    const errno_t err = _ftime_s(&timebuffer);
    assert(err == 0);
#   endif
    return double(timebuffer.time) + 1.0e-3 * double(timebuffer.millitm);
# else
    timeval tv;
    const int result = gettimeofday(&tv, 0);
    assert(result == 0);

    return double(tv.tv_sec) + 1.0e-6 * double(tv.tv_usec);
# endif
}

/**
 * @var boost::mutex openvrml::browser::out_mutex_
 *
 * @brief Mutex guarding @c #out_.
 */

/**
 * @var std::ostream & openvrml::browser::out_
 *
 * @brief Output stream, generally for console output.
 */

/**
 * @var boost::mutex openvrml::browser::err_mutex_
 *
 * @brief Mutex guarding @c #err_.
 */

/**
 * @var std::ostream & openvrml::browser::err_
 *
 * @brief Error output stream.
 */

/**
 * @var bool openvrml::browser::flags_need_updating
 *
 * @brief Set by @c node::bounding_volume_dirty on any node in this browser graph,
 *        cleared by @c #update_flags.
 *
 * @c true if the bvolume dirty flag has been set on a @c node in the
 * @c browser graph, but has not yet been propagated to that
 * <code>node</code>'s ancestors.
 */

namespace {
    void OPENVRML_LOCAL register_node_metatypes(openvrml::browser & b)
    {
        register_core_node_metatypes(b);
        register_vrml97_node_metatypes(b);
        register_networking_node_metatypes(b);
        register_grouping_node_metatypes(b);
        register_rendering_node_metatypes(b);
        register_shape_node_metatypes(b);
        register_geometry2d_node_metatypes(b);
        register_texturing_node_metatypes(b);
        register_interpolation_node_metatypes(b);
        register_key_device_sensor_node_metatypes(b);
        register_event_utilities_node_metatypes(b);
        register_dis_node_metatypes(b);
        register_environmental_effects_node_metatypes(b);
        register_geospatial_node_metatypes(b);
        register_hanim_node_metatypes(b);
        register_nurbs_node_metatypes(b);
        register_cad_geometry_node_metatypes(b);
    }
}

/**
 * @brief Constructor.
 *
 * @param[in] fetcher   a @c resource_fetcher implementation.
 * @param[in] out       output stream for console output.
 * @param[in] err       output stream for error console output.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::browser::browser(resource_fetcher & fetcher,
                           std::ostream & out,
                           std::ostream & err)
    OPENVRML_THROW1(std::bad_alloc):
    null_node_metatype_(new null_node_metatype(*this)),
    null_node_type_(new null_node_type(*null_node_metatype_)),
    script_node_metatype_(*this),
    fetcher_(fetcher),
    scene_(new scene(*this)),
    default_viewpoint_(new default_viewpoint(*null_node_type_)),
    active_viewpoint_(node_cast<viewpoint_node *>(default_viewpoint_.get())),
    default_navigation_info_(new default_navigation_info(*null_node_type_)),
    active_navigation_info_(
        node_cast<navigation_info_node *>(default_navigation_info_.get())),
    new_view(false),
    delta_time(DEFAULT_DELTA),
    viewer_(0),
    modified_(false),
    frame_rate_(0.0),
    out_(&out),
    err_(&err),
    flags_need_updating(false)
{
    assert(this->active_viewpoint_);
    assert(this->active_navigation_info_);
    register_node_metatypes(*this);
}

/**
 * @brief Destructor.
 */
openvrml::browser::~browser() OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock
        lock(this->load_root_scene_thread_mutex_);
    if (this->load_root_scene_thread_) {
        this->load_root_scene_thread_->join();
    }

    this->load_proto_thread_group_.join_all();

    const double now = browser::current_time();

    read_write_mutex::scoped_read_lock scene_lock(this->scene_mutex_);
    if (this->scene_) { this->scene_->shutdown(now); }

    this->node_metatype_map_.shutdown(now);
    assert(this->viewpoint_list_.empty());
    assert(this->scoped_lights_.empty());
    assert(this->scripts_.empty());
    assert(this->timers_.empty());
}

/**
 * @brief Add a @c node_metatype.
 *
 * If a @c node_metatype identified by @p id has already been added to
 * the browser, it will be replaced.
 *
 * @warning If @c std::bad_alloc is thrown here, the <code>browser</code>'s
 *          @c node_metatype map is left in an unknown state.  In all
 *          likelihood any preexisting entry in the map with the same
 *          implementation identifier as @p id will have been removed.
 *
 * @param[in] id    a @c node_metatype identifier.
 * @param[in] nc    a @c boost::shared_ptr to a @c node_metatype
 *
 * @exception std::invalid_argument if @p nc is null.
 * @exception std::bad_alloc        if memory allocation fails.
 */
void
openvrml::browser::
add_node_metatype(const node_metatype_id & id,
               const boost::shared_ptr<openvrml::node_metatype> & nc)
    OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
{
    if (!nc) {
        throw std::invalid_argument("cannot add null node_metatype pointer");
    }
    this->node_metatype_map_.remove(id); // Remove any existing entry.
    this->node_metatype_map_.insert(id, nc);
}

/**
 * @brief Get the @c node_metatype corresponding to @p id.
 *
 * @param[in] id    a @c node_metatype identifier.
 *
 * @return the @c node_metatype corresponding to @p id; or a null pointer if
 *         no such @c node_metatype exists.
 */
const boost::shared_ptr<openvrml::node_metatype>
openvrml::browser::node_metatype(const node_metatype_id & id) const OPENVRML_NOTHROW
{
    return this->node_metatype_map_.find(id);
}

/**
 * @brief Get the root @c scene.
 *
 * @return the root @c scene, or 0 if no @c scene is loaded.
 */
openvrml::scene * openvrml::browser::root_scene() const OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock lock(this->scene_mutex_);
    return this->scene_.get();
}

/**
 * @brief Get the path to a @c node in the scene graph.
 *
 * @param[in] n  the objective @c node.
 *
 * @return the path to @p node, starting with a root @c node, and ending with
 *         @p node. If @p node is not in the scene graph, the returned
 *         @c node_path is empty.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
const openvrml::node_path openvrml::browser::find_node(const node & n) const
    OPENVRML_THROW1(std::bad_alloc)
{
    read_write_mutex::scoped_read_lock lock(this->scene_mutex_);
    assert(this->scene_);

    class FindNodeTraverser : public node_traverser {
        const node & objectiveNode;
        node_path & nodePath;

    public:
        FindNodeTraverser(const node & objectiveNode, node_path & nodePath)
            OPENVRML_NOTHROW:
            objectiveNode(objectiveNode),
            nodePath(nodePath)
        {}

        virtual ~FindNodeTraverser() throw()
        {}

    private:
        virtual void on_entering(node & n) OPENVRML_THROW1(std::bad_alloc)
        {
            this->nodePath.push_back(&n);
            if (&n == &this->objectiveNode) { this->halt_traversal(); }
        }

        virtual void on_leaving(node &) OPENVRML_NOTHROW
        {
            if (!this->halted()) { this->nodePath.pop_back(); }
        }
    };

    node_path nodePath;
    FindNodeTraverser(n, nodePath).traverse(this->scene_->nodes());
    return nodePath;
}

/**
 * @brief Get the active @c viewpoint_node.
 *
 * The active @c viewpoint_node is the one currently associated with the user
 * view.
 *
 * @return the active @c viewpoint_node.
 */
openvrml::viewpoint_node & openvrml::browser::active_viewpoint() const
    OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock lock(this->active_viewpoint_mutex_);
    return *this->active_viewpoint_;
}

/**
 * @brief Set the active @c viewpoint_node.
 *
 * @param[in] viewpoint a @c viewpoint_node.
 *
 * @pre @p viewpoint.scene() == @c this->root_scene()
 */
void openvrml::browser::active_viewpoint(viewpoint_node & viewpoint)
    OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock scene_lock(this->scene_mutex_);
    read_write_mutex::scoped_write_lock
        active_viewpoint_lock(this->active_viewpoint_mutex_);
# ifndef NDEBUG
    scene * root_scene = 0;
    for (root_scene = viewpoint.scene();
         root_scene->parent();
         root_scene = root_scene->parent())
    {}
# endif
    assert(root_scene == this->scene_.get());
    this->active_viewpoint_ = &viewpoint;
}

/**
 * @brief Reset the active @c viewpoint_node to the default.
 */
void openvrml::browser::reset_default_viewpoint() OPENVRML_NOTHROW
{
    read_write_mutex::scoped_write_lock lock(this->active_viewpoint_mutex_);
    assert(this->default_viewpoint_);
    this->active_viewpoint_ =
        node_cast<viewpoint_node *>(this->default_viewpoint_.get());
    assert(this->active_viewpoint_);
}

/**
 * @brief Get the active @c navigation_info_node.
 *
 * The active @c navigation_info_node is the one currently associated with the
 * user view.
 *
 * @return the active @c navigation_info_node.
 */
openvrml::navigation_info_node &
openvrml::browser::active_navigation_info() const OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock
        lock(this->active_navigation_info_mutex_);
    return *this->active_navigation_info_;
}

/**
 * @brief Set the active @c navigation_info_node.
 *
 * @param[in] nav_info a @c navigation_info_node.
 *
 * @pre @p viewpoint.scene() == @c this->root_scene()
 */
void openvrml::browser::active_navigation_info(navigation_info_node & nav_info)
    OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock scene_lock(this->scene_mutex_);
    read_write_mutex::scoped_write_lock
        active_navigation_info_lock(this->active_navigation_info_mutex_);
# ifndef NDEBUG
    scene * root_scene = 0;
    for (root_scene = nav_info.scene();
         root_scene->parent();
         root_scene = root_scene->parent())
    {}
# endif
    assert(root_scene == this->scene_.get());
    this->active_navigation_info_ = &nav_info;
}

/**
 * @brief Reset the active @c navigation_info_node to the default.
 */
void openvrml::browser::reset_default_navigation_info() OPENVRML_NOTHROW
{
    read_write_mutex::scoped_write_lock
        lock(this->active_navigation_info_mutex_);
    assert(this->default_navigation_info_);
    this->active_navigation_info_ =
        node_cast<navigation_info_node *>(
            this->default_navigation_info_.get());
    assert(this->active_navigation_info_);
}

/**
 * @brief Add a @c viewpoint_node to the list of @c viewpoint_node%s for the
 *        @c browser.
 *
 * @param[in] viewpoint a @c viewpoint_node.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @pre @p viewpoint is not in the list of @c viewpoint_node%s for the @c
 *      browser.
 */
void openvrml::browser::add_viewpoint(viewpoint_node & viewpoint)
    OPENVRML_THROW1(std::bad_alloc)
{
    read_write_mutex::scoped_write_lock lock(this->viewpoint_list_mutex_);
    assert(std::find(this->viewpoint_list_.begin(), this->viewpoint_list_.end(),
                     &viewpoint) == this->viewpoint_list_.end());
    this->viewpoint_list_.push_back(&viewpoint);
}

/**
 * @brief Remove a @c viewpoint_node from the list of @c viewpoint_node%s for
 *        the @c browser.
 *
 * @param[in] viewpoint a @c viewpoint_node.
 *
 * @pre @p viewpoint is in the list of @c viewpoint_node%s for the @c browser.
 */
void openvrml::browser::remove_viewpoint(viewpoint_node & viewpoint)
    OPENVRML_NOTHROW
{
    read_write_mutex::scoped_write_lock lock(this->viewpoint_list_mutex_);
    assert(!this->viewpoint_list_.empty());
    typedef std::list<viewpoint_node *> viewpoint_list_t;
    const viewpoint_list_t::iterator end = this->viewpoint_list_.end();
    const viewpoint_list_t::iterator pos =
            std::find(this->viewpoint_list_.begin(), end, &viewpoint);
    assert(pos != end);
    this->viewpoint_list_.erase(pos);
}

/**
 * @brief Get the list of @c viewpoint_node%s for the world.
 *
 * @return the list of @c viewpoint_node%s for the world.
 */
const std::list<openvrml::viewpoint_node *>
openvrml::browser::viewpoints() const OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock lock(this->viewpoint_list_mutex_);
    return this->viewpoint_list_;
}

/**
 * @brief Set the current @c viewer.
 *
 * @param[in] v @c viewer.
 *
 * @exception viewer_in_use if @p v is already associated with a
 *                          @c browser.
 */
void openvrml::browser::viewer(openvrml::viewer * v)
    OPENVRML_THROW1(viewer_in_use)
{
    read_write_mutex::scoped_write_lock lock(this->viewer_mutex_);
    if (v && v->browser_) { throw viewer_in_use(); }
    if (this->viewer_) { this->viewer_->browser_ = 0; }
    this->viewer_ = v;
    if (v) { v->browser_ = this; }
}

/**
 * @brief The current @c viewer.
 *
 * @return the current @c viewer.
 */
openvrml::viewer * openvrml::browser::viewer() const OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock lock(this->viewer_mutex_);
    return this->viewer_;
}

/**
 * @brief Get the browser name.
 *
 * @return "OpenVRML"
 *
 * Specific browsers may wish to override this method.
 */
const char * openvrml::browser::name() const OPENVRML_NOTHROW
{
    return "OpenVRML";
}

/**
 * @brief Get the @c browser version.
 *
 * @return the version of OpenVRML.
 *
 * Specific browsers may wish to override this method.
 */
const char * openvrml::browser::version() const OPENVRML_NOTHROW
{
    return PACKAGE_VERSION;
}

/**
 * @brief Get the average navigation speed in meters per second.
 *
 * @return the average navigation speed.
 */
float openvrml::browser::current_speed()
{
    read_write_mutex::scoped_read_lock
        lock(this->active_navigation_info_mutex_);
    navigation_info_node & nav_info = this->active_navigation_info();
    return nav_info.speed();
}

/**
 * @brief Get the URI for the world.
 *
 * @return the URI for the world.
 */
const std::string openvrml::browser::world_url() const
    OPENVRML_THROW1(std::bad_alloc)
{
    read_write_mutex::scoped_read_lock lock(this->scene_mutex_);
    assert(this->scene_);
    return this->scene_->url(); // Throws std::bad_alloc.
}

/**
 * @brief Set the world from a stream.
 *
 * @param[in,out] in    an input stream.
 *
 * @exception bad_media_type    if @p in.type() is not @c model/vrml,
 *                              @c x-world/x-vrml, or @c model/x3d+vrml.
 * @exception invalid_vrml      if @p in has invalid syntax.
 */
void openvrml::browser::set_world(resource_istream & in)
{
    using std::for_each;
    {
        read_write_mutex::scoped_read_write_lock scene_lock(this->scene_mutex_);

        using std::string;
        using local::uri;

        //
        // Clear out the current scene.
        //
        this->load_proto_thread_group_.join_all();
        double now = browser::current_time();
        if (this->scene_) { this->scene_->shutdown(now); }
        this->node_metatype_map_.shutdown(now);
        read_write_mutex::scoped_read_lock
            listeners_lock(this->listeners_mutex_);
        for_each(this->listeners_.begin(), this->listeners_.end(),
                 boost::bind2nd(
                     boost::mem_fun(&browser_listener::browser_changed),
                     browser_event(*this, browser_event::shutdown)));

        scene_lock.promote();

        this->scene_.reset();
        assert(this->viewpoint_list_.empty());
        assert(this->scoped_lights_.empty());
        assert(this->scripts_.empty());
        assert(this->timers_.empty());

        //
        // Create the new scene.
        //
        node_metatype_map new_map;
        this->node_metatype_map_ = new_map;
        register_node_metatypes(*this);
        this->scene_.reset(new scene(*this));

        scene_lock.demote();

        this->scene_->load(in);

        //
        // Initialize.
        //
        now = browser::current_time();
        this->scene_->initialize(now);

        //
        // Get the initial viewpoint_node, if any was specified.
        //
        viewpoint_node * initial_viewpoint = 0;
        const string viewpoint_node_id = uri(this->scene_->url()).fragment();
        if (!viewpoint_node_id.empty()) {
            if (!this->scene_->nodes().empty()) {
                using boost::intrusive_ptr;
                const intrusive_ptr<node> & n = this->scene_->nodes().front();
                if (n) {
                    node * const vp = n->scope().find_node(viewpoint_node_id);
                    initial_viewpoint = dynamic_cast<viewpoint_node *>(vp);
                }
            }
        }

        //
        // Initialize the node_metatypes.
        //
        this->node_metatype_map_.init(initial_viewpoint, now);

        if (!this->active_viewpoint_) {
            read_write_mutex::scoped_write_lock
                lock(this->active_viewpoint_mutex_);
            this->active_viewpoint_ = this->default_viewpoint_.get();
        }

        this->modified(true);
        this->new_view = true; // Force resetUserNav
    } // unlock this->scene_mutex_, this->active_viewpoint_mutex_

    read_write_mutex::scoped_read_lock
        listeners_lock(this->listeners_mutex_);
    for_each(this->listeners_.begin(), this->listeners_.end(),
             boost::bind2nd(boost::mem_fun(&browser_listener::browser_changed),
                            browser_event(*this, browser_event::initialized)));
}

/**
 * @brief Replace the root nodes of the world.
 *
 * @param[in] nodes new root nodes for the world.
 */
void
openvrml::browser::replace_world(
    const std::vector<boost::intrusive_ptr<node> > & nodes)
{
    read_write_mutex::scoped_read_lock lock(this->scene_mutex_);
    const double now = browser::current_time();
    this->scene_->nodes(nodes);
    this->scene_->initialize(now);
    //
    // Initialize the node_metatypes.
    //
    static viewpoint_node * const initial_viewpoint = 0;
    this->node_metatype_map_.init(initial_viewpoint, now);
    this->modified(true);
    this->new_view = true; // Force resetUserNav
}

struct OPENVRML_LOCAL openvrml::browser::root_scene_loader {
    root_scene_loader(browser & b, const std::vector<std::string> & url)
        OPENVRML_THROW1(std::bad_alloc):
        browser_(&b),
        url_(url)
    {}

    void operator()() const OPENVRML_NOTHROW
    {
        try {
            using std::endl;

            openvrml::browser & browser = *this->browser_;

            try {
                std::auto_ptr<resource_istream> in;
                {
                    read_write_mutex::scoped_read_lock
                        lock(browser.scene_mutex_);
                    in = browser.scene_->get_resource(this->url_);
                }
                if (!(*in)) { throw unreachable_url(); }
                browser.set_world(*in);
            } catch (invalid_vrml & ex) {
                std::ostringstream out;
                out << ex.url << ':' << ex.line << ':' << ex.column
                    << ": error: " << ex.what();
                browser.err(out.str());
            } catch (unreachable_url &) {
                throw;
            } catch (std::exception & ex) {
                browser.err(ex.what());
            } catch (...) {
                throw unreachable_url();
            }
        } catch (unreachable_url & ex) {
            this->browser_->err(ex.what());
        }
    }

private:
    browser * browser_;
    const std::vector<std::string> url_;
};

/**
 * @brief Asynchronously load a VRML world into the @c browser.
 *
 * This function takes an alternative URI list consistent with such lists as
 * they appear in VRML nodes.  @c openvrml::browser_event::initialized will be
 * emitted once the world has been loaded and initialized.
 *
 * For synchronously loading a world, see @c #set_world.
 *
 * @param[in] url       a URI.
 * @param[in] parameter parameters for @p url.
 *
 * @exception std::bad_alloc                if memory allocation fails.
 * @exception boost::thread_resource_error  if thread creation fails.
 *
 * @sa #set_world
 */
void openvrml::browser::load_url(const std::vector<std::string> & url,
                                 const std::vector<std::string> &)
    OPENVRML_THROW2(std::bad_alloc, boost::thread_resource_error)
{
    {
        read_write_mutex::scoped_read_lock
            lock(this->load_root_scene_thread_mutex_);
        if (this->load_root_scene_thread_) {
            this->load_root_scene_thread_->join();
        }
    }

    boost::function0<void> f = root_scene_loader(*this, url);

    read_write_mutex::scoped_write_lock
        lock(this->load_root_scene_thread_mutex_);
    this->load_root_scene_thread_.reset(new boost::thread(f));
}

/**
 * @brief Send a string to the user interface.
 *
 * The default implementation of this method simply prints @p description to
 * @a out.  Subclasses can override this method to direct messages to an
 * application's UI; for instance, a status bar.
 *
 * @param[in] description   a string.
 */
void openvrml::browser::description(const std::string & description)
{
    this->out(description);
}


//
// stream_id_index_ is used to construct the URI for the stream; this is used
// to identify any PROTOs in the stream in the browser's node_metatype map.  A
// side-effect of this approach is that the node_metatype map will keep growing,
// even if identical streams are repeatedly loaded.  For this reason it is
// preferable to use an EXTERNPROTO in the stream.
//
namespace {
    OPENVRML_LOCAL size_t stream_id_index_ = 0;
    OPENVRML_LOCAL boost::mutex stream_id_index_mutex_;
}

/**
 * @brief Generate nodes from a stream of VRML syntax.
 *
 * In addition to the exceptions listed, this method may throw any
 * exception that may result from reading the input stream.
 *
 * @param[in,out] in    an input stream.
 * @param[in]     type  MIME content type of @p in.
 *
 * @return the root nodes generated from @p in.
 *
 * @exception invalid_vrml          if @p in has invalid VRML syntax.
 * @exception std::invalid_argument if @p type refers to an unsupported content
 *                                  type.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const std::vector<boost::intrusive_ptr<openvrml::node> >
openvrml::browser::create_vrml_from_stream(std::istream & in,
                                           const std::string & type)
{
    using std::ostringstream;
    using std::string;
    using std::vector;
    using boost::lexical_cast;

    {
        boost::mutex::scoped_lock lock(stream_id_index_mutex_);
        ++stream_id_index_;
    }

    ostringstream stream_id;
    stream_id << anonymous_stream_id_prefix_
              << lexical_cast<string>(stream_id_index_);

    std::vector<boost::intrusive_ptr<node> > nodes;
    try {
        read_write_mutex::scoped_read_lock lock(this->scene_mutex_);
        assert(this->scene_);
        std::map<string, string> meta;
        parse_vrml(in, stream_id.str(), type, *this->scene_, nodes, meta);
    } catch (openvrml::bad_media_type & ex) {
        //
        // bad_media_type is a std::runtime_error.  However, here we're using
        // the media type as an argument.
        //
        throw std::invalid_argument(ex.what());
    }
    return nodes;
}

/**
 * @brief Create nodes from a URI.
 *
 * This function executes asynchronously. When the nodes have been completely
 * loaded, they are sent to the @p event MFNode eventIn of @p node.
 *
 * @param[in] url       an alternative URI list.
 * @param[in] node      the node to which the nodes loaded from @p url should be
 *                      sent as an event.
 * @param[in] event     the event of @p node to which the new nodes will be sent.
 *
 * @exception unsupported_interface         if @p node has no eventIn @p event.
 * @exception std::bad_cast                 if the @p event eventIn of @p node
 *                                          is not an MFNode.
 * @exception boost::thread_resource_error  if thread creation fails.
 */
void
openvrml::browser::
create_vrml_from_url(const std::vector<std::string> & url,
                     const boost::intrusive_ptr<node> & node,
                     const std::string & event)
    OPENVRML_THROW3(unsupported_interface, std::bad_cast,
                    boost::thread_resource_error)
{
    read_write_mutex::scoped_read_lock lock(this->scene_mutex_);
    assert(this->scene_);
    this->scene_->create_vrml_from_url(url, node, event);
}

/**
 * @brief Add a listener for @c browser_event%s.
 *
 * @param[in] listener  a @c browser_listener.
 *
 * @return @c true if @p listener is added successfully; @c false otherwise (if
 *         @p listener is already listening for events from the
 *         @c browser).
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
bool openvrml::browser::add_listener(browser_listener & listener)
    OPENVRML_THROW1(std::bad_alloc)
{
    read_write_mutex::scoped_write_lock lock(this->listeners_mutex_);
    return this->listeners_.insert(&listener).second;
}

/**
 * @brief Remove a listener for @c browser_event%s.
 *
 * @param[in] listener  a @c browser_listener.
 *
 * @return @c true if @p listener is removed successfully; @c false otherwise
 *         (if @p listener is not listening for events from the
 *         @c browser).
 */
bool openvrml::browser::remove_listener(browser_listener & listener)
    OPENVRML_NOTHROW
{
    read_write_mutex::scoped_write_lock lock(this->listeners_mutex_);
    return this->listeners_.erase(&listener) > 0;
}

/**
 * @brief Get the current frame rate.
 *
 * @return the current frame rate.
 */
double openvrml::browser::frame_rate() const
{
    read_write_mutex::scoped_read_lock lock(this->frame_rate_mutex_);
    return this->frame_rate_;
}

/**
 * Called by the @c viewer when the cursor passes over, clicks, drags, or
 * releases a sensitive object (an Anchor or another grouping node with
 * an enabled TouchSensor child).
 */
void openvrml::browser::sensitive_event(node * const n,
                                        const double timestamp,
                                        const bool is_over,
                                        const bool is_active,
                                        const double (&point)[3])
{
    if (n) {
        if (pointing_device_sensor_node * pointing_device_sensor =
            node_cast<pointing_device_sensor_node *>(n)) {
            pointing_device_sensor->activate(timestamp,
                                             is_over,
                                             is_active,
                                             point);
        } else if (grouping_node * g = node_cast<grouping_node *>(n)) {
            //
            // The parent grouping node is registered for Touch/Drag Sensors.
            //
            g->activate_pointing_device_sensors(timestamp,
                                                is_over,
                                                is_active,
                                                point);
            this->modified(true);
        }
    }
}

/**
 * @brief Process events (update the @c browser).
 *
 * This method should be called after each frame is rendered.
 *
 * @return @c true if the @c browser needs to be rerendered, @c false otherwise.
 */
bool openvrml::browser::update(double current_time)
{
    using std::for_each;

    read_write_mutex::scoped_read_lock
        timers_lock(this->timers_mutex_),
        scripts_lock(this->scripts_mutex_);

    if (current_time <= 0.0) { current_time = browser::current_time(); }

    this->delta_time = DEFAULT_DELTA;

    //
    // Update each of the timers.
    //
    for_each(this->timers_.begin(), this->timers_.end(),
             boost::bind2nd(boost::mem_fun(&time_dependent_node::update),
                            current_time));

    //
    // Update each of the scripts.
    //
    for_each(this->scripts_.begin(), this->scripts_.end(),
             boost::bind2nd(boost::mem_fun(&script_node::update),
                            current_time));

    // Signal a redisplay if necessary
    return this->modified();
}

/**
 * @brief Indicate whether the headlight is on.
 *
 * @return @c true if the headlight is on; @c false otherwise.
 */
bool openvrml::browser::headlight_on()
{
    navigation_info_node & nav_info = this->active_navigation_info();
    return nav_info.headlight();
}

/**
 * @brief Draw this @c browser into the specified @c viewer
 */
void openvrml::browser::render()
{
    read_write_mutex::scoped_read_lock
        scene_lock(this->scene_mutex_),
        active_viewpoint_lock_(this->active_viewpoint_mutex_);

    if (!this->viewer_) { return; }

    if (this->new_view) {
        this->viewer_->reset_user_navigation();
        this->new_view = false;
    }
    navigation_info_node & nav_info = this->active_navigation_info();
    const float avatarSize = nav_info.avatar_size().empty()
        ? 0.25f
        : nav_info.avatar_size()[0];
    const float visibilityLimit = nav_info.visibility_limit();

    this->viewer_->set_frustum(this->active_viewpoint_->field_of_view(),
                               avatarSize,
                               visibilityLimit);

    //
    // Per-node_metatype rendering happens before viewer::set_viewpoint is called
    // This is important for things like background rendering, since
    // viewer::insert_background must be called before viewer::set_viewpoint.
    //
    this->node_metatype_map_.render(*this->viewer_);

    // Activate the headlight.
    // ambient is supposed to be 0 according to the spec...
    if (this->headlight_on()) {
        static const color color = make_color(1.0, 1.0, 1.0);
        static const vec3f direction = make_vec3f(0.0, 0.0, -1.0);
        static const float ambientIntensity = 0.3f;
        static const float intensity = 1.0;

        this->viewer_->insert_dir_light(ambientIntensity,
                                        intensity,
                                        color,
                                        direction);
    }

    // sets the viewpoint transformation
    //
    const mat4f t = this->active_viewpoint_->user_view_transform()
                    * this->active_viewpoint_->transformation();
    vec3f position, scale;
    rotation orientation;
    t.transformation(position, orientation, scale);
    this->viewer_->set_viewpoint(position,
                                 orientation,
                                 avatarSize,
                                 visibilityLimit);

    // Top level object

    this->viewer_->begin_object(0);
    mat4f modelview = t.inverse();
    rendering_context rc(bounding_volume::partial, modelview);
    rc.draw_bounding_spheres = true;

    // Do the browser-level lights (Points and Spots)
    {
        using std::for_each;
        read_write_mutex::scoped_read_lock
            scoped_lights_lock(this->scoped_lights_mutex_);
        for_each(this->scoped_lights_.begin(), this->scoped_lights_.end(),
                 boost::bind2nd(
                     boost::mem_fun(&scoped_light_node::render_scoped_light),
                     *this->viewer_));
    }

    //
    // Render the nodes.  scene_ may be 0 if the world failed to load.
    //
    if (this->scene_) {
        this->scene_->render(*this->viewer_, rc);
    }

    this->viewer_->end_object();

    // This is actually one frame late...
    read_write_mutex::scoped_write_lock
        frame_rate_lock(this->frame_rate_mutex_);
    this->frame_rate_ = this->viewer_->frame_rate();

    this->modified(false);
}

/**
 * @brief Indicate whether rendering is necessary.
 *
 * @param[in] value @c true to indicate that the browser state has changed and
 *                  rerendering is necessary; @c false once rendering has
 *                  occurred.
 */
void openvrml::browser::modified(const bool value)
{
    read_write_mutex::scoped_write_lock lock(this->modified_mutex_);
    this->modified_ = value;
}

/**
 * @brief Check if the browser has been modified.
 *
 * @return @c true if the browser has been modified, @c false otherwise.
 */
bool openvrml::browser::modified() const
{
    read_write_mutex::scoped_read_lock lock(this->modified_mutex_);
    return this->modified_;
}

/**
 * @brief Set the time until the next update is needed.
 *
 * @param[in] d a time interval.
 */
void openvrml::browser::delta(const double d)
{
    read_write_mutex::scoped_write_lock lock(this->delta_time_mutex_);
    if (d < this->delta_time) { this->delta_time = d; }
}

/**
 * @brief Get the time interval between @c browser updates.
 *
 * @return the time interval between @c browser updates.
 */
double openvrml::browser::delta() const
{
    read_write_mutex::scoped_read_lock lock(this->delta_time_mutex_);
    return this->delta_time;
}

/**
 * @brief Add a scoped light node to the @c browser.
 *
 * @param[in] light a light node.
 *
 * @pre @p light is not in the list of light nodes for the @c browser.
 */
void
openvrml::browser::add_scoped_light(scoped_light_node & light)
{
    read_write_mutex::scoped_write_lock lock(this->scoped_lights_mutex_);
    assert(std::find(this->scoped_lights_.begin(), this->scoped_lights_.end(),
                     &light) == this->scoped_lights_.end());
    this->scoped_lights_.push_back(&light);
}

/**
 * @brief Remove a scoped light node from the @c browser.
 *
 * @param[in] light the light node to remove.
 *
 * @pre @p light is in the list of light nodes for the @c browser.
 */
void
openvrml::browser::remove_scoped_light(scoped_light_node & light)
{
    read_write_mutex::scoped_write_lock lock(this->scoped_lights_mutex_);
    assert(!this->scoped_lights_.empty());
    const std::list<scoped_light_node *>::iterator end =
        this->scoped_lights_.end();
    const std::list<scoped_light_node *>::iterator pos =
            std::find(this->scoped_lights_.begin(), end, &light);
    assert(pos != end);
    this->scoped_lights_.erase(pos);
}

/**
 * @brief Add a Script node to the browser.
 *
 * @param[in] script    a Script node.
 *
 * @pre @p script is not in the list of Script nodes for the @c browser.
 */
void openvrml::browser::add_script(script_node & script)
{
    read_write_mutex::scoped_write_lock lock(this->scripts_mutex_);
    assert(std::find(this->scripts_.begin(), this->scripts_.end(), &script)
            == this->scripts_.end());
    this->scripts_.push_back(&script);
}

/**
 * @brief Remove a Script node from the browser.
 *
 * @param[in] script    the Script node to remove.
 *
 * @pre @p script is in the list of Script nodes for the @c browser.
 */
void openvrml::browser::remove_script(script_node & script)
{
    read_write_mutex::scoped_write_lock lock(this->scripts_mutex_);
    assert(!this->scripts_.empty());
    typedef std::list<script_node *> script_node_list_t;
    const script_node_list_t::iterator end = this->scripts_.end();
    const script_node_list_t::iterator pos =
            std::find(this->scripts_.begin(), end, &script);
    assert(pos != end);
    this->scripts_.erase(pos);
}

/**
 * @brief Add a time-dependent node to the browser.
 *
 * @param[in] n a @c time_dependent_node.
 *
 * @pre @p n is not in the list of @c time_dependent_node%s for the @c browser.
 */
void openvrml::browser::add_time_dependent(time_dependent_node & n)
{
    read_write_mutex::scoped_write_lock lock(this->timers_mutex_);
    assert(std::find(this->timers_.begin(), this->timers_.end(), &n)
           == this->timers_.end());
    this->timers_.push_back(&n);
}

/**
 * @brief Remove a time-dependent node from the browser.
 *
 * @param[in] n the @c time_dependent_node to remove.
 *
 * @pre @p n is in the list of @c time_dependent_node%s for the @c browser.
 */
void
openvrml::browser::remove_time_dependent(time_dependent_node & n)
{
    read_write_mutex::scoped_write_lock lock(this->timers_mutex_);
    assert(!this->timers_.empty());
    const std::list<time_dependent_node *>::iterator end = this->timers_.end();
    const std::list<time_dependent_node *>::iterator pos =
            std::find(this->timers_.begin(), end, &n);
    assert(pos != end);
    this->timers_.erase(pos);
}

/**
 * @brief Propagate the bvolume dirty flag from children to ancestors.
 *
 * The invariant is that if a <code>node</code>'s bounding volume is out of
 * date, then the bounding volumes of all that <code>node</code>'s ancestors
 * must be out of date.  However, @c node does not maintain a parent pointer.
 * So we must do a traversal of the entire browser graph to do the
 * propagation.
 *
 * @see node::setBVolumeDirty
 * @see node::isBVolumeDirty
 */
void openvrml::browser::update_flags()
{
//  Node* root = this->getRoot();
//  root->updateModified(0x002);
}

/**
 * @brief Print a message to the output stream.
 *
 * @param[in] str   a string.
 */
void openvrml::browser::out(const std::string & str) const
{
    boost::mutex::scoped_lock(this->out_mutex_);
    *this->out_ << str << std::endl;
}

/**
 * @brief Print a message to the error stream.
 *
 * @param[in] str   a string.
 */
void openvrml::browser::err(const std::string & str) const
{
    boost::mutex::scoped_lock(this->err_mutex_);
    *this->err_ << str << std::endl;
}


openvrml::null_node_metatype::null_node_metatype(openvrml::browser & browser)
    OPENVRML_NOTHROW:
    node_metatype("urn:X-openvrml:node:null", browser)
{}

openvrml::null_node_metatype::~null_node_metatype() OPENVRML_NOTHROW
{}

const boost::shared_ptr<openvrml::node_type>
openvrml::null_node_metatype::
do_create_type(const std::string &, const node_interface_set &) const
    OPENVRML_NOTHROW
{
    assert(false);
    static const boost::shared_ptr<node_type> nodeType;
    return nodeType;
}


openvrml::null_node_type::null_node_type(null_node_metatype & nodeClass)
    OPENVRML_NOTHROW:
    node_type(nodeClass, std::string())
{}

openvrml::null_node_type::~null_node_type() OPENVRML_NOTHROW
{}

const openvrml::node_interface_set &
openvrml::null_node_type::do_interfaces() const OPENVRML_NOTHROW
{
    assert(false);
    static const node_interface_set interfaces;
    return interfaces;
}

const boost::intrusive_ptr<openvrml::node>
openvrml::null_node_type::
do_create_node(const boost::shared_ptr<openvrml::scope> &,
               const initial_value_map &) const
    OPENVRML_NOTHROW
{
    assert(false);
    static const boost::intrusive_ptr<node> node;
    return node;
}

namespace {

    component::xml_reader::xml_reader(::component & c):
        component_(c),
        state_(none),
        current_level_(0),
        current_node_(0)
    {}

    void component::xml_reader::read(const std::string & filename)
        OPENVRML_THROW1(std::runtime_error)
    {
        openvrml::local::xml_reader reader(filename);
        int result;
        while ((result = reader.read()) == 1) {
            this->process_node(reader);
        }

        if (result != 0) {
            throw std::runtime_error('\"' + filename + "\" failed to parse");
        }
    }

    void
    component::xml_reader::process_node(openvrml::local::xml_reader & reader)
    {
        const int node_type = reader.node_type();
        switch (node_type) {
        case openvrml::local::xml_reader::element_id:
            this->start_element(reader);
            break;
        case openvrml::local::xml_reader::end_element_id:
            this->end_element(reader);
            break;
        default:
            break;
        }
    }

    void
    component::xml_reader::start_element(openvrml::local::xml_reader & reader)
    {
        using std::pair;
        using std::strcmp;
        using std::string;
        using openvrml::node_interface;

        int move_to_attr_result;
        node_interface interface_;

        switch (this->state_) {
        case component::xml_reader::none:
            this->state_ = component::xml_reader::component;
            move_to_attr_result = reader.move_to_first_attribute();
            if (move_to_attr_result > 0) {
                this->component_.id_ = reader.value();
            }
            break;
        case component::xml_reader::component:
            this->state_ = ::component::xml_reader::level;
            this->component_.levels_.push_back(component::level());
            this->current_level_ = &this->component_.levels_.back();
            break;
        case component::xml_reader::level:
            if (reader.local_name() == "requires") {
                string id;
                size_t level;
                move_to_attr_result = reader.move_to_first_attribute();
                while (move_to_attr_result > 0) {
                    using boost::lexical_cast;
                    if (reader.local_name() == "id") {
                        id = reader.value();
                    } else if (reader.local_name() == "level") {
                        level = lexical_cast<size_t>(reader.value());
                    }
                    move_to_attr_result = reader.move_to_next_attribute();
                }

                this->current_level_->dependencies_.insert(
                    make_pair(id, level));
            } else if (reader.local_name() == "node") {
                this->state_ = component::xml_reader::node;

                string id;
                component::node_type_decl node_type;
                move_to_attr_result = reader.move_to_first_attribute();
                while (move_to_attr_result > 0) {
                    if (reader.local_name() == "id") {
                        id = reader.value();
                    } else if (reader.local_name() == "metatype-id") {
                        node_type.metatype_id = reader.value();
                    }
                    move_to_attr_result = reader.move_to_next_attribute();
                }
                pair<component::level::iterator, bool> result =
                    this->current_level_->insert(std::make_pair(id, node_type));
                if (result.second) {
                    this->current_node_ = &*result.first;
                }
            } else {
                throw std::runtime_error("unexpected element: "
                                         + reader.local_name());
            }
            break;
        case component::xml_reader::requires:
            break;
        case component::xml_reader::node:
            move_to_attr_result = reader.move_to_first_attribute();
            while (move_to_attr_result > 0) {
                using boost::lexical_cast;
                using openvrml::field_value;

                if (reader.local_name() == "id") {
                    interface_.id = reader.value();
                } else if (reader.local_name() == "type") {
                    try {
                        interface_.field_type =
                            lexical_cast<field_value::type_id>(reader.value());
                    } catch (const boost::bad_lexical_cast &) {
                        throw std::runtime_error(
                            "invalid field value type identifier \""
                            + reader.value() + '\"');
                    }
                } else if (reader.local_name() == "access-type") {
                    try {
                        interface_.type =
                            lexical_cast<node_interface::type_id>(
                                reader.value());
                    } catch (const boost::bad_lexical_cast &) {
                        throw std::runtime_error(
                            "invalid field access type identifier \""
                            + reader.value() + '\"');
                    }
                }
                move_to_attr_result = reader.move_to_next_attribute();
            }
            this->current_node_->second.interfaces.insert(interface_);
            break;
        case component::xml_reader::field: default:
            assert(false);
        }
    }

    void
    component::xml_reader::end_element(openvrml::local::xml_reader &)
    {
        switch (this->state_) {
        case component::xml_reader::none:
            break;
        case component::xml_reader::component:
            break;
        case component::xml_reader::level:
            this->state_ = component::xml_reader::component;
            break;
        case component::xml_reader::requires:
            break;
        case component::xml_reader::node:
            this->state_ = component::xml_reader::level;
            break;
        case component::xml_reader::field:
            break;
        }
    }

    component::component(const std::string & filename)
        OPENVRML_THROW1(std::runtime_error)
    {
        xml_reader reader(*this);
        reader.read(filename);
    }

    const std::string & component::id() const OPENVRML_NOTHROW
    {
        return this->id_;
    }

    std::size_t component::support_level() const OPENVRML_NOTHROW
    {
        return this->levels_.size();
    }

    bool
    component::add_type_desc(openvrml::node_type_decls & type_descs,
                             const std::string & node_type_id,
                             const openvrml::node_interface_set & interfaces)
        OPENVRML_THROW1(std::bad_alloc)
    {
        return type_descs.insert(make_pair(node_type_id, interfaces)).second;
    }

    void component::add_scope_entry(
        const openvrml::browser & b,
        const char * node_name,
        const openvrml::node_interface_set & interface_set,
        const char * urn,
        openvrml::scope & scope) const
        OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
    {
        using boost::shared_ptr;
        using openvrml::node_metatype;
        using openvrml::node_type;

        const shared_ptr<node_metatype> class_ = b.node_metatype(urn);
        assert(class_);
        const shared_ptr<node_type> type = class_->create_type(node_name,
                                                               interface_set);
        const std::pair<shared_ptr<node_type>, bool> add_type_result =
            scope.add_type(type);

        //
        // If scope::add_type failed, there are two possible reasons:
        //  1. We're trying to add exactly the same node_type as already exists
        //     in the scope.  The mostly likely way this would happen is if a
        //     component is added redundantly.  There are entirely reasonable
        //     ways that could happen; e.g., if the profile incorporates
        //     component A level 1, and the X3D file also has a component
        //     statement to import component A level 2.
        //  2. node_types in two different components have the same
        //     node_type::id.
        //
        if (!add_type_result.second && *add_type_result.first != *type) {
            //
            // The way things are currently implemented, we don't want to throw
            // here.  Even if a component has a problematic node, we might as
            // well add the rest of the nodes in it.  After all, we don't know
            // how many nodes in the component we've already added.
            //
            // At some point it may be desirable to make component import
            // transactional--i.e., all nodes are added successfully, or
            // component import fails totally.  But that doesn't seem necessary
            // at this point.
            //
            b.err(std::string("interfaces for \"") + node_name + "\" are not "
                  "consistent with the existing node type by that name");
        }
    }

    void component::add_to_scope(const openvrml::browser & b,
                                 openvrml::scope & scope,
                                 const size_t level) const
        OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
    {
        if (level > this->support_level()) {
            throw std::invalid_argument("unsupported component level");
        }
        for (std::size_t i = 0; i < level; ++i) {
            for (component::level::const_iterator node =
                     this->levels_[i].begin();
                 node != this->levels_[i].end();
                 ++node) try {
                this->add_scope_entry(b,
                                      node->first.c_str(),
                                      node->second.interfaces,
                                      node->second.metatype_id.c_str(),
                                      scope);
            } catch (openvrml::unsupported_interface & ex) {
                b.err(node->second.metatype_id + ": " + ex.what());
            }
        }
    }

    void
    component::
    add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                              const size_t level) const
        OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
    {
        if (level == 0 || level > this->support_level()) {
            throw std::invalid_argument("unsupported component level");
        }
        for (std::size_t i = 0; i < level; ++i) {
            for (component::level::const_iterator node =
                     this->levels_[i].begin();
                 node != this->levels_[i].end();
                 ++node) {
                component::add_type_desc(type_descs,
                                         node->first,
                                         node->second.interfaces);
            }
        }
    }


    component_registry::component_registry()
        OPENVRML_THROW2(boost::filesystem::filesystem_error,
                        std::bad_alloc)
    {
        using std::auto_ptr;
        using boost::filesystem::path;
        using boost::filesystem::directory_iterator;

        std::string key;
        bool succeeded;

        path data_path;
        const char * datadir_env = getenv("OPENVRML_DATADIR");
        data_path = datadir_env
                  ? datadir_env
                  : OPENVRML_PKGDATADIR_;

        const path component_path = data_path / "component";
        for (directory_iterator entry(component_path);
             entry != directory_iterator();
             ++entry) {
            if (!is_directory(entry->path())) try {
                auto_ptr<component>
                    c(new component(entry->path().file_string()));
                std::string key = c->id();
                succeeded = this->insert(key, c.release()).second;
                assert(succeeded);
            } catch (const std::runtime_error & ex) {
                std::cerr << ex.what() << std::endl;
            }
        }
    }


    void profile::add_component(const std::string & name, const int level)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //get the existing/new support level
        int & comp_level = components_[name];

        //update the support level.
        comp_level = std::max(level, comp_level);
    }

    std::auto_ptr<openvrml::scope>
    profile::create_root_scope(const openvrml::browser & browser,
                               const std::string & uri) const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::scope;

        std::auto_ptr<scope> root_scope(new scope(uri));
        for (map_t::const_iterator entry = this->components_.begin();
             entry != this->components_.end();
             ++entry) {
            try {
                const component & c = ::component_registry_.at(entry->first);
                c.add_to_scope(browser, *root_scope, entry->second);
            } catch (boost::bad_ptr_container_operation & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
            } catch (std::invalid_argument & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
            }
        }
        return root_scope;
    }

    std::auto_ptr<openvrml::node_type_decls>
    profile::create_node_type_desc_map() const
        OPENVRML_THROW1(std::bad_alloc)
    {
        using openvrml::node_type_decls;

        std::auto_ptr<node_type_decls> node_type_descs(new node_type_decls);
        for (map_t::const_iterator entry = this->components_.begin();
             entry != this->components_.end();
             ++entry) {
            try {
                const component & c = ::component_registry_.at(entry->first);
                c.add_to_node_type_desc_map(*node_type_descs, entry->second);
            } catch (boost::bad_ptr_container_operation & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
            } catch (std::invalid_argument & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
            }
        }
        return node_type_descs;
    }


    const char * const vrml97_profile::id = "VRML97";

    vrml97_profile::vrml97_profile()
    {
        this->add_component("VRML97", 1);
    }


    const char * const x3d_core_profile::id = "Core";

    x3d_core_profile::x3d_core_profile()
    {
        this->add_component("Core", 1);
    }


    const char * const x3d_interchange_profile::id = "Interchange";

    x3d_interchange_profile::x3d_interchange_profile()
    {
        this->add_component("Core", 1);
        this->add_component("Time", 1);
        this->add_component("Grouping", 1);
        this->add_component("Rendering", 3);
        this->add_component("Shape", 1);
        this->add_component("Geometry3D", 2);
        this->add_component("Lighting", 1);
        this->add_component("Texturing", 2);
        this->add_component("Interpolation", 2);
        this->add_component("Navigation", 1);
        this->add_component("EnvironmentalEffects", 1);
    }


    const char * const x3d_interactive_profile::id = "Interactive";

    x3d_interactive_profile::x3d_interactive_profile()
    {
        this->add_component("Core", 1);
        this->add_component("Time", 1);
        this->add_component("Grouping", 2);
        this->add_component("Rendering", 2);
        this->add_component("Shape", 1);
        this->add_component("Geometry3D", 3);
        this->add_component("Lighting", 2);
        this->add_component("Texturing", 2);
        this->add_component("Interpolation", 2);
        this->add_component("PointingDeviceSensor", 1);
        this->add_component("KeyDeviceSensor", 1);
        this->add_component("EnvironmentalSensor", 1);
        this->add_component("Navigation", 1);
        this->add_component("EnvironmentalEffects", 1);
        this->add_component("EventUtilities", 1);
    }


    const char * const x3d_mpeg4_profile::id = "MPEG-4";

    x3d_mpeg4_profile::x3d_mpeg4_profile()
    {
        this->add_component("Core", 1);
        this->add_component("Time", 1);
        this->add_component("Networking", 2);
        this->add_component("Grouping", 2);
        this->add_component("Rendering", 1);
        this->add_component("Shape", 1);
        this->add_component("Geometry3D", 2);
        this->add_component("Lighting", 2);
        this->add_component("Texturing", 2);
        this->add_component("Interpolation", 2);
        this->add_component("PointingDeviceSensor", 1);
        this->add_component("EnvironmentalSensor", 1);
        this->add_component("Navigation", 1);
        this->add_component("EnvironmentalEffects", 1);
    }


    const char * const x3d_immersive_profile::id = "Immersive";

    x3d_immersive_profile::x3d_immersive_profile()
    {
        this->add_component("Core", 2);
        this->add_component("Time", 1);
        this->add_component("Networking", 3);
        this->add_component("Grouping", 2);
        this->add_component("Rendering", 3);
        this->add_component("Shape", 2);
        this->add_component("Geometry3D", 4);
        this->add_component("Geometry2D", 1);
        this->add_component("Text", 1);
        this->add_component("Sound", 1);
        this->add_component("Lighting", 2);
        this->add_component("Texturing", 3);
        this->add_component("Interpolation", 2);
        this->add_component("PointingDeviceSensor", 1);
        this->add_component("KeyDeviceSensor", 2);
        this->add_component("EnvironmentalSensor", 2);
        this->add_component("Navigation", 2);
        this->add_component("EnvironmentalEffects", 2);
        this->add_component("EventUtilities", 1);
    }


    const char * const x3d_full_profile::id = "Full";

    x3d_full_profile::x3d_full_profile()
    {
        this->add_component("Core", 2);
        this->add_component("Time", 2);
        this->add_component("Networking", 3);
        this->add_component("Grouping", 3);
        this->add_component("Rendering", 4);
        this->add_component("Shape", 3);
        this->add_component("Geometry3D", 4);
        this->add_component("Geometry2D", 2);
        this->add_component("Text", 1);
        this->add_component("Sound", 1);
        this->add_component("Lighting", 3);
        this->add_component("Texturing", 3);
        this->add_component("Interpolation", 3);
        this->add_component("PointingDeviceSensor", 1);
        this->add_component("KeyDeviceSensor", 2);
        this->add_component("EnvironmentalSensor", 2);
        this->add_component("Navigation", 2);
        this->add_component("EnvironmentalEffects", 3);
        this->add_component("Geospatial", 1);
        this->add_component("H-Anim", 1);
        this->add_component("NURBS", 4);
        this->add_component("DIS", 1);
        this->add_component("EventUtilities", 1);
    }
} // namespace
