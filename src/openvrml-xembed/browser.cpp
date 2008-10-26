// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML XEmbed Control
//
// Copyright 2004, 2005, 2006, 2007, 2008  Braden N. McDaniel
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along
// with this library; if not, see <http://www.gnu.org/licenses/>.
//

# include <iostream>
# include <sstream>
# include <boost/multi_index/detail/scope_guard.hpp>
// Must include before X11 headers.
# include <boost/numeric/conversion/converter.hpp>
# include <boost/ptr_container/ptr_map.hpp>
# include <X11/keysym.h>
# include <dbus/dbus-glib.h>
# include <gdk/gdkx.h>
# include <openvrml/browser.h>
# include <openvrml/gl/viewer.h>
# include "browser.h"
# include "browser-server-glue.h"
# include "plugin_streambuf.h"
# include <gtk/gtkgl.h>
# include <gtk/gtkdrawingarea.h>

using namespace boost::multi_index::detail; // for scope_guard

GQuark openvrml_xembed_error_quark()
{
    return g_quark_from_static_string("openvrml-xembed-error-quark");
}

extern "C" {
    //
    // GObject overrides
    //
    G_GNUC_INTERNAL
    void openvrml_xembed_browser_set_property(GObject * obj, guint property_id,
                                              const GValue * value,
                                              GParamSpec * pspec);
    G_GNUC_INTERNAL
    void openvrml_xembed_browser_get_property(GObject * obj, guint property_id,
                                              GValue * value,
                                              GParamSpec * pspec);

    //
    // GtkWidget overrides
    //
    G_GNUC_INTERNAL void openvrml_xembed_browser_realize(GtkWidget * widget);
    G_GNUC_INTERNAL void openvrml_xembed_browser_unrealize(GtkWidget * widget);

    //
    // OpenvrmlXembedStreamClient implementation
    //
    G_GNUC_INTERNAL
    void
    openvrml_xembed_browser_stream_client_interface_init(gpointer g_iface,
                                                         gpointer iface_data);

    //
    // Signal handlers
    //
    G_GNUC_INTERNAL
    gboolean openvrml_xembed_browser_expose_event(GtkWidget * widget,
                                           GdkEventExpose * event,
                                           gpointer data);
    G_GNUC_INTERNAL
    gboolean openvrml_xembed_browser_configure_event(GtkWidget * widget,
                                              GdkEventConfigure * event,
                                              gpointer data);
    G_GNUC_INTERNAL
    gboolean openvrml_xembed_browser_key_press_event(GtkWidget * widget,
                                              GdkEventKey * event,
                                              gpointer data);
    G_GNUC_INTERNAL
    gboolean openvrml_xembed_browser_button_press_event(GtkWidget * widget,
                                                 GdkEventButton * event,
                                                 gpointer data);
    G_GNUC_INTERNAL
    gboolean openvrml_xembed_browser_button_release_event(GtkWidget * widget,
                                                   GdkEventButton * event,
                                                   gpointer data);
    G_GNUC_INTERNAL
    gboolean openvrml_xembed_browser_motion_notify_event(GtkWidget * widget,
                                                  GdkEventMotion * event,
                                                  gpointer data);

    G_GNUC_INTERNAL gint openvrml_xembed_browser_timeout_callback(gpointer ptr);
}

G_DEFINE_TYPE_WITH_CODE(OpenvrmlXembedBrowser,
                        openvrml_xembed_browser,
                        GTK_TYPE_PLUG,
                        G_IMPLEMENT_INTERFACE(
                            OPENVRML_XEMBED_TYPE_STREAM_CLIENT,
                            openvrml_xembed_browser_stream_client_interface_init))

namespace {
    G_GNUC_INTERNAL GdkGLConfig * gl_config;

    class G_GNUC_INTERNAL resource_fetcher : public openvrml::resource_fetcher {
        DBusGProxy & control_host_;
        openvrml_xembed::uninitialized_plugin_streambuf_map &
            uninitialized_plugin_streambuf_map_;
        openvrml_xembed::plugin_streambuf_map & plugin_streambuf_map_;
        boost::thread_group thread_group_;

