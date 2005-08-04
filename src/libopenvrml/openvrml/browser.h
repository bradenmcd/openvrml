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

#   include <boost/thread/read_write_mutex.hpp>
#   include <openvrml/script.h>

namespace openvrml {

    class resource_istream : public std::istream {
    public:
        virtual ~resource_istream() = 0;
        virtual const std::string url() const throw () = 0;
        virtual const std::string type() const throw () = 0;
        virtual bool data_available() const throw () = 0;

    protected:
        explicit resource_istream(std::streambuf * streambuf);
    };


    class stream_listener {
    public:
        virtual ~stream_listener() throw () = 0;

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

    void read_stream(std::auto_ptr<resource_istream> in,
                     std::auto_ptr<stream_listener> listener);


    class invalid_vrml : public std::runtime_error {
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


    class viewer_in_use : public std::invalid_argument {
    public:
        viewer_in_use();
        virtual ~viewer_in_use() throw ();
    };


    class bad_url : public std::runtime_error {
    public:
        bad_url(const std::string & message);
        virtual ~bad_url() throw ();
    };


    class invalid_url : public bad_url {
    public:
        invalid_url();
        virtual ~invalid_url() throw ();
    };


    class unreachable_url : public bad_url {
    public:
        unreachable_url();
        virtual ~unreachable_url() throw ();
    };


    class no_alternative_url : public bad_url {
    public:
        no_alternative_url();
        virtual ~no_alternative_url() throw ();
    };


    class browser;

    class browser_event {
        friend class browser;

    public:
        enum type_id {
            initialized = 1,
            shutdown = 2
        };

    private:
        browser * source_;
        type_id id_;

        browser_event(browser & b, type_id id) throw ();

    public:
        type_id id() const throw ();
        browser & source() const throw ();
    };


    class browser_listener {
        friend class browser;

    public:
        virtual ~browser_listener() throw () = 0;

    private:
        void browser_changed(const browser_event & event);
        virtual void do_browser_changed(const browser_event & event) = 0;
    };


    class viewer;
    class scene;
    class null_node_class;
    class null_node_type;

    class browser : boost::noncopyable {
        friend class Vrml97Parser;

        class node_class_map {
            mutable boost::mutex mutex_;
            typedef std::map<std::string, boost::shared_ptr<node_class> >
                map_t;
            map_t map_;

        public:
            explicit node_class_map(browser & b);

            node_class_map & operator=(const node_class_map & map);

            void init(viewpoint_node * initial_viewpoint, double timestamp);

            const boost::shared_ptr<node_class>
            insert(const std::string & id,
                   const boost::shared_ptr<node_class> & node_class);

            const boost::shared_ptr<node_class>
            find(const std::string & id) const;

            void render(viewer & v);

        private:
            //
            // No convenient way to make copy-construction thread-safe, and we
            // don't really need it.
            //
            node_class_map(const node_class_map & map);            
        };

        mutable boost::recursive_mutex mutex_;
        std::auto_ptr<null_node_class> null_node_class_;
        std::auto_ptr<null_node_type> null_node_type_;
        node_class_map node_class_map_;
        script_node_class script_node_class_;
        boost::scoped_ptr<scene> scene_;
        const node_ptr default_viewpoint_;
        viewpoint_node * active_viewpoint_;
        const node_ptr default_navigation_info_;
        navigation_info_node * active_navigation_info_;
        std::list<viewpoint_node *> viewpoint_list;
        std::list<scoped_light_node *> scoped_lights;
        std::list<script_node *> scripts;
        std::list<node *> timers;
        std::list<node *> audio_clips;
        std::list<node *> movies;
        std::set<browser_listener *> listeners_;
        bool new_view;
        double delta_time;
        openvrml::viewer * viewer_;

        bool modified_;
        mutable boost::mutex modified_mutex_;

    protected:
        double frame_rate_;

    public:
        static double current_time() throw ();

        std::ostream & out;
        std::ostream & err;
        bool flags_need_updating;

        browser(std::ostream & out, std::ostream & err) throw (std::bad_alloc);
        virtual ~browser() throw ();

