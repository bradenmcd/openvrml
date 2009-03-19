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

# include <boost/multi_index/detail/scope_guard.hpp>
// Must include before X11 headers.
# include <boost/numeric/conversion/converter.hpp>
# include <X11/keysym.h>
# include <dbus/dbus-glib.h>
# include <gdk/gdkx.h>
# include <openvrml_control/browser.h>
# include <openvrml/gl/viewer.h>
# include "browser.h"
# include "browser-server-glue.h"
# include "browser-host-client-glue.h"
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
    // OpenvrmlXembedStreamClient implementation
    //
    G_GNUC_INTERNAL
    void
    openvrml_xembed_browser_stream_client_interface_init(gpointer g_iface,
                                                         gpointer iface_data);
}

G_DEFINE_TYPE_WITH_CODE(OpenvrmlXembedBrowser,
                        openvrml_xembed_browser,
                        G_TYPE_OBJECT,
                        G_IMPLEMENT_INTERFACE(
                            OPENVRML_XEMBED_TYPE_STREAM_CLIENT,
                            openvrml_xembed_browser_stream_client_interface_init))

namespace {

    class G_GNUC_INTERNAL browser_listener : public openvrml::browser_listener {
        OpenvrmlXembedBrowser & browser_;

    public:
        explicit browser_listener(OpenvrmlXembedBrowser & browser);

    private:
        virtual void do_browser_changed(const openvrml::browser_event & event);
    };


    class G_GNUC_INTERNAL browser_host_proxy :
        public openvrml_control::browser_host {

        DBusGProxy & host_;

    public:
        explicit browser_host_proxy(DBusGProxy & host):
            host_(host)
        {}

    private:
        virtual int do_get_url(const std::string & url)
        {
            GError * error = 0;
            scope_guard error_guard = make_guard(g_error_free, boost::ref(error));
            gint result = -1;
            gboolean succeeded =
                org_openvrml_BrowserHost_get_url(&this->host_,
                                                 url.c_str(),
                                                 &result,
                                                 &error);
            if (!succeeded) {
                throw std::invalid_argument(error->message);
            }

            error_guard.dismiss();

            return result;
        }
    };
}