    public:
        resource_fetcher(
            DBusGProxy & control_host,
            openvrml_xembed::uninitialized_plugin_streambuf_map &
                uninitialized_plugin_streambuf_map,
            openvrml_xembed::plugin_streambuf_map & plugin_streambuf_map);
        virtual ~resource_fetcher() OPENVRML_NOTHROW;

        void create_thread(const boost::function0<void> & threadfunc);

    private:
        virtual std::auto_ptr<openvrml::resource_istream>
        do_get_resource(const std::string & uri);
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

        friend gboolean
        (::openvrml_xembed_browser_load_url)(OpenvrmlXembedBrowser * browser,
                                             const gchar ** url,
                                             const gchar ** parameter,
                                             GError **);

        friend gchar *
        (::openvrml_xembed_browser_get_world_url)(
            OpenvrmlXembedBrowser * vrml_browser,
            GError ** error);

        friend gint (::openvrml_xembed_browser_timeout_callback)(gpointer ptr);
        friend gboolean (::openvrml_xembed_browser_expose_event)(
            GtkWidget *,
            GdkEventExpose *,
            gpointer);
        friend gboolean
            (::openvrml_xembed_browser_motion_notify_event)(GtkWidget *,
                                                            GdkEventMotion *,
                                                            gpointer);

        struct load_url;

        ::browser_listener browser_listener_;
        bool browser_initialized_;
        openvrml::read_write_mutex browser_initialized_mutex_;
        OpenvrmlXembedBrowser & vrml_browser_;
        guint timer;

    public:
        bool redrawNeeded;

        explicit GtkGLViewer(OpenvrmlXembedBrowser & vrml_browser);
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

struct OpenvrmlXembedBrowserPrivate_ {
    DBusGProxy * control_host;
    ::resource_fetcher * resource_fetcher;
    openvrml::browser * browser;
    GtkDrawingArea * drawing_area;
    GtkGLViewer * viewer;
    openvrml_xembed::uninitialized_plugin_streambuf_map * uninitialized_streambuf_map;
    openvrml_xembed::plugin_streambuf_map * streambuf_map;
    boost::thread * initial_stream_reader_thread;
    bool expect_initial_stream;
    bool got_initial_stream;
};

#   define OPENVRML_XEMBED_BROWSER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), OPENVRML_XEMBED_TYPE_BROWSER, OpenvrmlXembedBrowserPrivate))

void openvrml_xembed_browser_init(OpenvrmlXembedBrowser * const vrml_browser)
{
    vrml_browser->priv = OPENVRML_XEMBED_BROWSER_GET_PRIVATE(vrml_browser);
}

namespace {
    enum browser_property_id {
        control_host_proxy_id = 1,
        expect_initial_stream_id
    };

    enum browser_signal_id {
        initialized_id,
        shutdown_id,
        last_signal_id
    };

