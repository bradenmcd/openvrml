// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
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
// Must include before X11 headers.
# include <boost/numeric/conversion/converter.hpp>
# include <X11/keysym.h>
# include <gdk/gdkx.h>
# include <openvrml/browser.h>
# include <openvrml/gl/viewer.h>
# include "gtkvrmlbrowser.h"
# include "plugin_streambuf.h"

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

    class G_GNUC_INTERNAL browser : public openvrml::browser {
        GIOChannel * request_channel_;

    public:
        explicit browser(GIOChannel & request_channel);

    private:
        virtual std::auto_ptr<openvrml::resource_istream>
        do_get_resource(const std::string & uri);
    };

    class G_GNUC_INTERNAL GtkGLViewer : public openvrml::gl::viewer {
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

        ::browser browser_;
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
    viewer.browser_.load_url(url_vec, param_vec);
}

void gtk_vrml_browser_set_world(GtkVrmlBrowser * vrml_browser,
                                openvrml::resource_istream & in)
{
    GtkGLViewer & viewer = *static_cast<GtkGLViewer *>(vrml_browser->viewer);
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
                          | GDK_BUTTON_PRESS_MASK
                          | GDK_BUTTON_RELEASE_MASK
                          | GDK_KEY_PRESS_MASK
                          | GDK_POINTER_MOTION_MASK);

    g_signal_connect(G_OBJECT(vrml_browser),
                     "destroy_event",
                     G_CALLBACK(gtk_vrml_browser_destroy),
                     0);
    g_signal_connect(G_OBJECT(vrml_browser),
                     "expose_event",
                     G_CALLBACK(gtk_vrml_browser_expose_event),
                     0);
    g_signal_connect(G_OBJECT(vrml_browser),
                     "configure_event",
                     G_CALLBACK(gtk_vrml_browser_configure_event),
                     0);
    g_signal_connect(G_OBJECT(vrml_browser),
                     "key_press_event",
                     G_CALLBACK(gtk_vrml_browser_key_press_event),
                     0);
    g_signal_connect(G_OBJECT(vrml_browser),
                     "button_press_event",
                     G_CALLBACK(gtk_vrml_browser_button_press_event),
                     0);
    g_signal_connect(G_OBJECT(vrml_browser),
                     "button_release_event",
                     G_CALLBACK(gtk_vrml_browser_button_release_event),
                     0);
    g_signal_connect(G_OBJECT(vrml_browser),
                     "motion_notify_event",
                     G_CALLBACK(gtk_vrml_browser_motion_notify_event),
                     0);
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
        GtkGLViewer * const viewer =
            static_cast<GtkGLViewer *>(GTK_VRML_BROWSER(widget)->viewer);
        g_assert(viewer);
        viewer->input(&info);
    }
    return true;
}

gint gtk_vrml_browser_timeout_callback(const gpointer ptr)
{
    assert(ptr);

    GtkGLViewer & viewer = *static_cast<GtkGLViewer *>(ptr);
    viewer.timer_update();
    return false;
}

namespace {

    browser::browser(GIOChannel & request_channel):
        openvrml::browser(std::cout, std::cerr),
        request_channel_(&request_channel)
    {}

    std::auto_ptr<openvrml::resource_istream>
    browser::do_get_resource(const std::string & uri)
    {
        using openvrml_player::plugin_streambuf;

        class plugin_resource_istream : public openvrml::resource_istream {
            boost::shared_ptr<plugin_streambuf> streambuf_;
            GIOChannel * request_channel_;

        public:
            plugin_resource_istream(const std::string & uri,
                                    GIOChannel * const request_channel):
                openvrml::resource_istream(0),
                streambuf_(new plugin_streambuf(uri)),
                request_channel_(request_channel)
            {
                using std::ostringstream;
                using openvrml_player::uninitialized_plugin_streambuf_map_;

                this->rdbuf(this->streambuf_.get());
                uninitialized_plugin_streambuf_map_.insert(uri,
                                                           this->streambuf_);

                ostringstream request;
                request << "get-url " << uri << '\n';
                gsize bytes_written;
                g_io_channel_write_chars(this->request_channel_,
                                         request.str().data(),
                                         request.str().length(),
                                         &bytes_written,
                                         0);
                g_io_channel_flush(this->request_channel_, 0);

                //
                // This blocks until we know the result of NPN_GetURL.
                //
                const int get_url_result = this->streambuf_->get_url_result();
                if (get_url_result != 0) {
                    this->setstate(std::ios_base::failbit);
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
            new plugin_resource_istream(uri, this->request_channel_));
    }

    GtkGLViewer::GtkGLViewer(GIOChannel & request_channel,
                             GtkVrmlBrowser & vrml_browser):
        browser_(request_channel),
        vrml_browser_(vrml_browser),
        timer(0),
        redrawNeeded(false)
    {
        this->browser_.viewer(this);
    }

    GtkGLViewer::~GtkGLViewer() throw ()
    {
        if (this->timer) { g_source_remove(timer); }
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