struct OpenvrmlXembedBrowserPrivate_ {
    DBusGProxy * control_host;
    browser_host_proxy * browser_control_host_proxy;
    openvrml_control::browser * browser_control;
    browser_listener * listener;
    OpenvrmlXembedBrowserPlug * browser_plug;
    GMutex * browser_plug_mutex;
    GCond * browser_plug_set_cond;
    bool expect_initial_stream;
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

extern "C" {
    G_GNUC_INTERNAL void openvrml_xembed_browser_finalize(GObject * obj);
}

void
openvrml_xembed_browser_class_init(OpenvrmlXembedBrowserClass * const klass)
{
    GObjectClass * const g_object_class = G_OBJECT_CLASS(klass);

    g_object_class->constructor  = openvrml_xembed_browser_constructor;
    g_object_class->finalize     = openvrml_xembed_browser_finalize;
    g_object_class->set_property = openvrml_xembed_browser_set_property;
    g_object_class->get_property = openvrml_xembed_browser_get_property;

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
        g_param_spec_object(
            "control-host-proxy",
            "BrowserHost proxy",
            "DBusGProxy for a BrowserHost",
            DBUS_TYPE_G_PROXY,
            GParamFlags(G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));
    g_object_class_install_property(g_object_class,
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

    g_type_class_add_private(g_object_class,
                             sizeof (OpenvrmlXembedBrowserPrivate));

    dbus_g_object_type_install_info(
        OPENVRML_XEMBED_TYPE_BROWSER,
        &dbus_glib_openvrml_xembed_browser_object_info);
}

GObject *
openvrml_xembed_browser_constructor(
    GType type,
    guint n_construct_properties,
    GObjectConstructParam * construct_properties)
{
    GObject * obj;
    {
        OpenvrmlXembedBrowserClass * const klass =
            OPENVRML_XEMBED_BROWSER_CLASS(
                g_type_class_peek(OPENVRML_XEMBED_TYPE_BROWSER));
        GObjectClass * const parent_class =
            G_OBJECT_CLASS(g_type_class_peek_parent(klass));
        obj = parent_class->constructor(type,
                                        n_construct_properties,
                                        construct_properties);
    }

    try {
        OpenvrmlXembedBrowser * const browser = OPENVRML_XEMBED_BROWSER(obj);
        browser->priv->browser_control_host_proxy =
            new browser_host_proxy(*browser->priv->control_host);
        browser->priv->browser_control =
            new openvrml_control::browser(
                *browser->priv->browser_control_host_proxy,
                browser->priv->expect_initial_stream);
        browser->priv->listener = new browser_listener(*browser);
        browser->priv->browser_control->add_listener(*browser->priv->listener);

        browser->priv->browser_plug          = 0;
        browser->priv->browser_plug_mutex    = g_mutex_new();
        browser->priv->browser_plug_set_cond = g_cond_new();
    } catch (std::exception & ex) {
        //
        // ex is most likely std::bad_alloc.
        //
        g_critical("%s", ex.what());
        return 0;
    }
    return obj;
}

void openvrml_xembed_browser_finalize(GObject * const obj)
{
    OpenvrmlXembedBrowser * const browser = OPENVRML_XEMBED_BROWSER(obj);
    g_cond_free(browser->priv->browser_plug_set_cond);
    g_mutex_free(browser->priv->browser_plug_mutex);
    browser->priv->browser_control->remove_listener(*browser->priv->listener);
    delete browser->priv->listener;
    delete browser->priv->browser_control;
    delete browser->priv->browser_control_host_proxy;
}

void openvrml_xembed_browser_set_property(GObject * const obj,
                                          const guint property_id,
                                          const GValue * const value,
                                          GParamSpec * const pspec)
{
    OpenvrmlXembedBrowser * const browser = OPENVRML_XEMBED_BROWSER(obj);
    switch (property_id) {
    case control_host_proxy_id:
        browser->priv->control_host = DBUS_G_PROXY(g_value_get_object(value));
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
        g_value_set_object(value, browser->priv->control_host);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
    }
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

G_GNUC_INTERNAL
GSource *
openvrml_xembed_browser_ready_source_new(OpenvrmlXembedBrowser * browser,
                                         guint64 host_id);

OpenvrmlXembedBrowser *
openvrml_xembed_browser_new(DBusGProxy * const host_proxy,
                            const gboolean expect_initial_stream,
                            GMainContext * const gtk_thread_context,
                            const GdkNativeWindow socket_id)
{
    OpenvrmlXembedBrowser * const browser =
        OPENVRML_XEMBED_BROWSER(
            g_object_new(OPENVRML_XEMBED_TYPE_BROWSER,
                         "control-host-proxy", host_proxy,
                         "expect-initial-stream", expect_initial_stream,
                         0));
    if (!browser) { return 0; }
    scope_guard browser_guard = make_guard(g_object_unref, browser);

    GSource * const browser_ready_source =
        openvrml_xembed_browser_ready_source_new(browser, socket_id);
    if (!browser_ready_source) { return 0; }
    scope_guard browser_ready_source_guard =
        make_guard(g_object_unref, browser_ready_source);

    gdk_threads_enter();
    g_source_attach(browser_ready_source, gtk_thread_context);
    gdk_threads_leave();

    browser_ready_source_guard.dismiss();
    browser_guard.dismiss();

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
    using boost::shared_ptr;

    OpenvrmlXembedBrowser * const browser =
        OPENVRML_XEMBED_BROWSER(stream_client);

    try {
        browser->priv->browser_control->new_stream(stream_id, type, url);
    } catch (const openvrml_control::unknown_stream & ex) {
        g_set_error(error,
                    OPENVRML_XEMBED_ERROR,
                    OPENVRML_XEMBED_ERROR_UNKNOWN_STREAM,
                    ex.what());
        return false;
    }
    return true;
}

gboolean
openvrml_xembed_browser_destroy_stream(
    OpenvrmlXembedStreamClient * const stream_client,
    const guint64 stream_id,
    GError ** const error)
{
    using boost::shared_ptr;

    OpenvrmlXembedBrowser * const browser =
        OPENVRML_XEMBED_BROWSER(stream_client);

    try {
        browser->priv->browser_control->destroy_stream(stream_id);
    } catch (const openvrml_control::unknown_stream & ex) {
        g_set_error(error,
                    OPENVRML_XEMBED_ERROR,
                    OPENVRML_XEMBED_ERROR_UNKNOWN_STREAM,
                    ex.what());
        return false;
    }
    return true;
}

gboolean
openvrml_xembed_browser_write(OpenvrmlXembedStreamClient * const stream_client,
                              const guint64 stream_id,
                              const GArray * const data,
                              GError ** const error)
{
    using boost::shared_ptr;

    OpenvrmlXembedBrowser * const browser =
        OPENVRML_XEMBED_BROWSER(stream_client);

    try {
        browser->priv->browser_control->write(
            stream_id,
            reinterpret_cast<unsigned char *>(data->data),
            data->len);
    } catch (const openvrml_control::unknown_stream & ex) {
        g_set_error(error,
                    OPENVRML_XEMBED_ERROR,
                    OPENVRML_XEMBED_ERROR_UNKNOWN_STREAM,
                    ex.what());
    }
    return true;
}

gboolean
openvrml_xembed_browser_load_url(OpenvrmlXembedBrowser * const vrml_browser,
                                 const gchar ** url,
                                 const gchar ** parameter,
                                 GError ** error)
{
    using std::string;
    using std::vector;

    try {
        vector<string> url_vec, param_vec;
        while (url && *url) { url_vec.push_back(*(url++)); }
        while (parameter && *parameter) { param_vec.push_back(*(parameter++)); }

        vrml_browser->priv->browser_control->load_uri(url_vec, param_vec);
    } catch (const std::bad_alloc & ex) {
        *error = g_error_new(OPENVRML_XEMBED_ERROR,
                             OPENVRML_XEMBED_ERROR_NO_MEMORY,
                             "out of memory");
        return false;
    } catch (const std::exception & ex) {
        *error = g_error_new(OPENVRML_XEMBED_ERROR,
                             OPENVRML_XEMBED_ERROR_FAILED,
                             ex.what());
        return false;
    }
    return true;
}

guint64 openvrml_xembed_browser_get_id(OpenvrmlXembedBrowser * const browser)
{
    g_assert(browser);
    g_mutex_lock(browser->priv->browser_plug_mutex);
    while (!browser->priv->browser_plug) {
        g_cond_wait(browser->priv->browser_plug_set_cond,
                    browser->priv->browser_plug_mutex);
    }
    g_assert(browser->priv->browser_plug);
    const guint64 id = gtk_plug_get_id(GTK_PLUG(browser->priv->browser_plug));
    g_mutex_unlock(browser->priv->browser_plug_mutex);
    return id;
}

gboolean
openvrml_xembed_browser_new_stream(OpenvrmlXembedBrowser * const browser,
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
    OpenvrmlXembedBrowser * const browser,
    GError ** /* error */)
{
    return g_strdup(browser->priv->browser_control->world_url().c_str());
}

gboolean
openvrml_xembed_browser_initialized(OpenvrmlXembedBrowser * const browser)
{
    return browser->priv->browser_control->initialized();
}


G_DEFINE_TYPE(OpenvrmlXembedBrowserPlug,
              openvrml_xembed_browser_plug,
              GTK_TYPE_PLUG);

extern "C" {
    G_GNUC_INTERNAL
    void openvrml_xembed_browser_plug_set_property(GObject * obj,
                                                   guint property_id,
                                                   const GValue * value,
                                                   GParamSpec * pspec);

    G_GNUC_INTERNAL
    void openvrml_xembed_browser_plug_get_property(GObject * obj,
                                                   guint property_id,
                                                   GValue * value,
                                                   GParamSpec * pspec);

    G_GNUC_INTERNAL
    void openvrml_xembed_browser_plug_realize(GtkWidget * widget);

    G_GNUC_INTERNAL
    void openvrml_xembed_browser_plug_unrealize(GtkWidget * widget);

    //
    // Signal handlers
    //
    G_GNUC_INTERNAL
    gboolean
    openvrml_xembed_drawing_area_expose_event(
        GtkWidget * widget,
        GdkEventExpose * event,
        OpenvrmlXembedBrowserPlug * browser_plug);

    G_GNUC_INTERNAL
    gboolean
    openvrml_xembed_drawing_area_configure_event(
        GtkWidget * widget,
        GdkEventConfigure * event,
        OpenvrmlXembedBrowserPlug * browser_plug);

    G_GNUC_INTERNAL
    gboolean
    openvrml_xembed_drawing_area_key_press_event(
        GtkWidget * widget,
        GdkEventKey * event,
        OpenvrmlXembedBrowserPlug * browser_plug);

    G_GNUC_INTERNAL
    gboolean
    openvrml_xembed_drawing_area_button_press_event(
        GtkWidget * widget,
        GdkEventButton * event,
        OpenvrmlXembedBrowserPlug * browser_plug);

    G_GNUC_INTERNAL
    gboolean
    openvrml_xembed_drawing_area_button_release_event(
        GtkWidget * widget,
        GdkEventButton * event,
        OpenvrmlXembedBrowserPlug * browser_plug);

    G_GNUC_INTERNAL
    gboolean
    openvrml_xembed_drawing_area_motion_notify_event(
        GtkWidget * widget,
        GdkEventMotion * event,
        OpenvrmlXembedBrowserPlug * browser_plug);

    G_GNUC_INTERNAL
    gint openvrml_xembed_browser_plug_timeout_callback(gpointer ptr);

    G_GNUC_INTERNAL
    void browser_initialized(OpenvrmlXembedBrowser *,
                             OpenvrmlXembedBrowserPlug *);
}

namespace {

    G_GNUC_INTERNAL GdkGLConfig * gl_config;


    class G_GNUC_INTERNAL GtkGLViewer : public openvrml::gl::viewer {
        friend
        void (::openvrml_xembed_browser_plug_realize)(GtkWidget * widget);

        friend gboolean
        (::openvrml_xembed_drawing_area_expose_event)(
            GtkWidget *,
            GdkEventExpose *,
            OpenvrmlXembedBrowserPlug *);

        friend void (::browser_initialized)(OpenvrmlXembedBrowser *,
                                            OpenvrmlXembedBrowserPlug *);

        OpenvrmlXembedBrowserPlug & browser_plug_;
        guint timer;

    public:
        explicit GtkGLViewer(OpenvrmlXembedBrowserPlug & browser_plug);
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

struct OpenvrmlXembedBrowserPlugPrivate_ {
    OpenvrmlXembedBrowser * browser;
    GtkDrawingArea * drawing_area;
    GtkGLViewer * viewer;
    bool redraw_needed;
};

#   define OPENVRML_XEMBED_BROWSER_PLUG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), OPENVRML_XEMBED_TYPE_BROWSER_PLUG, OpenvrmlXembedBrowserPlugPrivate))

void
openvrml_xembed_browser_plug_init(
    OpenvrmlXembedBrowserPlug * const browser_plug)
{
    browser_plug->priv = OPENVRML_XEMBED_BROWSER_PLUG_GET_PRIVATE(browser_plug);
}

namespace {
    enum browser_plug_property_id {
        browser_id = 1
    };
}

void
openvrml_xembed_browser_plug_class_init(
    OpenvrmlXembedBrowserPlugClass * const klass)
{
    GObjectClass * const g_object_class = G_OBJECT_CLASS(klass);
    g_object_class->set_property = openvrml_xembed_browser_plug_set_property;
    g_object_class->get_property = openvrml_xembed_browser_plug_get_property;

    GtkWidgetClass * const gtk_widget_class = GTK_WIDGET_CLASS(klass);
    gtk_widget_class->realize   = openvrml_xembed_browser_plug_realize;
    gtk_widget_class->unrealize = openvrml_xembed_browser_plug_unrealize;

    GParamSpec * pspec =
        g_param_spec_object(
            "browser",
            "OpenvrmlXembedBrowser",
            "The OpenvrmlXembedBrowser associated with the plug",
            OPENVRML_XEMBED_TYPE_BROWSER,
            GParamFlags(G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));
    g_object_class_install_property(g_object_class, browser_id, pspec);

    g_type_class_add_private(g_object_class,
                             sizeof (OpenvrmlXembedBrowserPrivate));
}

void openvrml_xembed_browser_plug_set_property(GObject * const obj,
                                               const guint property_id,
                                               const GValue * const value,
                                               GParamSpec * const pspec)
{
    OpenvrmlXembedBrowserPlug * const browser_plug =
        OPENVRML_XEMBED_BROWSER_PLUG(obj);
    switch (property_id) {
    case browser_id:
        g_assert(browser_plug->priv->browser == 0);
        browser_plug->priv->browser =
            OPENVRML_XEMBED_BROWSER(g_value_get_object(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
    }
}

void openvrml_xembed_browser_plug_get_property(GObject * const obj,
                                               const guint property_id,
                                               GValue * const value,
                                               GParamSpec * const pspec)
{
    OpenvrmlXembedBrowserPlug * const browser_plug =
        OPENVRML_XEMBED_BROWSER_PLUG(obj);
    switch (property_id) {
    case browser_id:
        g_value_set_object(value, browser_plug->priv->browser);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
    }
}

GtkWidget *
openvrml_xembed_browser_plug_new(OpenvrmlXembedBrowser * browser,
                                 GdkNativeWindow socket_id)
{
    GtkWidget * browser_plug =
        GTK_WIDGET(g_object_new(OPENVRML_XEMBED_TYPE_BROWSER_PLUG,
                                "browser", browser,
                                0));
    gtk_plug_construct(GTK_PLUG(browser_plug), socket_id);

    g_signal_connect_data(browser,
                          "initialized",
                          G_CALLBACK(browser_initialized),
                          browser_plug,
                          0,
                          GConnectFlags(0));

    return browser_plug;
}

void browser_initialized(OpenvrmlXembedBrowser * /* browser */,
                         OpenvrmlXembedBrowserPlug * const browser_plug)
{
    g_assert(OPENVRML_XEMBED_IS_BROWSER_PLUG(browser_plug));
    //
    // Set redraw_needed to false to ensure that the following call to
    // post_redraw results in a redraw.
    //
    browser_plug->priv->redraw_needed = false;

    //
    // viewer will be nonnull if the plug has been realized.
    //
    if (browser_plug->priv->viewer) {
        browser_plug->priv->viewer->post_redraw();
    }
}

void openvrml_xembed_browser_plug_realize(GtkWidget * const widget)
{
    GtkWidgetClass * klass =
        GTK_WIDGET_CLASS(
            g_type_class_peek(OPENVRML_XEMBED_TYPE_BROWSER_PLUG));
    GtkWidgetClass * parent_class =
        GTK_WIDGET_CLASS(g_type_class_peek_parent(klass));
    parent_class->realize(widget);

    OpenvrmlXembedBrowserPlug * const browser_plug =
        OPENVRML_XEMBED_BROWSER_PLUG(widget);

    {
        OpenvrmlXembedBrowser * browser = 0;
        g_object_get(browser_plug,
                     "browser", &browser,
                     NULL);

        g_assert(browser);
        scope_guard browser_guard = make_guard(g_object_unref, browser);
        boost::ignore_unused_variable_warning(browser_guard);

        browser_plug->priv->drawing_area =
            GTK_DRAWING_AREA(g_object_new(GTK_TYPE_DRAWING_AREA, 0));

        try {
            browser_plug->priv->viewer = new GtkGLViewer(*browser_plug);
        } catch (const std::exception & ex) {
            //
            // ex is most likely std::bad_alloc.
            //
            g_critical("%s", ex.what());
            return;
        }
    } // unref the OpenvrmlXembedBrowser.

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
    gtk_widget_set_gl_capability(GTK_WIDGET(browser_plug->priv->drawing_area),
                                 ::gl_config,
                                 share_list,
                                 direct,
                                 render_type);

    gtk_widget_add_events(GTK_WIDGET(browser_plug->priv->drawing_area),
                          GDK_EXPOSURE_MASK
                          | GDK_POINTER_MOTION_MASK
                          | GDK_BUTTON_PRESS_MASK
                          | GDK_BUTTON_RELEASE_MASK
                          | GDK_KEY_PRESS_MASK
                          | GDK_FOCUS_CHANGE_MASK);

    g_object_set(G_OBJECT(browser_plug->priv->drawing_area),
                 "can-focus", true,
                 NULL);

    g_object_connect(
        G_OBJECT(browser_plug->priv->drawing_area),

        "signal::expose_event",
        G_CALLBACK(openvrml_xembed_drawing_area_expose_event),
        browser_plug,

        "signal::configure_event",
        G_CALLBACK(openvrml_xembed_drawing_area_configure_event),
        browser_plug,

        "signal::key_press_event",
        G_CALLBACK(openvrml_xembed_drawing_area_key_press_event),
        browser_plug,

        "signal::button_press_event",
        G_CALLBACK(openvrml_xembed_drawing_area_button_press_event),
        browser_plug,

        "signal::button_release_event",
        G_CALLBACK(openvrml_xembed_drawing_area_button_release_event),
        browser_plug,

        "signal::motion_notify_event",
        G_CALLBACK(openvrml_xembed_drawing_area_motion_notify_event),
        browser_plug,
        NULL);

    gtk_container_add(GTK_CONTAINER(widget),
                      GTK_WIDGET(browser_plug->priv->drawing_area));

    //
    // If the browser has already been initialized, get things moving.
    //
    if (openvrml_xembed_browser_initialized(browser_plug->priv->browser)) {
        browser_plug->priv->viewer->post_redraw();
    }
}

void openvrml_xembed_browser_plug_unrealize(GtkWidget * const widget)
{
    OpenvrmlXembedBrowserPlug * const browser_plug =
        OPENVRML_XEMBED_BROWSER_PLUG(widget);

    delete browser_plug->priv->viewer;
    browser_plug->priv->viewer = 0;
    gtk_widget_destroy(GTK_WIDGET(browser_plug->priv->drawing_area));
    browser_plug->priv->drawing_area = 0;

    GtkWidgetClass * klass =
        GTK_WIDGET_CLASS(g_type_class_peek(OPENVRML_XEMBED_TYPE_BROWSER_PLUG));
    GtkWidgetClass * parent_class =
        GTK_WIDGET_CLASS(g_type_class_peek_parent(klass));
    parent_class->unrealize(widget);
}

gboolean
openvrml_xembed_drawing_area_expose_event(
    GtkWidget * const widget,
    GdkEventExpose * const event,
    OpenvrmlXembedBrowserPlug * const browser_plug)
{
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);

    if (!openvrml_xembed_browser_initialized(browser_plug->priv->browser)) {
        return true;
    }

    if (event->count == 0
        && gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        browser_plug->priv->viewer->redraw();
    }
    browser_plug->priv->redraw_needed = false;
    if (browser_plug->priv->viewer->timer == 0) {
        browser_plug->priv->viewer->timer_update();
    }
    return true;
}

gboolean
openvrml_xembed_drawing_area_configure_event(
    GtkWidget * const widget,
    GdkEventConfigure *,
    OpenvrmlXembedBrowserPlug * const browser_plug)
{
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);

    if (gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        browser_plug->priv->viewer->resize(widget->allocation.width,
                                           widget->allocation.height);
    }
    return true;
}

gboolean
openvrml_xembed_drawing_area_key_press_event(
    GtkWidget * const widget,
    GdkEventKey * const event,
    OpenvrmlXembedBrowserPlug * const browser_plug)
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
        browser_plug->priv->viewer->input(&info);
    }
    return true;
}

gboolean
openvrml_xembed_drawing_area_button_press_event(
    GtkWidget * const widget,
    GdkEventButton * const event,
    OpenvrmlXembedBrowserPlug * const browser_plug)
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
        browser_plug->priv->viewer->input(&info);
    }
    return true;
}

gboolean
openvrml_xembed_drawing_area_button_release_event(
    GtkWidget * const widget,
    GdkEventButton * const event,
    OpenvrmlXembedBrowserPlug * const browser_plug)
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
        browser_plug->priv->viewer->input(&info);
    }
    return true;
}