    G_GNUC_INTERNAL guint signals[last_signal_id];
}

void
openvrml_xembed_browser_class_init(OpenvrmlXembedBrowserClass * const klass)
{
    G_OBJECT_CLASS(klass)->set_property = openvrml_xembed_browser_set_property;
    G_OBJECT_CLASS(klass)->get_property = openvrml_xembed_browser_get_property;
    GTK_WIDGET_CLASS(klass)->realize    = openvrml_xembed_browser_realize;
    GTK_WIDGET_CLASS(klass)->unrealize  = openvrml_xembed_browser_unrealize;

    signals[initialized_id] =
        g_signal_new("initialized",
                     G_OBJECT_CLASS_TYPE(klass),
                     GSignalFlags(G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED),
                     0,
                     NULL, NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE, 0);

    signals[shutdown_id] =
        g_signal_new("shutdown",
                     G_OBJECT_CLASS_TYPE(klass),
                     GSignalFlags(G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED),
                     0,
                     NULL, NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE, 0);

    GParamSpec * pspec =
        g_param_spec_pointer(
            "control-host-proxy",
            "BrowserHost proxy",
            "DBusGProxy for a BrowserHost",
            GParamFlags(G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));
    g_object_class_install_property(G_OBJECT_CLASS(klass),
                                    control_host_proxy_id,
                                    pspec);

    pspec =
        g_param_spec_boolean(
            "expect-initial-stream",
            "expect an initial stream",
            "The VrmlControl will be delivered an initial stream",
            false,
            GParamFlags(G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE));
    g_object_class_install_property(G_OBJECT_CLASS(klass),
                                    expect_initial_stream_id,
                                    pspec);

    g_type_class_add_private(G_OBJECT_CLASS(klass),
                             sizeof (OpenvrmlXembedBrowserPrivate));

    dbus_g_object_type_install_info(OPENVRML_XEMBED_TYPE_BROWSER,
                                    &dbus_glib_openvrml_xembed_browser_object_info);
}

void openvrml_xembed_browser_set_property(GObject * const obj,
                                          const guint property_id,
                                          const GValue * const value,
                                          GParamSpec * const pspec)
{
    OpenvrmlXembedBrowser * const browser = OPENVRML_XEMBED_BROWSER(obj);
    switch (property_id) {
    case control_host_proxy_id:
        browser->priv->control_host =
            static_cast<DBusGProxy *>(g_value_get_pointer(value));
        break;
    case expect_initial_stream_id:
        browser->priv->expect_initial_stream = g_value_get_boolean(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
    }
}

void openvrml_xembed_browser_get_property(GObject * const obj,
                                          const guint property_id,
                                          GValue * const value,
                                          GParamSpec * const pspec)
{
    OpenvrmlXembedBrowser * const browser = OPENVRML_XEMBED_BROWSER(obj);
    switch (property_id) {
    case control_host_proxy_id:
        g_value_set_pointer(value, browser->priv->control_host);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
    }
}

namespace {
    //
    // We don't already know what the URI of the initial stream is until we
    // start getting that data from the browser.  This is a placeholder that
    // is used to identify the plugin_streambuf that will be used to receive
    // the initial stream data.
    //
    const char initial_stream_uri[] = "x-openvrml-initial:";

    struct OPENVRML_LOCAL initial_stream_reader {
        initial_stream_reader(
            const boost::shared_ptr<openvrml_xembed::plugin_streambuf> &
                streambuf,
            openvrml::browser & browser):
            streambuf_(streambuf),
            browser_(browser)
        {}

        void operator()() const throw ()
        {
            using openvrml_xembed::plugin_streambuf;

            class plugin_istream : public openvrml::resource_istream {
                boost::shared_ptr<plugin_streambuf> streambuf_;

            public:
                explicit plugin_istream(
                    const boost::shared_ptr<plugin_streambuf> & streambuf):
                    openvrml::resource_istream(streambuf.get()),
                    streambuf_(streambuf)
                {}

                virtual ~plugin_istream() throw ()
                {}

            private:
                virtual const std::string do_url() const throw (std::bad_alloc)
                {
                    return this->streambuf_->url();
                }

                virtual const std::string do_type() const
                    throw (std::bad_alloc)
                {
                    return this->streambuf_->type();
                }

                virtual bool do_data_available() const throw ()
                {
                    return this->streambuf_->data_available();
                }
            } in(this->streambuf_);

            this->browser_.set_world(in);
        }

    private:
        boost::shared_ptr<openvrml_xembed::plugin_streambuf> streambuf_;
        openvrml::browser & browser_;
    };
}

void openvrml_xembed_browser_realize(GtkWidget * const widget)
{
    GtkWidgetClass * klass =
        GTK_WIDGET_CLASS(g_type_class_peek(OPENVRML_XEMBED_TYPE_BROWSER));
    GtkWidgetClass * parent_class =
        GTK_WIDGET_CLASS(g_type_class_peek_parent(klass));
    parent_class->realize(widget);

    OpenvrmlXembedBrowser * const browser = OPENVRML_XEMBED_BROWSER(widget);

    try {
        browser->priv->uninitialized_streambuf_map =
            new openvrml_xembed::uninitialized_plugin_streambuf_map;
        browser->priv->streambuf_map =
            new openvrml_xembed::plugin_streambuf_map;
        browser->priv->resource_fetcher =
            new resource_fetcher(*browser->priv->control_host,
                                 *browser->priv->uninitialized_streambuf_map,
                                 *browser->priv->streambuf_map);
        browser->priv->browser =
            new openvrml::browser(*browser->priv->resource_fetcher,
                                  std::cout,
                                  std::cerr);
        browser->priv->drawing_area =
            GTK_DRAWING_AREA(g_object_new(GTK_TYPE_DRAWING_AREA, 0));
        browser->priv->viewer = new GtkGLViewer(*browser);

        //
        // If necessary, create the initial stream.
        //
        if (browser->priv->expect_initial_stream) {
            using boost::function0;
            using boost::shared_ptr;
            using openvrml_xembed::plugin_streambuf;

            const shared_ptr<plugin_streambuf> initial_stream(
                new plugin_streambuf(
                    ::initial_stream_uri,
                    *browser->priv->uninitialized_streambuf_map,
                    *browser->priv->streambuf_map));
            initial_stream->state_ = plugin_streambuf::uninitialized;
            browser->priv->uninitialized_streambuf_map
                ->insert(::initial_stream_uri, initial_stream);

            const function0<void> initial_stream_reader_func =
                initial_stream_reader(initial_stream, *browser->priv->browser);

            browser->priv->initial_stream_reader_thread =
                new boost::thread(initial_stream_reader_func);
        }
    } catch (std::exception & ex) {
        //
        // ex is most likely std::bad_alloc or boost::thread_resource_error.
        //
        g_critical("%s", ex.what());
        return;
    }

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
    gtk_widget_set_gl_capability(GTK_WIDGET(browser->priv->drawing_area),
                                 ::gl_config,
                                 share_list,
                                 direct,
                                 render_type);

    gtk_widget_add_events(GTK_WIDGET(browser->priv->drawing_area),
                          GDK_EXPOSURE_MASK
                          | GDK_POINTER_MOTION_MASK
                          | GDK_BUTTON_PRESS_MASK
                          | GDK_BUTTON_RELEASE_MASK
                          | GDK_KEY_PRESS_MASK
                          | GDK_FOCUS_CHANGE_MASK);

    g_object_set(G_OBJECT(browser->priv->drawing_area),
                 "can-focus", true,
                 NULL);

    g_object_connect(
        G_OBJECT(browser->priv->drawing_area),

        "signal::expose_event",
        G_CALLBACK(openvrml_xembed_browser_expose_event),
        browser->priv->viewer,

        "signal::configure_event",
        G_CALLBACK(openvrml_xembed_browser_configure_event),
        browser->priv->viewer,

        "signal::key_press_event",
        G_CALLBACK(openvrml_xembed_browser_key_press_event),
        browser->priv->viewer,

        "signal::button_press_event",
        G_CALLBACK(openvrml_xembed_browser_button_press_event),
        browser->priv->viewer,

        "signal::button_release_event",
        G_CALLBACK(openvrml_xembed_browser_button_release_event),
        browser->priv->viewer,

        "signal::motion_notify_event",
        G_CALLBACK(openvrml_xembed_browser_motion_notify_event),
        browser->priv->viewer,
        NULL);

    gtk_container_add(GTK_CONTAINER(widget),
                      GTK_WIDGET(browser->priv->drawing_area));
}

void openvrml_xembed_browser_unrealize(GtkWidget * const widget)
{
    OpenvrmlXembedBrowser * const browser = OPENVRML_XEMBED_BROWSER(widget);

    if (browser->priv->expect_initial_stream) {
        browser->priv->initial_stream_reader_thread->join();
        delete browser->priv->initial_stream_reader_thread;
    }

    delete browser->priv->viewer;

    GtkWidgetClass * klass =
        GTK_WIDGET_CLASS(g_type_class_peek(OPENVRML_XEMBED_TYPE_BROWSER));
    GtkWidgetClass * parent_class =
        GTK_WIDGET_CLASS(g_type_class_peek_parent(klass));
    parent_class->unrealize(widget);
}

void
openvrml_xembed_browser_stream_client_interface_init(const gpointer g_iface,
                                                     gpointer /* iface_data */)
{
    OpenvrmlXembedStreamClientInterface * const iface =
        static_cast<OpenvrmlXembedStreamClientInterface *>(g_iface);
    iface->new_stream     = openvrml_xembed_browser_new_stream;
    iface->destroy_stream = openvrml_xembed_browser_destroy_stream;
    iface->write          = openvrml_xembed_browser_write;
}

GtkWidget * openvrml_xembed_browser_new(DBusGProxy * const host_proxy,
                                        const GdkNativeWindow socket_id,
                                        const gboolean expect_initial_stream)
{
    GtkWidget * browser =
        GTK_WIDGET(g_object_new(OPENVRML_XEMBED_TYPE_BROWSER,
                                "control-host-proxy", host_proxy,
                                "expect-initial-stream", expect_initial_stream,
                                0));
    gtk_plug_construct(GTK_PLUG(browser), socket_id);
    return browser;
}

GtkWidget * openvrml_xembed_browser_new_for_display(DBusGProxy * const host_proxy,
                                         GdkDisplay * const display,
                                         const GdkNativeWindow socket_id,
                                         const gboolean expect_initial_stream)
{
    GtkWidget * browser =
        GTK_WIDGET(g_object_new(OPENVRML_XEMBED_TYPE_BROWSER,
                                "control-host-proxy", host_proxy,
                                "expect-initial-stream", expect_initial_stream,
                                0));
    gtk_plug_construct_for_display(GTK_PLUG(browser), display, socket_id);
    return browser;
}

gboolean
openvrml_xembed_browser_new_stream(
    OpenvrmlXembedStreamClient * const stream_client,
    const guint64 stream_id,
    const char * const type,
    const char * const url,
    GError ** const error)
{
    using namespace openvrml_xembed;
    using boost::shared_ptr;

    OpenvrmlXembedBrowser * const browser =
        OPENVRML_XEMBED_BROWSER(stream_client);

    shared_ptr<plugin_streambuf> streambuf =
        browser->priv->uninitialized_streambuf_map->find(url);

    if (!streambuf) {
        if (!browser->priv->got_initial_stream) {
            g_assert(
                browser->priv->uninitialized_streambuf_map->size() == 1);
            streambuf =
                browser->priv->uninitialized_streambuf_map->front();
            browser->priv->got_initial_stream = true;
        } else {
            g_set_error(
                error,
                OPENVRML_XEMBED_ERROR,
                OPENVRML_XEMBED_ERROR_UNKNOWN_STREAM,
                "Attempt to create a stream that has not been requested: %s",
                url);
            return false;
        }
    }
    g_assert(streambuf->state() == plugin_streambuf::uninitialized);
    streambuf->init(stream_id, url, type);
    return true;
}

gboolean
openvrml_xembed_browser_destroy_stream(
    OpenvrmlXembedStreamClient * const stream_client,
    const guint64 stream_id,
    GError ** const error)
{
    using namespace openvrml_xembed;
    using boost::shared_ptr;

    OpenvrmlXembedBrowser * const browser =
        OPENVRML_XEMBED_BROWSER(stream_client);

    const shared_ptr<plugin_streambuf> streambuf =
        browser->priv->streambuf_map->find(stream_id);
    if (!streambuf) {
        g_set_error(
            error,
            OPENVRML_XEMBED_ERROR,
            OPENVRML_XEMBED_ERROR_UNKNOWN_STREAM,
            "Attempt to destroy a nonexistent stream: %lu",
            stream_id);
        return false;
    }
    streambuf->buf_.set_eof();
    browser->priv->streambuf_map->erase(stream_id);
    return true;
}

gboolean
openvrml_xembed_browser_write(OpenvrmlXembedStreamClient * const stream_client,
                              const guint64 stream_id,
                              const GArray * const data,
                              GError ** const error)
{
    using namespace openvrml_xembed;
    using boost::shared_ptr;

    OpenvrmlXembedBrowser * const browser =
        OPENVRML_XEMBED_BROWSER(stream_client);

    const shared_ptr<plugin_streambuf> streambuf =
        browser->priv->streambuf_map->find(stream_id);
    if (!streambuf) {
        g_set_error(
            error,
            OPENVRML_XEMBED_ERROR,
            OPENVRML_XEMBED_ERROR_UNKNOWN_STREAM,
            "Attempt to write to a nonexistent stream: %lu",
            stream_id);
        return false;
    }
    for (size_t i = 0; i < data->len; ++i) {
        streambuf->buf_.put(g_array_index(data, unsigned char, i));
    }
    return true;
}

gboolean openvrml_xembed_browser_expose_event(GtkWidget * const widget,
                                              GdkEventExpose * const event,
                                              const gpointer user_data)
{
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);
    GtkGLViewer * viewer = static_cast<GtkGLViewer *>(user_data);
    g_return_val_if_fail(viewer, true);
    {
        openvrml::read_write_mutex::scoped_read_lock
            lock(viewer->browser_initialized_mutex_);
        if (!viewer->browser_initialized_) { return true; }
    }

    if (event->count == 0
        && gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        viewer->redraw();
    }
    viewer->redrawNeeded = false;
    if (viewer->timer == 0) { viewer->timer_update(); }
    return true;
}

gboolean openvrml_xembed_browser_configure_event(GtkWidget * const widget,
                                                 GdkEventConfigure *,
                                                 const gpointer user_data)
{
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);

