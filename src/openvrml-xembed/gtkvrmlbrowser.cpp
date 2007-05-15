// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML Mozilla plug-in
// Copyright 2004, 2005, 2006  Braden N. McDaniel
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

# include <iostream>
# include <sstream>
# include <boost/multi_index/detail/scope_guard.hpp>
// Must include before X11 headers.
# include <boost/numeric/conversion/converter.hpp>
# include <X11/keysym.h>
# include <gdk/gdkx.h>
# include <openvrml/browser.h>
# include <openvrml/gl/viewer.h>
# include "gtkvrmlbrowser.h"
# include "plugin_streambuf.h"

using namespace boost::multi_index::detail; // for scope_guard

extern "C" {
    void gtk_vrml_browser_class_init(GtkVrmlBrowserClass * klass);
    void gtk_vrml_browser_init(GtkVrmlBrowser * vrml_browser);

    G_GNUC_INTERNAL gboolean gtk_vrml_browser_destroy(GtkWidget * widget,
                                                      GdkEvent * event,
                                                      gpointer data);
    G_GNUC_INTERNAL gboolean gtk_vrml_browser_realize(GtkWidget * widget,
                                                      GdkEvent * event,
                                                      gpointer data);
    G_GNUC_INTERNAL gboolean
    gtk_vrml_browser_expose_event(GtkWidget * widget,
                                  GdkEventExpose * event,
                                  gpointer data);
    G_GNUC_INTERNAL gboolean
    gtk_vrml_browser_configure_event(GtkWidget * widget,
                                     GdkEventConfigure * event,
                                     gpointer data);
    G_GNUC_INTERNAL gboolean
    gtk_vrml_browser_key_press_event(GtkWidget * widget,
                                     GdkEventKey * event,
                                     gpointer data);
    G_GNUC_INTERNAL gboolean
    gtk_vrml_browser_button_press_event(GtkWidget * widget,
                                        GdkEventButton * event,
                                        gpointer data);
    G_GNUC_INTERNAL gboolean
    gtk_vrml_browser_button_release_event(GtkWidget * widget,
                                          GdkEventButton * event,
                                          gpointer data);
    G_GNUC_INTERNAL gboolean
    gtk_vrml_browser_motion_notify_event(GtkWidget * widget,
                                         GdkEventMotion * event,
                                         gpointer data);

    G_GNUC_INTERNAL gint gtk_vrml_browser_timeout_callback(gpointer ptr);
}

GType gtk_vrml_browser_get_type()
{
    static GType type = 0;

    if (!type) {
        static const GTypeInfo info = {
            sizeof (GtkVrmlBrowserClass),
            0, // base_init
            0, // base_finalize
            reinterpret_cast<GClassInitFunc>(gtk_vrml_browser_class_init),
            0, // class_finalize
            0, // class_data
            sizeof (GtkVrmlBrowser),
            0, // n_preallocs
            reinterpret_cast<GInstanceInitFunc>(gtk_vrml_browser_init),
            0  // value_table
        };

        type = g_type_register_static(GTK_TYPE_DRAWING_AREA,
                                      "GtkVrmlBrowser",
                                      &info,
                                      GTypeFlags(0));
    }
    return type;
}

namespace {
    G_GNUC_INTERNAL GdkGLConfig * gl_config;

    class G_GNUC_INTERNAL resource_fetcher : public openvrml::resource_fetcher {
        GIOChannel * request_channel_;
        boost::mutex request_channel_mutex_;
        boost::thread_group thread_group_;

    public:
        explicit resource_fetcher(GIOChannel & request_channel);
        virtual ~resource_fetcher() OPENVRML_NOTHROW;

        void create_thread(const boost::function0<void> & threadfunc);

    private:
        virtual std::auto_ptr<openvrml::resource_istream>
        do_get_resource(const std::string & uri);

        bool write_request_chars(const gchar * buf, gssize count,
                                 gsize * bytes_written);
    };


    class GtkGLViewer;

