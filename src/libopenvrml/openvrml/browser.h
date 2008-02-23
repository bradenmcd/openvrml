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

# ifndef OPENVRML_BROWSER_H
#   define OPENVRML_BROWSER_H

#   include <boost/thread.hpp>
#   include <openvrml/read_write_mutex.h>
#   include <openvrml/script.h>

namespace openvrml {

    OPENVRML_API extern const char vrml_media_type[11];
    OPENVRML_API extern const char x_vrml_media_type[15];
    OPENVRML_API extern const char x3d_vrml_media_type[15];

    std::auto_ptr<node_type_decls> profile(const std::string & profile_id)
        OPENVRML_THROW2(std::invalid_argument, std::bad_alloc);

    void add_component(node_type_decls & node_types,
                       const std::string & component_id, size_t level)
        OPENVRML_THROW2(std::invalid_argument, std::bad_alloc);

    class OPENVRML_API resource_istream : public std::istream {
    public:
        virtual ~resource_istream() = 0;

        const std::string url() const OPENVRML_THROW1(std::bad_alloc);
        const std::string type() const OPENVRML_THROW1(std::bad_alloc);
        bool data_available() const OPENVRML_NOTHROW;

    protected:
        explicit resource_istream(std::streambuf * streambuf);

    private:
        virtual const std::string do_url() const
            OPENVRML_THROW1(std::bad_alloc) = 0;
        virtual const std::string do_type() const
            OPENVRML_THROW1(std::bad_alloc) = 0;
        virtual bool do_data_available() const OPENVRML_NOTHROW = 0;
    };


    class OPENVRML_API resource_fetcher {
    public:
        virtual ~resource_fetcher() OPENVRML_NOTHROW = 0;

        std::auto_ptr<resource_istream> get_resource(const std::string & uri);

    private:
        virtual std::auto_ptr<resource_istream>
        do_get_resource(const std::string & uri) = 0;
    };


    class OPENVRML_API stream_listener {
    public:
        virtual ~stream_listener() OPENVRML_NOTHROW = 0;

        void stream_available(const std::string & uri,
                              const std::string & media_type);
        void data_available(const std::vector<unsigned char> & data);

    private:
        virtual void
        do_stream_available(const std::string & uri,
                            const std::string & media_type) = 0;
        virtual void
        do_data_available(const std::vector<unsigned char> & data) = 0;
    };


    class OPENVRML_API invalid_vrml : public std::runtime_error {
    public:
        const std::string url;
        const size_t line;
        const size_t column;

        invalid_vrml(const std::string & url,
                     size_t line,
                     size_t column,
                     const std::string & message);
        virtual ~invalid_vrml() throw ();
    };


    class OPENVRML_API viewer_in_use : public std::invalid_argument {
    public:
        viewer_in_use();
        virtual ~viewer_in_use() throw ();
    };


    class OPENVRML_API bad_url : public std::runtime_error {
    public:
        bad_url(const std::string & message);
        virtual ~bad_url() throw ();
    };


    class OPENVRML_API invalid_url : public bad_url {
    public:
        invalid_url();
        virtual ~invalid_url() throw ();
    };


    class OPENVRML_API bad_media_type : public bad_url {
    public:
        explicit bad_media_type(const std::string & received_type)
            OPENVRML_NOTHROW;
        virtual ~bad_media_type() throw ();
    };


    class OPENVRML_API unreachable_url : public bad_url {
    public:
        unreachable_url();
        virtual ~unreachable_url() throw ();
    };


    class OPENVRML_API no_alternative_url : public bad_url {
    public:
        no_alternative_url();
        virtual ~no_alternative_url() throw ();
    };


    class OPENVRML_API script_node : public child_node {
        friend class script;

        static script_factory_registry script_factory_registry_;

    public:
        typedef std::map<std::string, boost::shared_ptr<field_value> >
            field_value_map_t;

        class OPENVRML_API eventout : boost::noncopyable {
            script_node & node_;
            boost::scoped_ptr<field_value> value_;
            bool modified_;
            boost::scoped_ptr<openvrml::event_emitter> emitter_;

        public:
            eventout(field_value::type_id type, script_node & node)
                OPENVRML_THROW1(std::bad_alloc);

            const field_value & value() const OPENVRML_NOTHROW;
            void value(const field_value & val)
                OPENVRML_THROW2(std::bad_alloc, std::bad_cast);

            bool modified() const OPENVRML_NOTHROW;

            openvrml::event_emitter & emitter() OPENVRML_NOTHROW;

            void emit_event(double timestamp) OPENVRML_THROW1(std::bad_alloc);
        };