    GtkGLViewer * const viewer = static_cast<GtkGLViewer *>(user_data);
    g_return_val_if_fail(viewer, true);

    if (gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        viewer->resize(widget->allocation.width, widget->allocation.height);
    }
    return true;
}

gboolean openvrml_xembed_browser_key_press_event(GtkWidget * const widget,
                                                 GdkEventKey * const event,
                                                 const gpointer user_data)
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
        GtkGLViewer * const viewer = static_cast<GtkGLViewer *>(user_data);
        viewer->input(&info);
    }
    return true;
}

gboolean
openvrml_xembed_browser_button_press_event(GtkWidget * const widget,
                                           GdkEventButton * const event,
                                           const gpointer user_data)
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
        GtkGLViewer * const viewer = static_cast<GtkGLViewer *>(user_data);
        viewer->input(&info);
    }
    return true;
}

gboolean
openvrml_xembed_browser_button_release_event(GtkWidget * const widget,
                                             GdkEventButton * const event,
                                             const gpointer user_data)
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
        GtkGLViewer * const viewer = static_cast<GtkGLViewer *>(user_data);
        viewer->input(&info);
    }
    return true;
}

gboolean
openvrml_xembed_browser_motion_notify_event(GtkWidget * const widget,
                                            GdkEventMotion * const event,
                                            const gpointer user_data)
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
        GtkGLViewer * const viewer = static_cast<GtkGLViewer *>(user_data);
        openvrml::read_write_mutex::scoped_read_lock
            lock(viewer->browser_initialized_mutex_);
        if (!viewer->browser_initialized_) { return true; }
        viewer->input(&info);
    }
    return true;
}