    class G_GNUC_INTERNAL browser_listener :
        public openvrml::browser_listener {

        GtkGLViewer & viewer_;

    public:
        explicit browser_listener(GtkGLViewer & viewer);

    private:
        virtual void do_browser_changed(const openvrml::browser_event & event);
    };

    class G_GNUC_INTERNAL GtkGLViewer : public openvrml::gl::viewer {
        friend class browser_listener;
        friend void
        (::gtk_vrml_browser_load_url)(GtkVrmlBrowser * vrml_browser,
                                      const gchar ** url,
                                      const gchar ** parameter);
        friend void
        (::gtk_vrml_browser_set_world)(GtkVrmlBrowser * vrml_browser,
                                       openvrml::resource_istream & in);
        friend gint (::gtk_vrml_browser_timeout_callback)(gpointer ptr);
        friend gboolean (::gtk_vrml_browser_expose_event)(GtkWidget *,
                                                          GdkEventExpose *,
                                                          gpointer);
        friend gboolean
            (::gtk_vrml_browser_motion_notify_event)(GtkWidget *,
                                                     GdkEventMotion *,
                                                     gpointer);

        struct load_url;

        ::resource_fetcher fetcher_;
        openvrml::browser browser_;
        ::browser_listener browser_listener_;
        bool browser_initialized_;
        openvrml::read_write_mutex browser_initialized_mutex_;
        GtkVrmlBrowser & vrml_browser_;
        guint timer;

    public:
        bool redrawNeeded;

        GtkGLViewer(GIOChannel & request_channel,
                    GtkVrmlBrowser & vrml_browser);
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
}

GtkWidget * gtk_vrml_browser_new(GIOChannel * const request_channel)
{
    GtkVrmlBrowser * const vrml_browser =
        GTK_VRML_BROWSER(g_object_new(GTK_TYPE_VRML_BROWSER, 0));
    try {
        vrml_browser->viewer = new GtkGLViewer(*request_channel,
                                               *vrml_browser);
    } catch (std::bad_alloc &) {
        g_return_val_if_reached(0);
    }
    static_cast<GtkGLViewer *>(vrml_browser->viewer)
        ->resize(GTK_WIDGET(vrml_browser)->allocation.width,
                 GTK_WIDGET(vrml_browser)->allocation.height);

    return GTK_WIDGET(vrml_browser);
}

namespace {
    struct GtkGLViewer::load_url {
        load_url(GtkGLViewer & viewer,
                 const std::vector<std::string> & url,
                 const std::vector<std::string> & parameter):
            viewer_(&viewer),
            url_(url),
            parameter_(parameter)
        {}

        void operator()() const throw ()
        {
            {
                openvrml::read_write_mutex::scoped_write_lock
                    lock(this->viewer_->browser_initialized_mutex_);
                this->viewer_->browser_initialized_ = false;
            }
            this->viewer_->browser_.load_url(this->url_, this->parameter_);
        }

