// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
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
#   include <list>
#   include <map>
#   include <openvrml/common.h>
#   include <openvrml/node_class_ptr.h>
#   include <openvrml/script.h>

namespace openvrml {

    class invalid_vrml : public std::runtime_error {
    public:
        invalid_vrml();
        virtual ~invalid_vrml() throw ();
    };

    class viewer;
    class ProtoNode;
    class scene;
    class Vrml97RootScope;
    class null_node_class;
    class null_node_type;

    class browser {
        friend class Vrml97Parser;
        friend class ProtoNodeClass;
        friend class Vrml97RootScope;

    public:
        enum cb_reason {
            destroy_world_id,
            replace_world_id
        };

        typedef void (*scene_cb)(cb_reason reason);

    private:
        std::auto_ptr<null_node_class> null_node_class_;
        std::auto_ptr<null_node_type> null_node_type_;
        typedef std::map<std::string, node_class_ptr> node_class_map_t;
        node_class_map_t node_class_map;
        script_node_class script_node_class_;
        scene * scene_;
        node_ptr default_viewpoint;
        viewpoint_node * active_viewpoint_;
        std::list<viewpoint_node *> viewpoint_list;
        typedef std::list<node_ptr> bind_stack_t;
        bind_stack_t navigation_info_stack;
        std::list<node *> navigation_infos;
        std::list<node *> scoped_lights;
        std::list<script_node *> scripts;
        std::list<node *> timers;
        std::list<node *> audio_clips;
        std::list<node *> movies;
        std::list<ProtoNode *> proto_node_list;
        bool modified_;
        bool new_view;
        double delta_time;

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
        void add_viewpoint(viewpoint_node & viewpoint) throw (std::bad_alloc);
        void remove_viewpoint(viewpoint_node & viewpoint) throw ();
        const std::list<viewpoint_node *> & viewpoints() const throw ();

        virtual const char * name() const throw ();
        virtual const char * version() const throw ();
        float current_speed();
        const std::string world_url() const throw (std::bad_alloc);
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

        void render(openvrml::viewer & viewer);

        void modified(bool value);
        bool modified() const;

        void delta(double d);
        double delta() const;

        void add_navigation_info(vrml97_node::navigation_info_node &);
        void remove_navigation_info(vrml97_node::navigation_info_node &);
        vrml97_node::navigation_info_node *bindable_navigation_info_top();
        void bindable_push(vrml97_node::navigation_info_node *);
        void bindable_remove(vrml97_node::navigation_info_node *);

        void add_scoped_light(vrml97_node::abstract_light_node &);
        void remove_scoped_light(vrml97_node::abstract_light_node &);

        void add_time_sensor(vrml97_node::time_sensor_node &);
        void remove_time_sensor(vrml97_node::time_sensor_node &);

        void add_audio_clip(vrml97_node::audio_clip_node &);
        void remove_audio_clip(vrml97_node::audio_clip_node &);

        void add_movie(vrml97_node::movie_texture_node &);
        void remove_movie(vrml97_node::movie_texture_node &);

        void add_proto(ProtoNode & node);
        void remove_proto(ProtoNode & node);
        void add_script(script_node &);
        void remove_script(script_node &);

        void update_flags();

    protected:
        bool headlight_on();
        void do_callbacks(cb_reason reason);

        const node_ptr bindable_top(const bind_stack_t & stack);
        void bindable_push(bind_stack_t & stack, const node_ptr & node);
        void bindable_remove(bind_stack_t & stack, const node_ptr & node);

    private:
        // Not copyable.
        browser(const browser &);
        browser & operator=(const browser &);

        void init_node_class_map();
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


    class scene {
        std::vector<node_ptr> nodes_;
        std::string url_;

    public:
        openvrml::browser & browser;
        scene * const parent;

        scene(openvrml::browser & browser,
              const std::vector<std::string> & url,
              scene * parent = 0)
            throw (invalid_vrml, std::bad_alloc);

        void initialize(double timestamp) throw (std::bad_alloc);
        const std::vector<node_ptr> & nodes() const throw ();
        const std::string url() const throw (std::bad_alloc);
        void render(openvrml::viewer & viewer, rendering_context context);
        void load_url(const std::vector<std::string> & url,
                      const std::vector<std::string> & parameter)
                throw (std::bad_alloc);
        void shutdown(double timestamp) throw ();

    private:
        // Noncopyable.
        scene(const scene &);
        scene & operator=(const scene &);
    };

    inline const std::vector<node_ptr> & scene::nodes() const throw()
    {
        return this->nodes_;
    }
}

# endif // OPENVRML_BROWSER_H