gboolean
openvrml_xembed_drawing_area_motion_notify_event(
    GtkWidget * const widget,
    GdkEventMotion * const event,
    OpenvrmlXembedBrowserPlug * const browser_plug)
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
        if (!openvrml_xembed_browser_initialized(browser_plug->priv->browser)) {
            return true;
        }
        browser_plug->priv->viewer->input(&info);
    }
    return true;
}

gint openvrml_xembed_browser_plug_timeout_callback(const gpointer ptr)
{
    g_assert(ptr);

    GtkGLViewer & viewer = *static_cast<GtkGLViewer *>(ptr);
    viewer.timer_update();
    return false;
}

namespace {

    browser_listener::browser_listener(OpenvrmlXembedBrowser & browser):
        browser_(browser)
    {}

    void
    browser_listener::do_browser_changed(const openvrml::browser_event & event)
    {
        switch (event.id()) {
        case openvrml::browser_event::initialized:
            g_signal_emit(&this->browser_, signals[initialized_id], 0);
            break;
        case openvrml::browser_event::shutdown:
            g_signal_emit(&this->browser_, signals[shutdown_id], 0);
            break;
        default:
            break;
        }
    }

    GtkGLViewer::GtkGLViewer(OpenvrmlXembedBrowserPlug & browser_plug):
        browser_plug_(browser_plug),
        timer(0)
    {
        this->browser_plug_.priv->browser->priv->browser_control->viewer(this);
    }