    private:
        GtkGLViewer * const viewer_;
        const std::vector<std::string> url_, parameter_;
    };
}

void gtk_vrml_browser_load_url(GtkVrmlBrowser * const vrml_browser,
                               const gchar ** url,
                               const gchar ** parameter)
{
    using std::string;
    using std::vector;

    GtkGLViewer & viewer = *static_cast<GtkGLViewer *>(vrml_browser->viewer);
    vector<string> url_vec, param_vec;
    while (url && *url) { url_vec.push_back(*(url++)); }
    while (parameter && *parameter) { param_vec.push_back(*(parameter++)); }

    viewer.fetcher_
        .create_thread(GtkGLViewer::load_url(viewer, url_vec, param_vec));
}

void gtk_vrml_browser_set_world(GtkVrmlBrowser * vrml_browser,
                                openvrml::resource_istream & in)
{
    GtkGLViewer & viewer = *static_cast<GtkGLViewer *>(vrml_browser->viewer);
    {
        openvrml::read_write_mutex::scoped_write_lock
            lock(viewer.browser_initialized_mutex_);
        viewer.browser_initialized_ = false;
    }
    viewer.browser_.set_world(in);
}

void gtk_vrml_browser_class_init(GtkVrmlBrowserClass *)
{}

void gtk_vrml_browser_init(GtkVrmlBrowser * const vrml_browser)
{
    vrml_browser->viewer = 0;

    if (!::gl_config) {
        static const int attrib_list[] = {
            // GDK_GL_ALPHA_SIZE, 1,
            GDK_GL_DOUBLEBUFFER,
            GDK_GL_DEPTH_SIZE, 1,
            GDK_GL_RGBA,
            GDK_GL_RED_SIZE, 1,
            GDK_GL_ATTRIB_LIST_NONE
        };
        ::gl_config = gdk_gl_config_new(attrib_list);
    }

    static GdkGLContext * const share_list = 0;
    static const gboolean direct = false;
    static const int render_type = GDK_GL_RGBA_TYPE;
    gtk_widget_set_gl_capability(GTK_WIDGET(vrml_browser),
                                 ::gl_config,
                                 share_list,
                                 direct,
                                 render_type);

    gtk_widget_add_events(GTK_WIDGET(vrml_browser),
                          GDK_EXPOSURE_MASK
                          | GDK_POINTER_MOTION_MASK
                          | GDK_BUTTON_PRESS_MASK
                          | GDK_BUTTON_RELEASE_MASK
                          | GDK_KEY_PRESS_MASK
                          | GDK_FOCUS_CHANGE_MASK);

    g_object_set(G_OBJECT(vrml_browser),
                 "can-focus", true,
                 NULL);

    g_object_connect(
        G_OBJECT(vrml_browser),

        "signal::destroy_event",
        G_CALLBACK(gtk_vrml_browser_destroy),
        0,

        "signal::expose_event",
        G_CALLBACK(gtk_vrml_browser_expose_event),
        0,

        "signal::configure_event",
        G_CALLBACK(gtk_vrml_browser_configure_event),
        0,

        "signal::key_press_event",
        G_CALLBACK(gtk_vrml_browser_key_press_event),
        0,

        "signal::button_press_event",
        G_CALLBACK(gtk_vrml_browser_button_press_event),
        0,

        "signal::button_release_event",
        G_CALLBACK(gtk_vrml_browser_button_release_event),
        0,

        "signal::motion_notify_event",
        G_CALLBACK(gtk_vrml_browser_motion_notify_event),
        0,
        NULL);
}

gboolean gtk_vrml_browser_destroy(GtkWidget * const widget,
                                  GdkEvent *,
                                  gpointer)
{
    delete static_cast<GtkGLViewer *>(GTK_VRML_BROWSER(widget)->viewer);
    return true;
}

gboolean gtk_vrml_browser_realize(GtkWidget *, GdkEvent *, gpointer)
{
    return true;
}

gboolean gtk_vrml_browser_expose_event(GtkWidget * const widget,
                                       GdkEventExpose * const event,
                                       gpointer)
{
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);
    GtkGLViewer & viewer =
        *static_cast<GtkGLViewer *>(GTK_VRML_BROWSER(widget)->viewer);
    {
        openvrml::read_write_mutex::scoped_read_lock
            lock(viewer.browser_initialized_mutex_);
        if (!viewer.browser_initialized_) { return true; }
    }

    if (event->count == 0
        && gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        viewer.redraw();
    }
    viewer.redrawNeeded = false;
    if (viewer.timer == 0) { viewer.timer_update(); }
    return true;
}

gboolean gtk_vrml_browser_configure_event(GtkWidget * const widget,
                                          GdkEventConfigure *,
                                          gpointer)
{
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);
    if (GTK_VRML_BROWSER(widget)->viewer
        && gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        GtkGLViewer & viewer =
            *static_cast<GtkGLViewer *>(GTK_VRML_BROWSER(widget)->viewer);
        viewer.resize(widget->allocation.width, widget->allocation.height);
    }
    return true;
}

