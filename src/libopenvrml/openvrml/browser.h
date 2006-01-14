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

#   include <openvrml/script.h>

namespace openvrml {

    class OPENVRML_API resource_istream : public std::istream {
    public:
        virtual ~resource_istream() = 0;

        const std::string url() const throw (std::bad_alloc);
        const std::string type() const throw (std::bad_alloc);
        bool data_available() const throw ();

    protected:
        explicit resource_istream(std::streambuf * streambuf);

    private:
        virtual const std::string do_url() const throw (std::bad_alloc) = 0;
        virtual const std::string do_type() const throw (std::bad_alloc) = 0;
        virtual bool do_data_available() const throw () = 0;
    };


    class OPENVRML_API stream_listener {
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
        explicit bad_media_type(const std::string & received_type) throw ();
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

        OPENVRML_LOCAL browser_event(browser & b, type_id id) throw ();

    public:
        type_id id() const throw ();
        browser & source() const throw ();
    };


    class OPENVRML_API browser_listener {
        friend class browser;

    public:
        virtual ~browser_listener() throw () = 0;

    private:
        OPENVRML_LOCAL void browser_changed(const browser_event & event);
        virtual void do_browser_changed(const browser_event & event) = 0;
    };


    enum profile_id {
        invalid_profile_id,
        vrml97_profile_id,
        x3d_core_profile_id,
        x3d_interchange_profile_id,
        x3d_interactive_profile_id,
        x3d_mpeg4_interactive_profile_id,
        x3d_immersive_profile_id,
        x3d_full_profile_id
    };


    class OPENVRML_API invalid_profile : public std::logic_error {
    public:
        invalid_profile() throw ();
        virtual ~invalid_profile() throw ();
    };


    class OPENVRML_API node_class_id {
        std::string id_;

    public:
        node_class_id(const char * id)
            throw (std::invalid_argument, std::bad_alloc);
        node_class_id(const std::string & id)
            throw (std::invalid_argument, std::bad_alloc);
        operator std::string() const;
    };


    class viewer;
    class scene;
    class null_node_class;
    class null_node_type;

    class OPENVRML_API browser : boost::noncopyable {
        friend class Vrml97Parser;
        friend class X3DVrmlParser;

        struct root_scene_loader;
        struct vrml_from_url_creator;

        class node_class_map {
            mutable boost::mutex mutex_;
            typedef std::map<std::string, boost::shared_ptr<node_class> >
                map_t;
            map_t map_;

        public:
            node_class_map();

            node_class_map & operator=(const node_class_map & ncm);

            void init(viewpoint_node * initial_viewpoint, double timestamp);

            const boost::shared_ptr<openvrml::node_class>
            insert(const std::string & id,
                   const boost::shared_ptr<openvrml::node_class> & node_class);

            bool remove(const std::string & id);

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

    protected:
        double frame_rate_;

    public:
        static double current_time() throw ();

        std::ostream & out;
        std::ostream & err;
        bool flags_need_updating;

        browser(std::ostream & out, std::ostream & err) throw (std::bad_alloc);
        browser(profile_id profile, std::ostream & out, std::ostream & err)
            throw (std::bad_alloc);
        virtual ~browser() throw ();

        void add_node_class(const node_class_id & id,
                            const boost::shared_ptr<openvrml::node_class> & nc)
            throw (std::invalid_argument, std::bad_alloc);
        const boost::shared_ptr<openvrml::node_class>
        node_class(const node_class_id & id) const throw ();

        const std::vector<boost::intrusive_ptr<node> > & root_nodes() const
            throw ();
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
        void set_world(resource_istream & in);
        void replace_world(
            const std::vector<boost::intrusive_ptr<node> > & nodes);
        void load_url(const std::vector<std::string> & url,
                      const std::vector<std::string> & parameter)
            throw (std::bad_alloc);
        virtual void description(const std::string & description);
        const std::vector<boost::intrusive_ptr<node> >
        create_vrml_from_stream(std::istream & in);
        void create_vrml_from_url(const std::vector<std::string> & url,
                                  const boost::intrusive_ptr<node> & node,
                                  const std::string & event)
            throw (unsupported_interface, std::bad_cast,
                   boost::thread_resource_error);
        bool add_listener(browser_listener & listener) throw (std::bad_alloc);
        bool remove_listener(browser_listener & listener) throw ();

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

    protected:
        bool headlight_on();

    private:
        virtual std::auto_ptr<resource_istream>
        do_get_resource(const std::string & uri) = 0;
    };


    class OPENVRML_API scene : boost::noncopyable {
        friend class X3DVrmlParser;

        struct load_scene;

        openvrml::browser * const browser_;
        scene * const parent_;

        profile_id profile_;

        mutable boost::recursive_mutex nodes_mutex_;
        std::vector<boost::intrusive_ptr<node> > nodes_;

        mutable boost::mutex url_mutex_;
        std::string url_;

    public:
        explicit scene(openvrml::browser & browser,
                       profile_id profile = invalid_profile_id,
                       scene * parent = 0)
            throw ();
        scene(openvrml::browser & browser, scene * parent) throw ();
        virtual ~scene() throw ();

        openvrml::browser & browser() const throw ();
        scene * parent() const throw ();
        profile_id profile() const throw ();
        void load(const std::vector<std::string> & url)
            throw (boost::thread_resource_error, std::bad_alloc);
        void load(resource_istream & in);
        void initialize(double timestamp) throw (std::bad_alloc);
        const std::vector<boost::intrusive_ptr<node> > & nodes() const
            throw ();
        void nodes(const std::vector<boost::intrusive_ptr<node> > & n)
            throw (std::bad_alloc);
        const std::string url() const throw (std::bad_alloc);
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