gint openvrml_xembed_browser_timeout_callback(const gpointer ptr)
{
    g_assert(ptr);

    GtkGLViewer & viewer = *static_cast<GtkGLViewer *>(ptr);
    viewer.timer_update();
    return false;
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

        void operator()() const OPENVRML_NOTHROW
        {
            try {
                {
                    openvrml::read_write_mutex::scoped_write_lock
                        lock(this->viewer_->browser_initialized_mutex_);
                    this->viewer_->browser_initialized_ = false;
                }
                this->viewer_->vrml_browser_.priv->browser
                    ->load_url(this->url_, this->parameter_);
            } catch (std::exception & ex) {
                this->viewer_->vrml_browser_.priv->browser->err(ex.what());
            }
        }

    private:
        GtkGLViewer * const viewer_;
        const std::vector<std::string> url_, parameter_;
    };
}

gboolean
openvrml_xembed_browser_load_url(OpenvrmlXembedBrowser * const vrml_browser,
                                 const gchar ** url,
                                 const gchar ** parameter,
                                 GError ** /* error */)
{
    using std::string;
    using std::vector;

    vector<string> url_vec, param_vec;
    while (url && *url) { url_vec.push_back(*(url++)); }
    while (parameter && *parameter) { param_vec.push_back(*(parameter++)); }

    vrml_browser->priv->resource_fetcher
        ->create_thread(GtkGLViewer::load_url(*vrml_browser->priv->viewer,
                                              url_vec, param_vec));
    return true;
}

