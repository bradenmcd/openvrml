// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML Mozilla plug-in
// Copyright 2004  Braden N. McDaniel
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

# include <cstdlib>
# include <boost/lexical_cast.hpp>
# include <boost/spirit.hpp>
# include <boost/spirit/phoenix.hpp>
# include <boost/thread/condition.hpp>
# include <boost/thread/mutex.hpp>
# include <boost/thread/thread.hpp>
# include <unistd.h>
# include <argp.h>
# include <X11/keysym.h>
# include <gdk/gdkx.h>
# include <gtk/gtk.h>
# include <gtk/gtkgl.h>
# include <openvrml/browser.h>
# include <openvrml/gl/viewer.h>

namespace {

    const char application_name[] = "OpenVRML Player";
}

extern "C" {
    const char * argp_program_version = "OpenVRML Player " PACKAGE_VERSION;
    const char * argp_program_bug_address = PACKAGE_BUGREPORT;

    error_t parse_opt(int key, char * arg, argp_state * state);

    gboolean realize(GtkWidget * widget, GdkEvent * event, gpointer data);
    gboolean expose_event(GtkWidget * widget, GdkEventExpose * event,
                          gpointer data);
    gboolean configure_event(GtkWidget * widget, GdkEventConfigure * event,
                             gpointer data);
    gboolean key_press_event(GtkWidget * widget, GdkEventKey * event,
                             gpointer data);
    gboolean button_press_event(GtkWidget * widget, GdkEventButton * event,
                                gpointer data);
    gboolean button_release_event(GtkWidget * widget, GdkEventButton * event,
                                  gpointer data);
    gboolean motion_notify_event(GtkWidget * widget, GdkEventMotion * event,
                                 gpointer data);
}

namespace {

    enum option_id {
        gtk_socket_id_id = 128,
        read_fd_id,
        write_fd_id
    };

    argp_option options[] = {
        {
            "gtk-socket-id",
            gtk_socket_id_id,
            "GTK_SOCKET_ID",
            0,
            "GtkSocket id"
        },
        {
            "read-fd",
            read_fd_id,
            "READ_FD",
            0,
            "file descriptor for reading commands"
        },
        {
            "write-fd",
            write_fd_id,
            "WRITE_FD",
            0,
            "file descriptor for writing commands"
        },
        {}
    };

    struct arguments {
        GdkNativeWindow socket_id;
        int read_fd;
        int write_fd;
    };

    class GtkGLViewer : public openvrml::gl::viewer {
        static GdkGLConfig * gl_config;

        GtkWidget * drawing_area;
        guint timer;

    public:
        bool redrawNeeded;

        explicit GtkGLViewer(GtkContainer & container);
        virtual ~GtkGLViewer() throw ();

        void timer_update();

    protected:
        //
        // Implement pure virtual methods from openvrml::gl::viewer.
        //
        virtual void post_redraw();
        virtual void set_cursor(openvrml::gl::viewer::cursor_style);
        virtual void swap_buffers();
        virtual void set_timer(double);
    };


    template <typename T, size_t BufferSize>
    class bounded_buffer {
        mutable boost::mutex mutex_;
        boost::condition buffer_not_full_, buffer_not_empty_;

        T buf_[BufferSize];
        size_t begin_, end_, buffered_;

    public:
        bounded_buffer();
        void put(const T & c);
        const T get();
        size_t buffered() const;
    };

    template <typename T, size_t BufferSize>
    bounded_buffer<T, BufferSize>::bounded_buffer():
        begin_(0),
        end_(0),
        buffered_(0)
    {}