    GtkGLViewer::~GtkGLViewer() throw ()
    {
        if (this->timer) { g_source_remove(timer); }
    }

    void GtkGLViewer::post_redraw()
    {
        if (!this->browser_plug_.priv->redraw_needed) {
            this->browser_plug_.priv->redraw_needed = true;
            gtk_widget_queue_draw(
                GTK_WIDGET(this->browser_plug_.priv->drawing_area));
        }
    }

    void GtkGLViewer::set_cursor(cursor_style style)
    {
        GdkCursor * cursor(0);

        switch(style) {
        case cursor_inherit:
            XDefineCursor(
                GDK_WINDOW_XDISPLAY(
                    GTK_WIDGET(this->browser_plug_.priv->drawing_area)->window),
                GDK_WINDOW_XWINDOW(
                    GTK_WIDGET(this->browser_plug_.priv->drawing_area)->window),
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
            GTK_WIDGET(this->browser_plug_.priv->drawing_area)->window,
            cursor);
        gdk_cursor_destroy(cursor);
    }

    void GtkGLViewer::swap_buffers()
    {
        GdkGLDrawable * const gl_drawable =
            gtk_widget_get_gl_drawable(
                GTK_WIDGET(this->browser_plug_.priv->drawing_area));
        gdk_gl_drawable_swap_buffers(gl_drawable);
    }

    void GtkGLViewer::set_timer(const double t)
    {
        if (!this->timer) {
            this->timer =
                g_timeout_add(
                    guint(10.0 * (t + 1)),
                    GtkFunction(openvrml_xembed_browser_plug_timeout_callback),
                    this);
        }
    }

    void GtkGLViewer::timer_update()
    {
        this->timer = 0;
        this->viewer::update();
    }
}

extern "C" {
    G_GNUC_INTERNAL
    gboolean openvrml_xembed_browser_ready_prepare(GSource * source,
                                                   gint * timeout);
    G_GNUC_INTERNAL
    gboolean openvrml_xembed_browser_ready_check(GSource * source);
    G_GNUC_INTERNAL
    gboolean openvrml_xembed_browser_ready_dispatch(GSource * source,
                                                    GSourceFunc callback,
                                                    gpointer user_data);
    G_GNUC_INTERNAL
    void openvrml_xembed_browser_ready_finalize(GSource * source);
}

struct OpenvrmlXembedBrowserReadySource {
    GSource source;
    OpenvrmlXembedBrowser * ready_browser;
    guint64 host_id;
};

GSource *
openvrml_xembed_browser_ready_source_new(OpenvrmlXembedBrowser * const browser,
                                         const guint64 host_id)
{
    static GSourceFuncs source_funcs = {
        openvrml_xembed_browser_ready_prepare,
        openvrml_xembed_browser_ready_check,
        openvrml_xembed_browser_ready_dispatch,
        openvrml_xembed_browser_ready_finalize,
        GSourceFunc(0),
        GSourceDummyMarshal(0)
    };

    OpenvrmlXembedBrowserReadySource * const source =
        static_cast<OpenvrmlXembedBrowserReadySource *>(
            static_cast<void *>(
                g_source_new(&source_funcs,
                             sizeof (OpenvrmlXembedBrowserReadySource))));
    if (!source) { return 0; }

    source->ready_browser = browser;
    source->host_id       = host_id;

    return static_cast<GSource *>(static_cast<void *>(source));
}

gboolean openvrml_xembed_browser_ready_prepare(GSource * /* source */,
                                               gint * const timeout)
{
    *timeout = 0;
    return true;
}

gboolean openvrml_xembed_browser_ready_check(GSource * /* source */)
{
    return true;
}

gboolean openvrml_xembed_browser_ready_dispatch(GSource * const source,
                                                GSourceFunc /* callback */,
                                                gpointer /* user_data */)
{
    OpenvrmlXembedBrowserReadySource * const browser_ready_source =
        static_cast<OpenvrmlXembedBrowserReadySource *>(
            static_cast<void *>(source));

    GtkWidget * const browser_plug =
        openvrml_xembed_browser_plug_new(browser_ready_source->ready_browser,
                                         browser_ready_source->host_id);

    g_mutex_lock(browser_ready_source->ready_browser->priv->browser_plug_mutex);
    browser_ready_source->ready_browser->priv->browser_plug =
        OPENVRML_XEMBED_BROWSER_PLUG(browser_plug);
    g_cond_signal(
        browser_ready_source->ready_browser->priv->browser_plug_set_cond);
    g_mutex_unlock(
        browser_ready_source->ready_browser->priv->browser_plug_mutex);

    gtk_widget_realize(browser_plug);
    gtk_widget_show_all(browser_plug);

    g_source_destroy(source);
    return true;
}

void openvrml_xembed_browser_ready_finalize(GSource * /* source */)
{
}