        typedef boost::shared_ptr<openvrml::event_listener> event_listener_ptr;
        typedef std::map<std::string, event_listener_ptr> event_listener_map_t;
        typedef boost::shared_ptr<eventout> eventout_ptr;
        typedef std::map<std::string, eventout_ptr> eventout_map_t;

    private:
        class script_node_type : public node_type {
            node_interface_set interfaces_;

        public:
            explicit script_node_type(script_node_metatype & class_);
            virtual ~script_node_type() OPENVRML_NOTHROW;

            void add_interface(const node_interface & interface_)
                OPENVRML_THROW2(std::bad_alloc, std::invalid_argument);

        private:
            virtual const node_interface_set & do_interfaces() const
                OPENVRML_NOTHROW;
            virtual const boost::intrusive_ptr<node>
            do_create_node(const boost::shared_ptr<openvrml::scope> & scope,
                           const initial_value_map & initial_values) const
                OPENVRML_THROW3(unsupported_interface, std::bad_cast,
                                std::bad_alloc);
        };

        template <typename FieldValue> class script_event_listener;

        typedef script_event_listener<sfbool> sfbool_listener;
        typedef script_event_listener<sfcolor> sfcolor_listener;
        typedef script_event_listener<sfcolorrgba> sfcolorrgba_listener;
        typedef script_event_listener<sffloat> sffloat_listener;
        typedef script_event_listener<sfdouble> sfdouble_listener;
        typedef script_event_listener<sfimage> sfimage_listener;
        typedef script_event_listener<sfint32> sfint32_listener;
        typedef script_event_listener<sfnode> sfnode_listener;
        typedef script_event_listener<sfrotation> sfrotation_listener;
        typedef script_event_listener<sfstring> sfstring_listener;
        typedef script_event_listener<sftime> sftime_listener;
        typedef script_event_listener<sfvec2f> sfvec2f_listener;
        typedef script_event_listener<sfvec2d> sfvec2d_listener;
        typedef script_event_listener<sfvec3f> sfvec3f_listener;
        typedef script_event_listener<sfvec3d> sfvec3d_listener;
        typedef script_event_listener<mfbool> mfbool_listener;
        typedef script_event_listener<mfcolor> mfcolor_listener;
        typedef script_event_listener<mfcolorrgba> mfcolorrgba_listener;
        typedef script_event_listener<mffloat> mffloat_listener;
        typedef script_event_listener<mfdouble> mfdouble_listener;
        typedef script_event_listener<mfimage> mfimage_listener;
        typedef script_event_listener<mfint32> mfint32_listener;
        typedef script_event_listener<mfnode> mfnode_listener;
        typedef script_event_listener<mfrotation> mfrotation_listener;
        typedef script_event_listener<mfstring> mfstring_listener;
        typedef script_event_listener<mftime> mftime_listener;
        typedef script_event_listener<mfvec2f> mfvec2f_listener;
        typedef script_event_listener<mfvec2d> mfvec2d_listener;
        typedef script_event_listener<mfvec3f> mfvec3f_listener;
        typedef script_event_listener<mfvec3d> mfvec3d_listener;

        struct script_event_listener_creator;

        static const boost::shared_ptr<openvrml::event_listener>
        create_listener(field_value::type_id type, const std::string & id,
                        script_node & node)
            OPENVRML_THROW1(std::bad_alloc);

        template <typename FieldValue> class script_event_emitter;

        struct script_event_emitter_creator;

        static std::auto_ptr<openvrml::event_emitter>
        create_emitter(script_node & node, const field_value & value)
            OPENVRML_THROW1(std::bad_alloc);

        class set_url_listener_t : public node_field_value_listener<mfstring> {
        public:
            explicit set_url_listener_t(script_node & node);
            virtual ~set_url_listener_t() OPENVRML_NOTHROW;