    template <typename T, size_t BufferSize>
    void bounded_buffer<T, BufferSize>::put(const T & c)
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        while (this->buffered_ == BufferSize) {
            this->buffer_not_full_.wait(lock);
        }
        this->buf_[this->end_] = c;
        this->end_ = (this->end_ + 1) % BufferSize;
        ++this->buffered_;
        this->buffer_not_empty_.notify_one();
    }

    template <typename T, size_t BufferSize>
    const T bounded_buffer<T, BufferSize>::get()
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        while (this->buffered_ == 0) {
            this->buffer_not_empty_.wait(lock);
        }
        T c = this->buf_[this->begin_];
        this->begin_ = (this->begin_ + 1) % BufferSize;
        --this->buffered_;
        this->buffer_not_full_.notify_one();
        return c;
    }

    template <typename T, size_t BufferSize>
    size_t bounded_buffer<T, BufferSize>::buffered() const
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        return this->buffered_;
    }


    extern "C" gboolean command_data_available(GIOChannel * source,
                                               GIOCondition condition,
                                               gpointer data);

    class command_streambuf : boost::noncopyable, public std::streambuf {
        friend gboolean command_data_available(GIOChannel * source,
                                               GIOCondition condition,
                                               gpointer data);

        bounded_buffer<int_type, 64> source_buffer_;
        char_type c_;

    protected:
        virtual int_type underflow();

    public:
        command_streambuf();
    };

    command_streambuf::command_streambuf()
    {
        this->setg(&this->c_, &this->c_, &this->c_);
    }

    command_streambuf::int_type command_streambuf::underflow()
    {
        int_type c = this->source_buffer_.get();
        if (c == traits_type::eof()) { return traits_type::eof(); }
        this->c_ = c;
        this->setg(&this->c_, &this->c_, &this->c_ + 1);
        return *this->gptr();
    }


    class command_istream : boost::noncopyable, public std::istream {
        command_streambuf buf_;

    public:
        command_istream();
    };

    command_istream::command_istream():
        std::istream(&this->buf_)
    {}

    class plugin_streambuf : public std::streambuf {
        friend class command_istream_reader;

        mutable boost::mutex mutex_;
        bool initialized_;
        mutable boost::condition streambuf_initialized_;
        const std::string url_;
        std::string type_;
        bounded_buffer<int_type, 64> buf_;
        char_type c_;
        bool npstream_destroyed_;

    protected:
        virtual int_type underflow();

    public:
        explicit plugin_streambuf(const std::string & url = "");
        void init(const std::string & type);
        const std::string & url() const;
        const std::string & type() const;
        bool data_available() const;
        void npstream_destroyed();
    };

    plugin_streambuf::plugin_streambuf(const std::string & url):
        initialized_(false),
        url_(url),
        c_(traits_type::not_eof(this->c_)),
        npstream_destroyed_(false)
    {
        this->setg(&this->c_, &this->c_, &this->c_);
    }

    void plugin_streambuf::init(const std::string & type)
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        this->type_ = type;
        this->initialized_ = true;
        this->streambuf_initialized_.notify_all();
    }

    const std::string & plugin_streambuf::url() const
    {
        //
        // No need to lock or wait on init here; this->url_ is set in the
        // constructor and cannot be changed.
        //
        return this->url_;
    }

    const std::string & plugin_streambuf::type() const
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        while (!this->initialized_) {
            this->streambuf_initialized_.wait(lock);
        }
        return this->type_;
    }

    bool plugin_streambuf::data_available() const
    {
        return this->buf_.buffered() > 0;
    }

    plugin_streambuf::int_type plugin_streambuf::underflow()
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        while (!this->initialized_) {
            this->streambuf_initialized_.wait(lock);
        }
        if (this->c_ == traits_type::eof()) { return traits_type::eof(); }
        this->c_ = this->buf_.get();
        this->setg(&this->c_, &this->c_, &this->c_ + 1);
        return *this->gptr();
    }

    void plugin_streambuf::npstream_destroyed()
    {
        this->npstream_destroyed_ = true;
    }


    class browser : public openvrml::browser {
    public:
        browser();

    private:
        virtual std::auto_ptr<openvrml::resource_istream>
        do_get_resource(const std::string & uri);
    };

    class uninitialized_plugin_streambuf_set {
        mutable boost::mutex mutex_;
        typedef std::set<boost::shared_ptr<plugin_streambuf> > set_t;
        set_t set_;

        struct plugin_streambuf_has_url :
            std::unary_function<boost::shared_ptr<plugin_streambuf>, bool> {

            explicit plugin_streambuf_has_url(const std::string & url):
                url_(&url)
            {}

            bool
            operator()(const boost::shared_ptr<plugin_streambuf> & arg) const
            {
                return arg->url() == *this->url_;
            }

        private:
            const std::string * url_;
        };

    public:
        const boost::shared_ptr<plugin_streambuf>
        find(const std::string & url) const;
        bool insert(const boost::shared_ptr<plugin_streambuf> & streambuf);
        bool erase(const boost::shared_ptr<plugin_streambuf> & streambuf);
        size_t size() const;
        const boost::shared_ptr<plugin_streambuf> front() const;
    } uninitialized_plugin_streambuf_set_;

    const boost::shared_ptr<plugin_streambuf>
    uninitialized_plugin_streambuf_set::find(const std::string & url) const
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        set_t::const_iterator pos = find_if(this->set_.begin(),
                                            this->set_.end(),
                                            plugin_streambuf_has_url(url));
        return pos == this->set_.end()
            ? boost::shared_ptr<plugin_streambuf>()
            : *pos;
    }

    bool
    uninitialized_plugin_streambuf_set::
    insert(const boost::shared_ptr<plugin_streambuf> & streambuf)
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        return this->set_.insert(streambuf).second;
    }

    bool
    uninitialized_plugin_streambuf_set::
    erase(const boost::shared_ptr<plugin_streambuf> & streambuf)
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        return this->set_.erase(streambuf) == 1;
    }

    size_t uninitialized_plugin_streambuf_set::size() const
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        return this->set_.size();
    }

    const boost::shared_ptr<plugin_streambuf>
    uninitialized_plugin_streambuf_set::front() const
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        g_assert(!this->set_.empty());
        return *this->set_.begin();
    }

    typedef std::map<size_t, boost::shared_ptr<plugin_streambuf> >
        plugin_streambuf_map_t;

    plugin_streambuf_map_t plugin_streambuf_map;

    struct command_istream_reader {
        explicit command_istream_reader(command_istream & in,
                                        openvrml::browser & browser):
            in_(&in),
            browser_(&browser)
        {}

        void operator()() const
        {
            using std::string;

            char c;
            string command_line;
            while (getline(*this->in_, command_line)) {
                using std::istringstream;

                istringstream command_line_stream(command_line);
                string command;
                command_line_stream >> command;
                if (command == "new-stream") {
                    using boost::shared_ptr;

                    size_t stream_id;
                    std::string type, url;
                    command_line_stream >> stream_id >> type >> url;

                    shared_ptr<plugin_streambuf> streambuf =
                        uninitialized_plugin_streambuf_set_.find(url);

                    if (!streambuf) {
                        //
                        // If the world_url is empty, this is the primordial
                        // stream.
                        //
                        if (this->browser_->world_url().empty()) {
                            g_assert(uninitialized_plugin_streambuf_set_.size()
                                     == 1);
                            streambuf =
                                uninitialized_plugin_streambuf_set_.front();
                            this->browser_->world_url(url);
                        } else {
                            g_warning("Attempt to create an unrequested "
                                      "stream.");
                            continue;
                        }
                    }
                    streambuf->init(type);
                    bool succeeded = ::plugin_streambuf_map
                        .insert(make_pair(stream_id, streambuf)).second;
                    g_assert(succeeded);
                    succeeded =
                        ::uninitialized_plugin_streambuf_set_.erase(streambuf);
                    g_assert(succeeded);
                } else if (command == "destroy-stream") {
                    size_t stream_id;
                    command_line_stream >> stream_id;
                    plugin_streambuf_map_t::iterator pos = 
                        plugin_streambuf_map.find(stream_id);
                    if (pos == plugin_streambuf_map.end()) {
                        g_warning("Attempt to destroy a nonexistent stream.");
                        continue;
                    }
                    pos->second->buf_.put(std::char_traits<char>::eof());
                    pos->second->npstream_destroyed();
                    plugin_streambuf_map.erase(pos);
                } else if (command == "write") {
                    size_t stream_id, offset, length;
                    command_line_stream >> stream_id >> offset >> length;
                    plugin_streambuf_map_t::const_iterator pos =
                        plugin_streambuf_map.find(stream_id);
                    if (pos == plugin_streambuf_map.end()) {
                        g_warning("Attempt to write to a nonexistent stream.");
                        continue;
                    }
                    for (size_t i = 0; i < length; ++i) {
                        pos->second->buf_.put(this->in_->get());
                    }
                }
            }
        }

    private:
        command_istream * in_;
        openvrml::browser * browser_;
    };


    struct initial_stream_reader {
        initial_stream_reader(
            const boost::shared_ptr<plugin_streambuf> & streambuf,
            openvrml::browser & browser):
            streambuf_(streambuf),
            browser_(&browser)
        {}

        void operator()() const
        {
            class plugin_istream : public std::istream {
                boost::shared_ptr<plugin_streambuf> streambuf_;

            public:
                explicit plugin_istream(
                    const boost::shared_ptr<plugin_streambuf> & streambuf):
                    std::istream(streambuf.get()),
                    streambuf_(streambuf)
                {}

                virtual ~plugin_istream() throw ()
                {}
            } in(this->streambuf_);
            std::vector<openvrml::node_ptr> nodes =
                this->browser_->create_vrml_from_stream(in);
            this->browser_->replace_world(nodes);
        }

    private:
        boost::shared_ptr<plugin_streambuf> streambuf_;
        openvrml::browser * browser_;
    };

    GIOChannel * command_channel;
    GIOChannel * request_channel;
    int request_write_fd;
}

