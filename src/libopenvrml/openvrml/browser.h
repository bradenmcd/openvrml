// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004  Braden McDaniel
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

#   include <cstddef>
#   include <cstdio>
#   include <iosfwd>
#   include <list>
#   include <map>
#   include <string>
#   include <boost/scoped_ptr.hpp>
#   include <boost/utility.hpp>
#   include <boost/thread/recursive_mutex.hpp>
#   include <openvrml/script.h>

namespace openvrml {

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


    class viewer;
    class scene;
    class Vrml97RootScope;
    class null_node_class;
    class null_node_type;

    class browser : boost::noncopyable {
        friend class Vrml97Parser;
        friend class Vrml97RootScope;

    public:
        enum cb_reason {
            destroy_world_id,
            replace_world_id
        };

        typedef void (*scene_cb)(cb_reason reason);

    private:
        mutable boost::recursive_mutex mutex_;
        std::auto_ptr<null_node_class> null_node_class_;
        std::auto_ptr<null_node_type> null_node_type_;
        typedef std::map<std::string, node_class_ptr> node_class_map_t;
        node_class_map_t node_class_map;
        script_node_class script_node_class_;
        boost::scoped_ptr<scene> scene_;
        const node_ptr default_viewpoint_;
        viewpoint_node * active_viewpoint_;
        const node_ptr default_navigation_info_;
        navigation_info_node * active_navigation_info_;
        std::list<viewpoint_node *> viewpoint_list;
        std::list<node *> scoped_lights;
        std::list<script_node *> scripts;
        std::list<node *> timers;
        std::list<node *> audio_clips;
        std::list<node *> movies;
        bool modified_;
        bool new_view;
        double delta_time;
        openvrml::viewer * viewer_;

    protected:
        typedef std::list<scene_cb> scene_cb_list_t;

        struct event {
            double timestamp;
            field_value * value;
            node_ptr to_node;
            std::string to_eventin;
        };

        scene_cb_list_t scene_callbacks;

        double frame_rate_;

        enum { max_events = 400 };
        event event_mem[max_events];
        size_t first_event;
        size_t last_event;

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

        virtual const char * name() const throw ();
        virtual const char * version() const throw ();
        float current_speed();
        const std::string world_url() const throw (std::bad_alloc);
        void world_url(const std::string & str)
            throw (invalid_url, std::bad_alloc);
        void replace_world(const std::vector<node_ptr> & nodes);
        virtual void load_url(const std::vector<std::string> & url,
                              const std::vector<std::string> & parameter)
            throw (std::bad_alloc);
        virtual void description(const std::string & description);
        const std::vector<node_ptr> create_vrml_from_stream(std::istream & in);
        void create_vrml_from_url(const std::vector<std::string> & url,
                                  const node_ptr & node,
                                  const std::string & event);

        void add_world_changed_callback(scene_cb);

        void sensitive_event(node * object, double timestamp,
                             bool is_over, bool is_active, double * point);

        void queue_event(double timestamp, field_value * value,
                         const node_ptr & toNode,
                         const std::string & to_eventin);

        bool events_pending();

        void flush_events();

        double frame_rate() const;

        bool update(double current_time = -1.0);

        void render();

        void modified(bool value);
        bool modified() const;

        void delta(double d);
        double delta() const;

        void add_scoped_light(vrml97_node::abstract_light_node &);
        void remove_scoped_light(vrml97_node::abstract_light_node &);

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
        void do_callbacks(cb_reason reason);

    private:
        void init_node_class_map();
    };


    class scene : boost::noncopyable {
        mutable boost::recursive_mutex mutex_;
        std::vector<node_ptr> nodes_;
        std::string url_;

    public:
        openvrml::browser & browser;
        scene * const parent;

        explicit scene(openvrml::browser & browser,
                       scene * parent = 0)
            throw ();
        scene(openvrml::browser & browser,
              const std::vector<std::string> & url,
              scene * parent = 0)
            throw (invalid_vrml, no_alternative_url, std::bad_alloc);

        void initialize(double timestamp) throw (std::bad_alloc);
        const std::vector<node_ptr> & nodes() const throw ();
        void nodes(const std::vector<node_ptr> & n) throw (std::bad_alloc);
        const std::string url() const throw (std::bad_alloc);
        void url(const std::string & str) throw (invalid_url, std::bad_alloc);
        void render(openvrml::viewer & viewer, rendering_context context);
        void load_url(const std::vector<std::string> & url,
                      const std::vector<std::string> & parameter)
                throw (std::bad_alloc);
        void shutdown(double timestamp) throw ();
    };

    inline const std::vector<node_ptr> & scene::nodes() const throw()
    {
        return this->nodes_;
    }


    class doc2;

    class doc : boost::noncopyable {
        char * url_;
        std::ostream * out_;
        FILE * fp_;
        char * tmpfile_; // Local copy of http: files

    public:
        explicit doc(const std::string & url = std::string(),
                     const doc * relative = 0);
        doc(const std::string & url, const doc2 * relative);
        ~doc();

        void seturl(const char * url, const doc * relative = 0);
        void seturl(const char * url, const doc2 * relative = 0);

        const char * url() const;          // "http://www.foo.com/dir/file.xyz#Viewpoint"
        const char * url_base() const;      // "file" or ""
        const char * url_ext() const;       // "xyz" or ""
        const char * url_path() const;      // "http://www.foo.com/dir/" or ""
        const char * url_protocol() const;  // "http"
        const char * url_modifier() const;  // "#Viewpoint" or ""

        const char * local_name();    // "/tmp/file.xyz" or NULL
        const char * local_path();    // "/tmp/" or NULL


        FILE * fopen(const char * mode);
        void fclose();

        std::ostream & output_stream();

    private:
        bool filename(char * fn, int nfn);
    };

    class doc2 : boost::noncopyable {
        std::string url_;
        char * tmpfile_;            // Local copy of http: files
        std::istream * istm_;
        std::ostream * ostm_;

    public:
        explicit doc2(const std::string & url = std::string(),
                      const doc2 * relative = 0);
        ~doc2();

        void seturl(const std::string & url, const doc2 * relative = 0);

        const std::string url() const;         // "http://www.foo.com/dir/file.xyz#Viewpoint"
        const std::string url_base() const;     // "file" or ""
        const std::string url_ext() const;      // "xyz" or ""
        const std::string url_path() const;     // "http://www.foo.com/dir/" or ""
        const std::string url_protocol() const; // "http"
        const std::string url_modifier() const; // "#Viewpoint" or ""

        const char * local_name();    // "/tmp/file.xyz" or NULL
        const char * local_path();    // "/tmp/" or NULL

        std::istream & input_stream();
        std::ostream & output_stream();

    private:
        bool filename(char * fn, size_t nfn);
    };
}

# endif // OPENVRML_BROWSER_H