        const std::vector<node_ptr> & root_nodes() const throw ();
        const node_path find_node(const node & n) const throw (std::bad_alloc);
        viewpoint_node & active_viewpoint() const throw ();
        void active_viewpoint(viewpoint_node & viewpoint) throw ();
        void reset_default_viewpoint() throw ();
        navigation_info_node & active_navigation_info() const throw ();
        void active_navigation_info(navigation_info_node & nav_info) throw ();
        void reset_default_navigation_info() throw ();
        void add_viewpoint(viewpoint_node & viewpoint) throw (std::bad_alloc);
        void remove_viewpoint(viewpoint_node & viewpoint) throw ();
        const std::list<viewpoint_node *> & viewpoints() const throw ();
        void viewer(openvrml::viewer * v) throw (viewer_in_use);
        openvrml::viewer * viewer() const throw ();
        std::auto_ptr<resource_istream> get_resource(const std::string & uri);

        virtual const char * name() const throw ();
        virtual const char * version() const throw ();
        float current_speed();
        const std::string world_url() const throw (std::bad_alloc);
        void world_url(const std::string & str)
            throw (invalid_url, std::bad_alloc);
        void replace_world(const std::vector<node_ptr> & nodes);
        void load_url(const std::vector<std::string> & url,
                      const std::vector<std::string> & parameter)
            throw (std::bad_alloc);
        virtual void description(const std::string & description);
        const std::vector<node_ptr> create_vrml_from_stream(std::istream & in);
        void create_vrml_from_url(const std::vector<std::string> & url,
                                  const node_ptr & node,
                                  const std::string & event);
        bool add_listener(browser_listener & listener) throw (std::bad_alloc);
        bool remove_listener(browser_listener & listener) throw ();

        void sensitive_event(node * object, double timestamp,
                             bool is_over, bool is_active, double * point);

        double frame_rate() const;

        bool update(double current_time = -1.0);

        void render();

        void modified(bool value);
        bool modified() const;

        void delta(double d);
        double delta() const;

        void add_scoped_light(scoped_light_node &);
        void remove_scoped_light(scoped_light_node &);

        void add_time_sensor(vrml97_node::time_sensor_node &);
        void remove_time_sensor(vrml97_node::time_sensor_node &);

        void add_audio_clip(vrml97_node::audio_clip_node &);
        void remove_audio_clip(vrml97_node::audio_clip_node &);

        void add_movie(vrml97_node::movie_texture_node &);
        void remove_movie(vrml97_node::movie_texture_node &);

        void add_script(script_node &);
        void remove_script(script_node &);

        void update_flags();

    protected:
        bool headlight_on();

    private:
        std::auto_ptr<scope>
        create_root_scope(const std::string & uri) throw (std::bad_alloc);

        virtual std::auto_ptr<resource_istream>
        do_get_resource(const std::string & uri) = 0;
    };


    class scene : boost::noncopyable {
        struct load_scene;

        openvrml::browser * const browser_;
        scene * const parent_;

        mutable boost::recursive_mutex nodes_mutex_;
        std::vector<node_ptr> nodes_;

        mutable boost::read_write_mutex url_mutex_;
        std::string url_;

    public:
        explicit scene(openvrml::browser & browser, scene * parent = 0)
            throw ();
        virtual ~scene() throw ();

        openvrml::browser & browser() const throw ();
        scene * parent() const throw ();
        void load(const std::vector<std::string> & url)
            throw (boost::thread_resource_error, std::bad_alloc);
        void initialize(double timestamp) throw (std::bad_alloc);
        const std::vector<node_ptr> & nodes() const throw ();
        void nodes(const std::vector<node_ptr> & n) throw (std::bad_alloc);
        const std::string url() const throw (std::bad_alloc);
        void url(const std::string & str) throw (invalid_url, std::bad_alloc);
        void render(openvrml::viewer & viewer, rendering_context context);
        void load_url(const std::vector<std::string> & url,
                      const std::vector<std::string> & parameter)
            throw (std::bad_alloc);
        std::auto_ptr<resource_istream>
        get_resource(const std::vector<std::string> & url) const
            throw (no_alternative_url, std::bad_alloc);
        void shutdown(double timestamp) throw ();

    private:
        virtual void scene_loaded();
    };
}

# endif // OPENVRML_BROWSER_H