int main(int argc, char * argv[])
{
    using std::string;
    using std::vector;
    using boost::function0;
    using boost::scoped_ptr;
    using boost::shared_ptr;
    using boost::thread;
    using boost::thread_group;

    g_set_application_name(application_name);

    gtk_init(&argc, &argv);
    gtk_gl_init(&argc, &argv);

    arguments arguments;
    arguments.socket_id = 0;
    arguments.read_fd = 0;
    arguments.write_fd = 0;

    char args_doc[] = "[URI]";
    char * const doc = 0;
    argp_child * const children = 0;
    char *(*help_filter)(int key, const char * text, void * input) = 0;
    char * argp_domain = 0;
    argp argp = {
        options,
        parse_opt,
        args_doc,
        doc,
        children,
        help_filter,
        argp_domain
    };
    int uri_arg_index;
    argp_parse(&argp, argc, argv, 0, &uri_arg_index, &arguments);

    command_istream command_in;

    if (arguments.read_fd) {
        ::command_channel = g_io_channel_unix_new(arguments.read_fd);
        GError * error = 0;
        GIOStatus status = g_io_channel_set_encoding(::command_channel,
                                                     0, // binary (no encoding)
                                                     &error);
        if (status != G_IO_STATUS_NORMAL) {
            if (error) {
                g_critical(error->message);
                g_error_free(error);
            }
            return EXIT_FAILURE;
        }
                                                     
        g_io_add_watch(::command_channel,
                       G_IO_IN,
                       command_data_available,
                       static_cast<command_streambuf *>(command_in.rdbuf()));
    }

    if (arguments.write_fd) {
        ::request_write_fd = arguments.write_fd;
        ::request_channel = g_io_channel_unix_new(arguments.write_fd);
    }

    GtkWidget * window = arguments.socket_id
        ? gtk_plug_new(arguments.socket_id)
        : gtk_window_new(GTK_WINDOW_TOPLEVEL);

    GtkGLViewer viewer(*(GTK_CONTAINER(window)));
    browser b;
    b.viewer(&viewer);

    gtk_widget_show_all(window);

    thread_group threads;

    scoped_ptr<thread> initial_stream_reader_thread;
    if (uri_arg_index < argc) {
        const vector<string> uri(1, argv[uri_arg_index]), parameter;
        b.load_url(uri, parameter);
    } else {
        shared_ptr<plugin_streambuf> initial_stream(new plugin_streambuf);
        bool succeeded =
            uninitialized_plugin_streambuf_set_.insert(initial_stream);
        g_return_val_if_fail(succeeded, EXIT_FAILURE);
        function0<void> f = initial_stream_reader(initial_stream, b);
        initial_stream_reader_thread.reset(threads.create_thread(f));
    }

    viewer.timer_update();

    function0<void> read_commands = command_istream_reader(command_in, b);
    scoped_ptr<thread> command_reader_thread(
        threads.create_thread(read_commands));

    gtk_main();

    g_io_channel_shutdown(::command_channel, false, 0);
    g_io_channel_unref(::command_channel);
}