guint64 openvrml_xembed_browser_get_id(OpenvrmlXembedBrowser * const browser)
{
    g_assert(browser);
//    gdk_threads_enter();
//    scope_guard gdk_threads_guard = make_guard(gdk_threads_leave);
    return gtk_plug_get_id(&browser->parent);
}

gboolean openvrml_xembed_browser_new_stream(OpenvrmlXembedBrowser * const browser,
                                 const guint64 stream_id,
                                 const char * const type,
                                 const char * const url,
                                 GError ** const error)
{
    return openvrml_xembed_stream_client_new_stream(
        OPENVRML_XEMBED_STREAM_CLIENT(browser),
        stream_id,
        type,
        url,
        error);
}

gboolean
openvrml_xembed_browser_destroy_stream(OpenvrmlXembedBrowser * const browser,
                                       const guint64 stream_id,
                                       GError ** const error)
{
    return openvrml_xembed_stream_client_destroy_stream(
        OPENVRML_XEMBED_STREAM_CLIENT(browser),
        stream_id,
        error);
}

gboolean openvrml_xembed_browser_write(OpenvrmlXembedBrowser * const browser,
                                       const guint64 stream_id,
                                       const GArray * const data,
                                       GError ** const error)
{
    return openvrml_xembed_stream_client_write(
        OPENVRML_XEMBED_STREAM_CLIENT(browser),
        stream_id,
        data,
        error);
}