        private:
            virtual const std::string do_eventin_id() const OPENVRML_NOTHROW;
            virtual void do_process_event(const mfstring & value,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class url_changed_emitter : public openvrml::mfstring_emitter {
        public:
            explicit url_changed_emitter(const mfstring & value)
                OPENVRML_NOTHROW;
            virtual ~url_changed_emitter() OPENVRML_NOTHROW;

        private:
            virtual const std::string do_eventout_id() const OPENVRML_NOTHROW;
        };

        class set_metadata_listener :
            public node_field_value_listener<sfnode> {
        public:
            explicit set_metadata_listener(script_node & node);
            virtual ~set_metadata_listener() OPENVRML_NOTHROW;

        private:
            virtual const std::string do_eventin_id() const OPENVRML_NOTHROW;
            virtual void do_process_event(const sfnode & value,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class metadata_changed_emitter : public openvrml::sfnode_emitter {
        public:
            explicit metadata_changed_emitter(const sfnode & value)
                OPENVRML_NOTHROW;
            virtual ~metadata_changed_emitter() OPENVRML_NOTHROW;

        private:
            virtual const std::string do_eventout_id() const OPENVRML_NOTHROW;
        };

        script_node_type type_;
        set_metadata_listener set_metadata_listener_;
        sfnode metadata_;
        metadata_changed_emitter metadata_changed_emitter_;
        sfbool direct_output;
        sfbool must_evaluate;
        set_url_listener_t set_url_listener;
        mfstring url_;
        url_changed_emitter url_changed_emitter_;
        field_value_map_t field_value_map_;
        event_listener_map_t event_listener_map_;
        eventout_map_t eventout_map_;
        boost::scoped_ptr<script> script_;
        int events_received;

    public:
        script_node(script_node_metatype & class_,
                    const boost::shared_ptr<openvrml::scope> & scope,
                    const node_interface_set & interfaces,
                    const initial_value_map & initial_values)
            OPENVRML_THROW4(unsupported_interface, std::bad_cast,
                            std::bad_alloc, std::invalid_argument);
        virtual ~script_node() OPENVRML_NOTHROW;

        void update(double current_time);

        const event_listener_map_t & event_listener_map() const
            OPENVRML_NOTHROW;
        const field_value_map_t & field_value_map() const OPENVRML_NOTHROW;
        const eventout_map_t & eventout_map() const OPENVRML_NOTHROW;

    private:
        OPENVRML_LOCAL std::auto_ptr<script> create_script()
            OPENVRML_THROW2(no_alternative_url, std::bad_alloc);

        OPENVRML_LOCAL void assign_with_self_ref_check(const sfnode &,
                                                       sfnode &) const
            OPENVRML_NOTHROW;
        OPENVRML_LOCAL void assign_with_self_ref_check(const mfnode &,
                                                       mfnode &) const
            OPENVRML_NOTHROW;

        virtual script_node * to_script() OPENVRML_NOTHROW;

        virtual void do_initialize(double timestamp)
            OPENVRML_THROW1(std::bad_alloc);
        virtual const field_value & do_field(const std::string & id) const
            OPENVRML_THROW1(unsupported_interface);
        virtual openvrml::event_listener &
        do_event_listener(const std::string & id)
            OPENVRML_THROW1(unsupported_interface);
        virtual openvrml::event_emitter &
        do_event_emitter(const std::string & id)
            OPENVRML_THROW1(unsupported_interface);
        virtual void do_shutdown(double timestamp) OPENVRML_NOTHROW;
        virtual void do_render_child(viewer & v, rendering_context context);
    };


    class browser;

    class OPENVRML_API browser_event {
        friend class browser;

    public:
        enum type_id {
            initialized = 1,
            shutdown = 2
        };

    private:
        browser * source_;
        type_id id_;

        OPENVRML_LOCAL browser_event(browser & b, type_id id) OPENVRML_NOTHROW;

    public:
        type_id id() const OPENVRML_NOTHROW;
        browser & source() const OPENVRML_NOTHROW;
    };


    class OPENVRML_API browser_listener {
        friend class browser;

    public:
        virtual ~browser_listener() OPENVRML_NOTHROW = 0;

    private:
        OPENVRML_LOCAL void browser_changed(const browser_event & event);
        virtual void do_browser_changed(const browser_event & event) = 0;
    };


    class viewer;
    class scene;
    class null_node_metatype;
    class null_node_type;
    class externproto_node;

    class OPENVRML_API browser : boost::noncopyable {
        friend class scene;
        friend class script_node;
        friend class externproto_node;
        friend bool OPENVRML_API operator==(const node_type &, const node_type &)
            OPENVRML_NOTHROW;

        struct root_scene_loader;
        class externproto_node_metatype;
        class externproto_node_type;
        struct vrml97_parse_actions;
        struct x3d_vrml_parse_actions;

        class OPENVRML_LOCAL node_metatype_map {
            mutable read_write_mutex mutex_;
            typedef std::map<std::string, boost::shared_ptr<node_metatype> >
                map_t;
            map_t map_;

        public:
            node_metatype_map();
            ~node_metatype_map() OPENVRML_NOTHROW;

            node_metatype_map & operator=(const node_metatype_map & ncm);

            void init(viewpoint_node * initial_viewpoint, double timestamp);

            const boost::shared_ptr<openvrml::node_metatype>
                insert(const std::string & id,
                       const boost::shared_ptr<openvrml::node_metatype> & metatype);

            bool remove(const std::string & id);

            const boost::shared_ptr<node_metatype>
                find(const std::string & id) const;

            const std::vector<node_metatype_id>
                node_metatype_ids(const openvrml::node_metatype & metatype) const
                OPENVRML_THROW1(std::bad_alloc);

            void render(viewer & v);

            void shutdown(double timestamp) OPENVRML_NOTHROW;

        private:
            //
            // No convenient way to make copy-construction thread-safe, and we
            // don't really need it.
            //
            node_metatype_map(const node_metatype_map & map);
        };

        OPENVRML_LOCAL static void
            parse_vrml(
                std::istream & in,
                const std::string & uri,
                const std::string & type,
                const openvrml::scene & scene,
                std::vector<boost::intrusive_ptr<openvrml::node> > & nodes,
                std::map<std::string, std::string> & meta);

        const boost::scoped_ptr<null_node_metatype> null_node_metatype_;
        const boost::scoped_ptr<null_node_type> null_node_type_;

        read_write_mutex load_root_scene_thread_mutex_;
        boost::scoped_ptr<boost::thread> load_root_scene_thread_;

        boost::thread_group load_proto_thread_group_;
        node_metatype_map node_metatype_map_;
        script_node_metatype script_node_metatype_;
        resource_fetcher & fetcher_;

        mutable read_write_mutex scene_mutex_;
        boost::scoped_ptr<scene> scene_;

        const boost::intrusive_ptr<viewpoint_node> default_viewpoint_;

        mutable read_write_mutex active_viewpoint_mutex_;
        viewpoint_node * active_viewpoint_;

        const boost::intrusive_ptr<navigation_info_node>
            default_navigation_info_;

        mutable read_write_mutex active_navigation_info_mutex_;
        navigation_info_node * active_navigation_info_;

        mutable read_write_mutex viewpoint_list_mutex_;
        std::list<viewpoint_node *> viewpoint_list_;

        read_write_mutex scoped_lights_mutex_;
        std::list<scoped_light_node *> scoped_lights_;

        read_write_mutex scripts_mutex_;
        std::list<script_node *> scripts_;

        read_write_mutex timers_mutex_;
        std::list<time_dependent_node *> timers_;

        read_write_mutex listeners_mutex_;
        std::set<browser_listener *> listeners_;

        bool new_view;

        mutable read_write_mutex delta_time_mutex_;
        double delta_time;

        mutable read_write_mutex viewer_mutex_;
        openvrml::viewer * viewer_;

        bool modified_;
        mutable read_write_mutex modified_mutex_;

        mutable read_write_mutex frame_rate_mutex_;
        double frame_rate_;

        mutable boost::mutex out_mutex_;
        std::ostream * const out_;

        mutable boost::mutex err_mutex_;
        std::ostream * const err_;

    public:
        static double current_time() OPENVRML_NOTHROW;

        bool flags_need_updating;

        browser(resource_fetcher & fetcher,
                std::ostream & out,
                std::ostream & err)
            OPENVRML_THROW1(std::bad_alloc);
        virtual ~browser() OPENVRML_NOTHROW;

        void add_node_metatype(
            const node_metatype_id & id,
            const boost::shared_ptr<openvrml::node_metatype> & metatype)
            OPENVRML_THROW2(std::invalid_argument, std::bad_alloc);
        const boost::shared_ptr<openvrml::node_metatype>
        node_metatype(const node_metatype_id & id) const OPENVRML_NOTHROW;

        scene * root_scene() const OPENVRML_NOTHROW;

        const node_path find_node(const node & n) const
            OPENVRML_THROW1(std::bad_alloc);
        viewpoint_node & active_viewpoint() const OPENVRML_NOTHROW;
        void active_viewpoint(viewpoint_node & viewpoint) OPENVRML_NOTHROW;
        void reset_default_viewpoint() OPENVRML_NOTHROW;
        navigation_info_node & active_navigation_info() const OPENVRML_NOTHROW;
        void active_navigation_info(navigation_info_node & nav_info)
            OPENVRML_NOTHROW;
        void reset_default_navigation_info() OPENVRML_NOTHROW;
        void add_viewpoint(viewpoint_node & viewpoint)
            OPENVRML_THROW1(std::bad_alloc);
        void remove_viewpoint(viewpoint_node & viewpoint) OPENVRML_NOTHROW;
        const std::list<viewpoint_node *> viewpoints() const OPENVRML_NOTHROW;
        void viewer(openvrml::viewer * v) OPENVRML_THROW1(viewer_in_use);
        openvrml::viewer * viewer() const OPENVRML_NOTHROW;

        virtual const char * name() const OPENVRML_NOTHROW;
        virtual const char * version() const OPENVRML_NOTHROW;
        float current_speed();
        const std::string world_url() const OPENVRML_THROW1(std::bad_alloc);
        void set_world(resource_istream & in);
        void replace_world(
            const std::vector<boost::intrusive_ptr<node> > & nodes);
        void load_url(const std::vector<std::string> & url,
                      const std::vector<std::string> & parameter)
            OPENVRML_THROW2(std::bad_alloc, boost::thread_resource_error);
        virtual void description(const std::string & description);
        const std::vector<boost::intrusive_ptr<node> >
        create_vrml_from_stream(std::istream & in,
                                const std::string & type = vrml_media_type);
        void create_vrml_from_url(const std::vector<std::string> & url,
                                  const boost::intrusive_ptr<node> & node,
                                  const std::string & event)
            OPENVRML_THROW3(unsupported_interface, std::bad_cast,
                            boost::thread_resource_error);
        bool add_listener(browser_listener & listener)
            OPENVRML_THROW1(std::bad_alloc);
        bool remove_listener(browser_listener & listener) OPENVRML_NOTHROW;

        void sensitive_event(node * object, double timestamp,
                             bool is_over, bool is_active,
                             const double (&point)[3]);

        double frame_rate() const;

        bool update(double current_time = -1.0);

        void render();

        void modified(bool value);
        bool modified() const;

        void delta(double d);
        double delta() const;

        void add_scoped_light(scoped_light_node &);
        void remove_scoped_light(scoped_light_node &);

        void add_time_dependent(time_dependent_node & n);
        void remove_time_dependent(time_dependent_node & n);

        void add_script(script_node &);
        void remove_script(script_node &);

        void update_flags();

        void out(const std::string & str) const;
        void err(const std::string & str) const;

    protected:
        bool headlight_on();
    };


    class OPENVRML_API scene : boost::noncopyable {
        struct vrml_from_url_creator;

        openvrml::browser * const browser_;
        scene * const parent_;

        mutable read_write_mutex nodes_mutex_;
        std::vector<boost::intrusive_ptr<node> > nodes_;

        mutable read_write_mutex url_mutex_;
        std::string url_;

        mutable read_write_mutex meta_mutex_;
        std::map<std::string, std::string> meta_;

        boost::thread_group stream_reader_threads_;

    public:
        explicit scene(openvrml::browser & browser, scene * parent = 0)
            OPENVRML_NOTHROW;
        virtual ~scene() OPENVRML_NOTHROW;

        openvrml::browser & browser() const OPENVRML_NOTHROW;
        scene * parent() const OPENVRML_NOTHROW;
        void load(resource_istream & in);
        void initialize(double timestamp) OPENVRML_THROW1(std::bad_alloc);
        const std::string meta(const std::string & key) const
            OPENVRML_THROW2(std::invalid_argument, std::bad_alloc);
        void meta(const std::string & key, const std::string & value)
            OPENVRML_THROW1(std::bad_alloc);
        const std::vector<std::string> meta_keys() const
            OPENVRML_THROW1(std::bad_alloc);
        const std::vector<boost::intrusive_ptr<node> > nodes() const
            OPENVRML_THROW1(std::bad_alloc);
        void nodes(const std::vector<boost::intrusive_ptr<node> > & n)
            OPENVRML_THROW2(std::invalid_argument, std::bad_alloc);
        const scope * root_scope() const OPENVRML_NOTHROW;
        const std::string url() const OPENVRML_THROW1(std::bad_alloc);
        void render(openvrml::viewer & viewer, rendering_context context);
        void load_url(const std::vector<std::string> & url,
                      const std::vector<std::string> & parameter)
            OPENVRML_THROW1(std::bad_alloc);
        std::auto_ptr<resource_istream>
        get_resource(const std::vector<std::string> & url) const
            OPENVRML_THROW2(no_alternative_url, std::bad_alloc);
        void read_stream(std::auto_ptr<resource_istream> in,
                         std::auto_ptr<stream_listener> listener);
        void create_vrml_from_url(const std::vector<std::string> & url,
                                  const boost::intrusive_ptr<node> & node,
                                  const std::string & event)
            OPENVRML_THROW3(unsupported_interface, std::bad_cast,
                            boost::thread_resource_error);
        void shutdown(double timestamp) OPENVRML_NOTHROW;

    private:
        virtual void scene_loaded();
    };
}

# endif // OPENVRML_BROWSER_H