error_t parse_opt(int key, char * arg, argp_state * state)
{
    using boost::lexical_cast;
    using boost::bad_lexical_cast;

    arguments & args = *static_cast<arguments *>(state->input);

    switch (key) {
    case gtk_socket_id_id:
        try {
            args.socket_id = lexical_cast<GdkNativeWindow>(arg);
        } catch (bad_lexical_cast &) {
            argp_error(state, "GTK_SOCKET_ID must be an integer");
        }
        break;
    case read_fd_id:
        try {
            args.read_fd = lexical_cast<int>(arg);
        } catch (bad_lexical_cast &) {
            argp_error(state, "READ_FD must be an unsigned integer");
        }
        break;
    case write_fd_id:
        try {
            args.write_fd = lexical_cast<int>(arg);
        } catch (bad_lexical_cast &) {
            argp_error(state, "WRITE_FD must be an unsigned integer");
        }
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

namespace {

    GdkGLConfig * GtkGLViewer::gl_config(0);

    GtkGLViewer::GtkGLViewer(GtkContainer & container):
        drawing_area(gtk_drawing_area_new()),
        timer(0),
        redrawNeeded(false)
    {
        if (!this->drawing_area) { throw std::bad_alloc(); }

        if (!GtkGLViewer::gl_config) {
            static const int attrib_list[] = {
                // GDK_GL_ALPHA_SIZE, 1,
                GDK_GL_DOUBLEBUFFER,
                GDK_GL_DEPTH_SIZE, 1,
                GDK_GL_RGBA,
                GDK_GL_RED_SIZE, 1,
                GDK_GL_ATTRIB_LIST_NONE
            };
            GtkGLViewer::gl_config = gdk_gl_config_new(attrib_list);
        }

        static GdkGLContext * const share_list = 0;
        static const gboolean direct = false;
        static const int render_type = GDK_GL_RGBA_TYPE;
        gtk_widget_set_gl_capability(this->drawing_area,
                                     GtkGLViewer::gl_config,
                                     share_list,
                                     direct,
                                     render_type);

        gtk_widget_add_events(this->drawing_area,
                              GDK_EXPOSURE_MASK
                              | GDK_BUTTON_PRESS_MASK
                              | GDK_BUTTON_RELEASE_MASK
                              | GDK_KEY_PRESS_MASK
                              | GDK_POINTER_MOTION_MASK);

        g_signal_connect(G_OBJECT((this->drawing_area)),
                         "expose_event",
                         G_CALLBACK(expose_event),
                         this);
        g_signal_connect(G_OBJECT(this->drawing_area),
                         "configure_event",
                         G_CALLBACK(configure_event),
                         this);
        g_signal_connect(G_OBJECT(this->drawing_area),
                         "key_press_event",
                         G_CALLBACK(key_press_event),
                         this);
        g_signal_connect(G_OBJECT(this->drawing_area),
                         "button_press_event",
                         G_CALLBACK(button_press_event),
                         this);
        g_signal_connect(G_OBJECT(this->drawing_area),
                         "button_release_event",
                         G_CALLBACK(button_release_event),
                         this);
        g_signal_connect(G_OBJECT(this->drawing_area),
                         "motion_notify_event",
                         G_CALLBACK(motion_notify_event),
                         this);

        gtk_container_add(&container, this->drawing_area);

        
    }

    GtkGLViewer::~GtkGLViewer() throw ()
    {
        if (this->timer) { g_source_remove(timer); }
    }

    void GtkGLViewer::post_redraw()
    {
        if (!this->redrawNeeded) {
            this->redrawNeeded = true;
            gtk_widget_queue_draw(GTK_WIDGET(this->drawing_area));
        }
    }

    void GtkGLViewer::set_cursor(cursor_style style)
    {
        GdkCursor * cursor(0);
        GtkWidget * const widget = GTK_WIDGET(this->drawing_area);

        switch(style) {
        case cursor_inherit:
            XDefineCursor(GDK_WINDOW_XDISPLAY(widget->window),
                          GDK_WINDOW_XWINDOW(widget->window),
                          None);
            return;

        case cursor_info:
            cursor = gdk_cursor_new(GDK_HAND1);
            break;

        case cursor_cycle:
            cursor = gdk_cursor_new(GDK_EXCHANGE);
            break;

        case cursor_up_down:
            cursor = gdk_cursor_new(GDK_SB_V_DOUBLE_ARROW);
            break;

        case cursor_crosshair:
            cursor = gdk_cursor_new(GDK_CROSSHAIR);
            break;

        default:
            cursor = gdk_cursor_new(GDK_ARROW);
        }

        gdk_window_set_cursor(widget->window, cursor);
        gdk_cursor_destroy(cursor);
    }

    void GtkGLViewer::swap_buffers()
    {
        GtkWidget * const widget = GTK_WIDGET(this->drawing_area);
        GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
        gdk_gl_drawable_swap_buffers(gl_drawable);
    }

    gint timeout_callback(const gpointer ptr)
    {
        assert(ptr);
        GtkGLViewer & viewer = *static_cast<GtkGLViewer *>(ptr);
        viewer.timer_update();
        return false;
    }

    void GtkGLViewer::set_timer(const double t)
    {
        if (!this->timer) {
            this->timer = g_timeout_add(guint(10.0 * (t + 1)),
                                        GtkFunction(timeout_callback),
                                        this);
        }
    }

    void GtkGLViewer::timer_update()
    {
        this->timer = 0;
        this->viewer::update();
    }

    browser::browser():
        openvrml::browser(std::cout, std::cerr)
    {}

    std::auto_ptr<openvrml::resource_istream>
    browser::do_get_resource(const std::string & uri)
    {
        class plugin_resource_istream : public openvrml::resource_istream {
            boost::shared_ptr<plugin_streambuf> streambuf_;

        public:
            explicit plugin_resource_istream(const std::string & uri):
                openvrml::resource_istream(0),
                streambuf_(new plugin_streambuf(uri))
            {
                using std::ostringstream;

                this->rdbuf(this->streambuf_.get());
                bool succeeded =
                    ::uninitialized_plugin_streambuf_set_
                    .insert(this->streambuf_);
                g_assert(succeeded);

                ostringstream request;
                request << "get-url " << uri << '\n';
                gsize bytes_written;
                g_io_channel_write_chars(::request_channel,
                                         request.str().data(),
                                         request.str().length(),
                                         &bytes_written,
                                         0);
                g_io_channel_flush(::request_channel, 0);
            }

            virtual const std::string url() const throw ()
            {
                return this->streambuf_->url();
            }

            virtual const std::string type() const throw ()
            {
                return this->streambuf_->type();
            }

            virtual bool data_available() const throw ()
            {
                return this->streambuf_->data_available();
            }
        };
        return std::auto_ptr<openvrml::resource_istream>(
            new plugin_resource_istream(uri));
    }

    gboolean command_data_available(GIOChannel * source,
                                    GIOCondition condition,
                                    gpointer data)
    {
        command_streambuf & streambuf =
            *static_cast<command_streambuf *>(data);

        const int fd = g_io_channel_unix_get_fd(source);
        fd_set readfds;
        do {
            gchar c;
            gsize bytes_read;
            GError * error = 0;
            const GIOStatus status =
                g_io_channel_read_chars(source, &c, 1, &bytes_read, &error);
            if (status == G_IO_STATUS_ERROR) {
                if (error) {
                    g_warning(error->message);
                    g_error_free(error);
                }
                return false;
            }
            if (status == G_IO_STATUS_EOF) { return false; }
            if (status == G_IO_STATUS_AGAIN) { continue; }
            g_return_val_if_fail(status == G_IO_STATUS_NORMAL, false);

            g_assert(bytes_read == 1);

            streambuf.source_buffer_.put(c);

            FD_ZERO(&readfds);
            FD_SET(fd, &readfds);

            fd_set errorfds;
            FD_ZERO(&errorfds);
            FD_SET(fd, &errorfds);

            timeval timeout = {};
            int bits_set = select(fd + 1, &readfds, 0, &errorfds, &timeout);
            if (FD_ISSET(fd, &errorfds) || bits_set < 0) {
                g_warning(strerror(errno));
                g_return_val_if_reached(false);
            }
        } while (FD_ISSET(fd, &readfds));

        return true;
    }
} // namespace


gboolean realize(GtkWidget * widget, GdkEvent * event, gpointer data)
{
    return true;
}

gboolean expose_event(GtkWidget * const widget,
                      GdkEventExpose * const event,
                      const gpointer data)
{
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);
    g_assert(data);
    GtkGLViewer & viewer = *static_cast<GtkGLViewer *>(data);
    if (event->count == 0
        && gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        viewer.redraw();
    }
    viewer.redrawNeeded = false;
    return true;
}

gboolean configure_event(GtkWidget * const widget,
                         GdkEventConfigure * const event,
                         const gpointer data)
{
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);
    if (gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        g_assert(data);
        GtkGLViewer & viewer = *static_cast<GtkGLViewer *>(data);
        viewer.resize(widget->allocation.width, widget->allocation.height);
    }
    return true;
}