gboolean gtk_vrml_browser_key_press_event(GtkWidget * const widget,
                                          GdkEventKey * const event,
                                          gpointer)
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
        GtkGLViewer * const viewer =
            static_cast<GtkGLViewer *>(GTK_VRML_BROWSER(widget)->viewer);
        g_assert(viewer);
        viewer->input(&info);
    }
    return true;
}

gboolean gtk_vrml_browser_button_press_event(GtkWidget * const widget,
                                             GdkEventButton * const event,
                                             gpointer)
{
    using openvrml::gl::viewer;

    gtk_widget_grab_focus(widget);

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
        GtkGLViewer * const viewer =
            static_cast<GtkGLViewer *>(GTK_VRML_BROWSER(widget)->viewer);
        g_assert(viewer);
        viewer->input(&info);
    }
    return true;
}

gboolean gtk_vrml_browser_button_release_event(GtkWidget * const widget,
                                               GdkEventButton * const event,
                                               gpointer)
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
        GtkGLViewer * const viewer =
            static_cast<GtkGLViewer *>(GTK_VRML_BROWSER(widget)->viewer);
        g_assert(viewer);
        viewer->input(&info);
    }
    return true;
}

gboolean gtk_vrml_browser_motion_notify_event(GtkWidget * const widget,
                                              GdkEventMotion * const event,
                                              gpointer)
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
        g_assert(GTK_VRML_BROWSER(widget)->viewer);
        GtkGLViewer & viewer =
            *static_cast<GtkGLViewer *>(GTK_VRML_BROWSER(widget)->viewer);
        openvrml::read_write_mutex::scoped_read_lock
            lock(viewer.browser_initialized_mutex_);
        if (!viewer.browser_initialized_) { return true; }
        viewer.input(&info);
    }
    return true;
}

gint gtk_vrml_browser_timeout_callback(const gpointer ptr)
{
    g_assert(ptr);

    GtkGLViewer & viewer = *static_cast<GtkGLViewer *>(ptr);
    viewer.timer_update();
    return false;
}

namespace {

    resource_fetcher::resource_fetcher(GIOChannel & request_channel):
        request_channel_(&request_channel)
    {}

    resource_fetcher::~resource_fetcher() OPENVRML_NOTHROW
    {
        this->thread_group_.join_all();
    }

    void
    resource_fetcher::create_thread(const boost::function0<void> & threadfunc)
    {
        this->thread_group_.create_thread(threadfunc);
    }

    std::auto_ptr<openvrml::resource_istream>
    resource_fetcher::do_get_resource(const std::string & uri)
    {
        using openvrml_xembed::plugin_streambuf;

        class plugin_resource_istream : public openvrml::resource_istream {
            const boost::shared_ptr<plugin_streambuf> streambuf_;
            resource_fetcher & resource_fetcher_;

        public:
            plugin_resource_istream(const std::string & uri,
                                    resource_fetcher & fetcher):
                openvrml::resource_istream(0),
                streambuf_(new plugin_streambuf(uri)),
                resource_fetcher_(fetcher)
            {
                using std::ostringstream;
                using boost::ref;
                using openvrml_xembed::uninitialized_plugin_streambuf_map_;

                this->rdbuf(this->streambuf_.get());
                uninitialized_plugin_streambuf_map_.insert(uri,
                                                           this->streambuf_);

                ostringstream request;
                request << "get-url " << uri << '\n';
                gsize bytes_written;
                const bool write_succeeded =
                    this->resource_fetcher_
                    .write_request_chars(request.str().data(),
                                         request.str().length(),
                                         &bytes_written);
                if (!write_succeeded) {
                    this->setstate(ios_base::badbit);
                    return;
                }

                //
                // This blocks until we receive a get-url-result command.
                //
                const int get_url_result = this->streambuf_->get_url_result();
                if (get_url_result != 0) {
                    this->setstate(ios_base::badbit);
                }
            }

        private:
            virtual const std::string do_url() const throw ()
            {
                return this->streambuf_->url();
            }

            virtual const std::string do_type() const throw ()
            {
                return this->streambuf_->type();
            }

            virtual bool do_data_available() const throw ()
            {
                return this->streambuf_->data_available();
            }
        };
        return std::auto_ptr<openvrml::resource_istream>(
            new plugin_resource_istream(uri, *this));
    }