gchar *
openvrml_xembed_browser_get_world_url(
    OpenvrmlXembedBrowser * const vrml_browser,
    GError ** /* error */)
{
    return g_strdup(vrml_browser->priv->browser->world_url().c_str());
}

extern "C"
OPENVRML_LOCAL
void openvrml_xembed_get_url_notify(DBusGProxy * const proxy,
                                    DBusGProxyCall * const call_id,
                                    void * const user_data)
{
    using openvrml_xembed::plugin_streambuf;

    gint get_url_result;

    GError * error = 0;
    scope_guard error_guard = make_guard(g_error_free, boost::ref(error));
    const gboolean result =
        dbus_g_proxy_end_call(proxy, call_id, &error,
                              G_TYPE_INT, &get_url_result,
                              G_TYPE_INVALID);

    if (!result) {
        g_critical("Call to org.openvrml.BrowserHost.GetUrl failed: %s",
                   error->message);
        return;
    }

    plugin_streambuf * const streambuf =
        static_cast<plugin_streambuf *>(user_data);

    streambuf->set_get_url_result(get_url_result);

    error_guard.dismiss();
}

namespace {

    resource_fetcher::resource_fetcher(
        DBusGProxy & control_host,
        openvrml_xembed::uninitialized_plugin_streambuf_map &
            uninitialized_plugin_streambuf_map,
        openvrml_xembed::plugin_streambuf_map & plugin_streambuf_map):
        control_host_(control_host),
        uninitialized_plugin_streambuf_map_(uninitialized_plugin_streambuf_map),
        plugin_streambuf_map_(plugin_streambuf_map)
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
                streambuf_(
                    new plugin_streambuf(
                        uri,
                        fetcher.uninitialized_plugin_streambuf_map_,
                        fetcher.plugin_streambuf_map_)),
                resource_fetcher_(fetcher)
            {
                using std::ostringstream;
                using boost::ref;

                this->rdbuf(this->streambuf_.get());
                fetcher.uninitialized_plugin_streambuf_map_
                    .insert(uri, this->streambuf_);

                //
                // We're in a thread started by libopenvrml here.  By using
                // dbus_g_proxy_begin_call here (instead of dbus_g_proxy_call),
                // we finish the call in the main thread.  That's important
                // because we want to make sure this call "finishes" before
                // we execute the host's successive NewStream call.
                //
                GError * error = 0;
                scope_guard error_guard = make_guard(g_error_free,
                                                     boost::ref(error));
                DBusGProxyCall * get_url_call =
                    dbus_g_proxy_begin_call(&fetcher.control_host_,
                                            "GetUrl",
                                            openvrml_xembed_get_url_notify,
                                            this->streambuf_.get(),
                                            0,
                                            G_TYPE_STRING, uri.c_str(),
                                            G_TYPE_INVALID);
                if (!get_url_call) {
                    g_critical("Call to org.openvrml.BrowserHost.GetUrl "
                               "failed");
                    this->setstate(ios_base::badbit);
                    return;
                }

                //
                // This blocks until the call to GetUrl returns.
                //
                const int get_url_result = this->streambuf_->get_url_result();
                if (get_url_result != 0) {
                    this->setstate(ios_base::badbit);
                }

                error_guard.dismiss();
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

    browser_listener::browser_listener(GtkGLViewer & viewer):
        viewer_(viewer)
    {}

    void
    browser_listener::do_browser_changed(const openvrml::browser_event & event)
    {
        switch (event.id()) {
        case openvrml::browser_event::initialized:
            g_signal_emit(&viewer_.vrml_browser_, signals[initialized_id], 0);

            {
                openvrml::read_write_mutex::scoped_write_lock
                    lock(this->viewer_.browser_initialized_mutex_);
                this->viewer_.browser_initialized_ = true;
            }
//            gdk_threads_enter();
//            scope_guard gdk_threads_guard = make_guard(gdk_threads_leave);
//            boost::ignore_unused_variable_warning(gdk_threads_guard);
            //
            // Set redrawNeeded to false to ensure that this particular call to
            // post_redraw results in a redraw.
            //
            this->viewer_.redrawNeeded = false;
            this->viewer_.post_redraw();
            break;
        case openvrml::browser_event::shutdown:
            g_signal_emit(&viewer_.vrml_browser_, signals[shutdown_id], 0);
            break;
        default:
            break;
        }
    }

    GtkGLViewer::GtkGLViewer(OpenvrmlXembedBrowser & vrml_browser):
        browser_listener_(*this),
        browser_initialized_(true),
        vrml_browser_(vrml_browser),
        timer(0),
        redrawNeeded(false)
    {
        g_assert(vrml_browser.priv->control_host);
        this->vrml_browser_.priv->browser
            ->add_listener(this->browser_listener_);
        this->vrml_browser_.priv->browser->viewer(this);
    }

    GtkGLViewer::~GtkGLViewer() throw ()
    {
        if (this->timer) { g_source_remove(timer); }
        this->vrml_browser_.priv->browser
            ->remove_listener(this->browser_listener_);
    }

    void GtkGLViewer::post_redraw()
    {
        if (!this->redrawNeeded) {
            this->redrawNeeded = true;
            gtk_widget_queue_draw(
                GTK_WIDGET(this->vrml_browser_.priv->drawing_area));
        }
    }

    void GtkGLViewer::set_cursor(cursor_style style)
    {
        GdkCursor * cursor(0);

        switch(style) {
        case cursor_inherit:
            XDefineCursor(
                GDK_WINDOW_XDISPLAY(
                    GTK_WIDGET(this->vrml_browser_.priv->drawing_area)->window),
                GDK_WINDOW_XWINDOW(
                    GTK_WIDGET(this->vrml_browser_.priv->drawing_area)->window),
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

        gdk_window_set_cursor(
            GTK_WIDGET(this->vrml_browser_.priv->drawing_area)->window,
            cursor);
        gdk_cursor_destroy(cursor);
    }

    void GtkGLViewer::swap_buffers()
    {
        GdkGLDrawable * const gl_drawable =
            gtk_widget_get_gl_drawable(
                GTK_WIDGET(this->vrml_browser_.priv->drawing_area));
        gdk_gl_drawable_swap_buffers(gl_drawable);
    }

    void GtkGLViewer::set_timer(const double t)
    {
        if (!this->timer) {
            this->timer =
                g_timeout_add(
                    guint(10.0 * (t + 1)),
                    GtkFunction(openvrml_xembed_browser_timeout_callback),
                    this);
        }
    }

    void GtkGLViewer::timer_update()
    {
        this->timer = 0;
        this->viewer::update();
    }
}