gboolean key_press_event(GtkWidget * const widget,
                         GdkEventKey * const event,
                         const gpointer data)
{
    using openvrml::gl::viewer;

    viewer::event_info info;
    info.event = viewer::event_key_down;

    switch (event->keyval) {
    case XK_Home:
        info.what = viewer::key_home;
        break;

    case XK_Left:
        info.what = viewer::key_left;
        break;

    case XK_Up:
        info.what = viewer::key_up;
        break;

    case XK_Right:
        info.what = viewer::key_right;
        break;

    case XK_Down:
        info.what = viewer::key_down;
        break;

    case XK_Page_Up:
        info.what = viewer::key_page_up;
        break;

    case XK_Page_Down:
        info.what = viewer::key_page_down;
        break;

    default:
        if (event->length <= 0) { return true; }
        info.what = event->string[0];
    }
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);
    if (gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        g_assert(data);
        GtkGLViewer & viewer = *static_cast<GtkGLViewer *>(data);
        viewer.input(&info);
    }
    return true;
}

gboolean button_press_event(GtkWidget * const widget,
                            GdkEventButton * const event,
                            const gpointer data)
{
    using openvrml::gl::viewer;

    viewer::event_info info;
    info.event = viewer::event_mouse_click;

    switch (event->button) {
    case Button1:
        info.what = 0;
        break;

    case Button2:
        info.what = 1;
        break;

    case Button3:
        info.what = 2;
        break;
    }

    info.x = int(event->x);
    info.y = int(event->y);
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);
    if (gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        g_assert(data);
        GtkGLViewer & viewer = *static_cast<GtkGLViewer *>(data);
        viewer.input(&info);
    }
    return true;
}