    bool resource_fetcher::write_request_chars(const gchar * const buf,
                                               const gssize count,
                                               gsize * const bytes_written)
    {
        boost::mutex::scoped_lock lock(this->request_channel_mutex_);

        using boost::ref;

        GError * error = 0;
        scope_guard error_guard = make_guard(g_error_free, ref(error));
        boost::ignore_unused_variable_warning(error_guard);

        GIOStatus status =
            g_io_channel_write_chars(this->request_channel_, buf, count,
                                     bytes_written, &error);
        if (status != G_IO_STATUS_NORMAL) {
            g_warning(error->message);
            return false;
        }

        status = g_io_channel_flush(this->request_channel_, &error);
        if (status != G_IO_STATUS_NORMAL) {
            g_warning(error->message);
            return false;
        }

        error_guard.dismiss();
        return true;
    }

    browser_listener::browser_listener(GtkGLViewer & viewer):
        viewer_(viewer)
    {}

    void
    browser_listener::do_browser_changed(const openvrml::browser_event & event)
    {
        if (event.id() == openvrml::browser_event::initialized) {
            {
                openvrml::read_write_mutex::scoped_write_lock
                    lock(this->viewer_.browser_initialized_mutex_);
                this->viewer_.browser_initialized_ = true;
            }
            gdk_threads_enter();
            scope_guard gdk_threads_guard = make_guard(gdk_threads_leave);
            boost::ignore_unused_variable_warning(gdk_threads_guard);
            //
            // Set redrawNeeded to false to ensure that this particular call to
            // post_redraw results in a redraw.
            //
            this->viewer_.redrawNeeded = false;
            this->viewer_.post_redraw();
        }
    }

    //
    // We use stdout for communication with the host process; so send
    // all browser output to stderr.
    //
    GtkGLViewer::GtkGLViewer(GIOChannel & request_channel,
                             GtkVrmlBrowser & vrml_browser):
        fetcher_(request_channel),
        browser_(this->fetcher_, std::cerr, std::cerr),
        browser_listener_(*this),
        browser_initialized_(true),
        vrml_browser_(vrml_browser),
        timer(0),
        redrawNeeded(false)
    {
        this->browser_.add_listener(this->browser_listener_);
        this->browser_.viewer(this);
    }

    GtkGLViewer::~GtkGLViewer() throw ()
    {
        if (this->timer) { g_source_remove(timer); }
        this->browser_.remove_listener(this->browser_listener_);
    }

    void GtkGLViewer::post_redraw()
    {
        if (!this->redrawNeeded) {
            this->redrawNeeded = true;
            gtk_widget_queue_draw(GTK_WIDGET(&this->vrml_browser_));
        }
    }

    void GtkGLViewer::set_cursor(cursor_style style)
    {
        GdkCursor * cursor(0);

        switch(style) {
        case cursor_inherit:
            XDefineCursor(GDK_WINDOW_XDISPLAY(
                              GTK_WIDGET(&this->vrml_browser_)->window),
                          GDK_WINDOW_XWINDOW(
                              GTK_WIDGET(&this->vrml_browser_)->window),
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

        gdk_window_set_cursor(GTK_WIDGET(&this->vrml_browser_)->window,
                              cursor);
        gdk_cursor_destroy(cursor);
    }

    void GtkGLViewer::swap_buffers()
    {
        GdkGLDrawable * const gl_drawable =
            gtk_widget_get_gl_drawable(GTK_WIDGET(&this->vrml_browser_));
        gdk_gl_drawable_swap_buffers(gl_drawable);
    }

    void GtkGLViewer::set_timer(const double t)
    {
        if (!this->timer) {
            this->timer =
                g_timeout_add(guint(10.0 * (t + 1)),
                              GtkFunction(gtk_vrml_browser_timeout_callback),
                              this);
        }
    }

    void GtkGLViewer::timer_update()
    {
        this->timer = 0;
        this->viewer::update();
    }
}
