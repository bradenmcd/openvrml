// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005  Braden McDaniel
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

# ifndef OPENVRML_BROWSER_H
#   define OPENVRML_BROWSER_H

#   include <boost/thread.hpp>
#   include <openvrml/script.h>

namespace openvrml {

    OPENVRML_API extern const char vrml_media_type[11];
    OPENVRML_API extern const char x_vrml_media_type[15];
    OPENVRML_API extern const char x3d_vrml_media_type[15];

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

    OPENVRML_API void read_stream(std::auto_ptr<resource_istream> in,
                                  std::auto_ptr<stream_listener> listener);


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

    class OPENVRML_API browser : boost::noncopyable {
        friend class Vrml97Parser;
        friend class X3DVrmlParser;
        friend bool OPENVRML_API operator==(const node_type &, const node_type &)
            OPENVRML_NOTHROW;

        struct root_scene_loader;
        struct vrml_from_url_creator;

        class OPENVRML_LOCAL node_metatype_map {
            mutable boost::mutex mutex_;
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

        mutable boost::recursive_mutex mutex_;
        std::auto_ptr<null_node_metatype> null_node_metatype_;
        std::auto_ptr<null_node_type> null_node_type_;
        boost::thread_group load_proto_thread_group_;
        node_metatype_map node_metatype_map_;
        script_node_metatype script_node_metatype_;
        boost::scoped_ptr<scene> scene_;
        const boost::intrusive_ptr<node> default_viewpoint_;
        viewpoint_node * active_viewpoint_;
        const boost::intrusive_ptr<node> default_navigation_info_;
        navigation_info_node * active_navigation_info_;
        std::list<viewpoint_node *> viewpoint_list;
        std::list<scoped_light_node *> scoped_lights;
        std::list<script_node *> scripts;
        std::list<time_dependent_node *> timers;
        std::set<browser_listener *> listeners_;
        bool new_view;
        double delta_time;
        openvrml::viewer * viewer_;

        bool modified_;
        mutable boost::mutex modified_mutex_;

        mutable boost::mutex out_mutex_;
        std::ostream * const out_;

        mutable boost::mutex err_mutex_;
        std::ostream * const err_;

    protected:
        double frame_rate_;

    public:
        static double current_time() OPENVRML_NOTHROW;

        bool flags_need_updating;

        browser(std::ostream & out, std::ostream & err)
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
        const std::list<viewpoint_node *> & viewpoints() const
            OPENVRML_NOTHROW;
        void viewer(openvrml::viewer * v) OPENVRML_THROW1(viewer_in_use);
        openvrml::viewer * viewer() const OPENVRML_NOTHROW;
        std::auto_ptr<resource_istream> get_resource(const std::string & uri);

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

    private:
        virtual std::auto_ptr<resource_istream>
        do_get_resource(const std::string & uri) = 0;
    };


    class OPENVRML_API scene : boost::noncopyable {
        openvrml::browser * const browser_;
        scene * const parent_;

        mutable boost::recursive_mutex nodes_mutex_;
        std::vector<boost::intrusive_ptr<node> > nodes_;

        mutable boost::mutex url_mutex_;
        std::string url_;

        mutable boost::mutex meta_mutex_;
        std::map<std::string, std::string> meta_;

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
        const std::vector<boost::intrusive_ptr<node> > & nodes() const
            OPENVRML_NOTHROW;
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
        void shutdown(double timestamp) OPENVRML_NOTHROW;

    private:
        virtual void scene_loaded();
    };
}

# endif // OPENVRML_BROWSER_H