gboolean button_release_event(GtkWidget * const widget,
                              GdkEventButton * const event,
                              const gpointer data)
{
    using openvrml::gl::viewer;

    viewer::event_info info;
    info.event = viewer::event_mouse_release;

    switch (event->button) {
    case Button1:
        info.what = 0;
        break;

    case Button2:
        info.what = 1;
        break;

    case Button3:
        info.what = 2;
        break;
    }

    info.x = int(event->x);
    info.y = int(event->y);
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);
    if (gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        g_assert(data);
        GtkGLViewer & viewer = *static_cast<GtkGLViewer *>(data);
        viewer.input(&info);
    }
    return true;
}

gboolean motion_notify_event(GtkWidget * const widget,
                             GdkEventMotion * const event,
                             const gpointer data)
{
    using openvrml::gl::viewer;

    viewer::event_info info;
    info.event = viewer::event_mouse_drag;
    info.what = 0;
    if (event->state & Button1Mask) {
        info.what = 0;
    } else if (event->state & Button2Mask) {
        info.what = 1;
    } else if (event->state & Button3Mask) {
        info.what = 2;
    } else {
        info.event = viewer::event_mouse_move;
    }

    info.x = int(event->x);
    info.y = int(event->y);
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);
    if (gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        g_assert(data);
        GtkGLViewer & viewer = *static_cast<GtkGLViewer *>(data);
        viewer.input(&info);
    }
    return true;
}
