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

#   include <openvrml/script.h>

namespace openvrml {

    OPENVRML_API extern const char vrml_media_type[11];
    OPENVRML_API extern const char x_vrml_media_type[15];
    OPENVRML_API extern const char x3d_vrml_media_type[15];

    OPENVRML_API
    std::auto_ptr<node_type_decls> profile(const std::string & profile_id)
        OPENVRML_THROW2(std::invalid_argument, std::bad_alloc);

    OPENVRML_API
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


    namespace local {
        class node_metatype_registry_impl;
    }

    class OPENVRML_API node_metatype_registry : boost::noncopyable {
        friend class browser;
        friend bool OPENVRML_API operator==(const node_type &,
                                            const node_type &)
            OPENVRML_NOTHROW;

    private:
        boost::scoped_ptr<local::node_metatype_registry_impl> impl_;

        explicit node_metatype_registry(openvrml::browser & b);

    public:
        ~node_metatype_registry() OPENVRML_NOTHROW;

        openvrml::browser & browser() const OPENVRML_NOTHROW;

        void register_node_metatype(
            const std::string & id,
            const boost::shared_ptr<node_metatype> & metatype)
            OPENVRML_THROW2(std::invalid_argument, std::bad_alloc);
    };

    typedef void (*register_node_metatypes_func)(node_metatype_registry &);


    class viewer;
    class scene;

    namespace local {
        struct vrml97_parse_actions;
        struct x3d_vrml_parse_actions;

        void parse_vrml(std::istream & in,
                        const std::string & uri,
                        const std::string & type,
                        const openvrml::scene & scene,
                        std::vector<boost::intrusive_ptr<node> > & nodes,
                        std::map<std::string, std::string> & meta);

        class externproto_node;
        class externproto_node_type;
        class externproto_node_metatype;
    }

    class OPENVRML_API browser : boost::noncopyable {
        friend class scene;
        friend class script_node;
        friend bool OPENVRML_API operator==(const node_type &,
                                            const node_type &)
            OPENVRML_NOTHROW;

        struct root_scene_loader;
        friend class local::externproto_node;
        friend class local::externproto_node_type;
        friend class local::externproto_node_metatype;
        friend struct local::vrml97_parse_actions;
        friend struct local::x3d_vrml_parse_actions;

        friend
        void
        local::parse_vrml(std::istream & in,
                          const std::string & uri,
                          const std::string & type,
                          const openvrml::scene & scene,
                          std::vector<boost::intrusive_ptr<node> > & nodes,
                          std::map<std::string, std::string> & meta);

        mutable boost::shared_mutex node_metatype_registry_mutex_;
        boost::scoped_ptr<node_metatype_registry> node_metatype_registry_;

        const boost::scoped_ptr<null_node_metatype> null_node_metatype_;
        const boost::scoped_ptr<null_node_type> null_node_type_;

        boost::shared_mutex load_root_scene_thread_mutex_;
        boost::scoped_ptr<boost::thread> load_root_scene_thread_;

        boost::thread_group load_proto_thread_group_;
        script_node_metatype script_node_metatype_;
        resource_fetcher & fetcher_;

        mutable boost::shared_mutex scene_mutex_;
        boost::scoped_ptr<scene> scene_;

        const boost::intrusive_ptr<viewpoint_node> default_viewpoint_;

        mutable boost::shared_mutex active_viewpoint_mutex_;
        viewpoint_node * active_viewpoint_;

        const boost::intrusive_ptr<navigation_info_node>
            default_navigation_info_;

        mutable boost::shared_mutex active_navigation_info_mutex_;
        navigation_info_node * active_navigation_info_;

        mutable boost::shared_mutex viewpoint_list_mutex_;
        std::list<viewpoint_node *> viewpoint_list_;

        boost::shared_mutex scoped_lights_mutex_;
        std::list<scoped_light_node *> scoped_lights_;

        boost::shared_mutex scripts_mutex_;
        std::list<script_node *> scripts_;

        boost::shared_mutex timers_mutex_;
        std::list<time_dependent_node *> timers_;

        boost::shared_mutex listeners_mutex_;
        std::set<browser_listener *> listeners_;

        bool new_view;

        mutable boost::shared_mutex delta_time_mutex_;
        double delta_time;

        mutable boost::shared_mutex viewer_mutex_;
        openvrml::viewer * viewer_;

        bool modified_;
        mutable boost::shared_mutex modified_mutex_;

        mutable boost::shared_mutex frame_rate_mutex_;
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
}

# endif // OPENVRML_BROWSER_H
