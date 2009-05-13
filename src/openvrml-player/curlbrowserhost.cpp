// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML Player
// Copyright 2008  Braden McDaniel
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

# include "curlbrowserhost.h"
# include <curl/curl.h>
# include <libgnomevfs/gnome-vfs.h>
# include <browser-host-server-glue.h>
# include <browser-factory-client-glue.h>
# include <browser-client-glue.h>
# include <dbus/dbus-glib-bindings.h>
# include <dbus/dbus-protocol.h>
# include <boost/concept_check.hpp>
# include <boost/multi_index/detail/scope_guard.hpp>
# include <boost/ref.hpp>
# include <list>
# include <vector>
# include <string.h>

using namespace boost::multi_index::detail; // for scope_guard

G_DEFINE_TYPE(OpenvrmlPlayerCurlBrowserHost,
              openvrml_player_curl_browser_host,
              GTK_TYPE_SOCKET)

namespace {
    G_GNUC_INTERNAL
    DBusGProxy * get_browser(DBusGConnection * connection,
                             const char * host_name,
                             const char * host_path,
                             guint64 host_id,
                             GError ** error)
        throw ();

    G_GNUC_INTERNAL GSource * curl_source_new(CURLM * multi_handle)
        throw (std::bad_alloc);
}

extern "C" {
    G_GNUC_INTERNAL
    void
    openvrml_player_curl_browser_host_get_property(GObject * obj,
                                                   guint property_id,
                                                   GValue * value,
                                                   GParamSpec * pspec);
    G_GNUC_INTERNAL
    void openvrml_player_curl_browser_host_realize(GtkWidget * widget);
    G_GNUC_INTERNAL
    void openvrml_player_curl_browser_host_unrealize(GtkWidget * widget);
    G_GNUC_INTERNAL
    void openvrml_player_curl_browser_host_finalize(GObject * obj);
    

    G_GNUC_INTERNAL
    void
    openvrml_player_curl_browser_host_delete_curl_stream_data(
        void * stream_data);
    G_GNUC_INTERNAL
    gboolean
    openvrml_player_curl_browser_host_curl_source_callback(gpointer data);
}

struct OpenvrmlPlayerCurlBrowserHostPrivate_ {
    gchar * path;
    DBusGProxy * browser;
    CURLM * multi_handle;
    GSource * curl_source;
    GHashTable * stream_data;
};

#   define OPENVRML_PLAYER_CURL_BROWSER_HOST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), OPENVRML_PLAYER_TYPE_CURL_BROWSER_HOST, OpenvrmlPlayerCurlBrowserHostPrivate))

void
openvrml_player_curl_browser_host_init(
    OpenvrmlPlayerCurlBrowserHost * const browser_host)
{
    browser_host->priv =
        OPENVRML_PLAYER_CURL_BROWSER_HOST_GET_PRIVATE(browser_host);

    static unsigned long count = 0;
    browser_host->priv->path =
        g_strdup_printf("/org/openvrml/BrowserHost/%u/%lu",
                        getpid(), count++);
    dbus_g_connection_register_g_object(
        OPENVRML_PLAYER_CURL_BROWSER_HOST_GET_CLASS(browser_host)->connection,
        browser_host->priv->path,
        G_OBJECT(browser_host));
}

namespace {
    enum curl_browser_host_property_id {
        object_path_id = 1
    };

    enum curl_browser_host_signal_id {
        shutdown_id,
        last_signal_id
    };

    G_GNUC_INTERNAL guint signals[last_signal_id];
}

void
openvrml_player_curl_browser_host_class_init(
    OpenvrmlPlayerCurlBrowserHostClass * klass)
{
    G_OBJECT_CLASS(klass)->get_property =
        openvrml_player_curl_browser_host_get_property;
    G_OBJECT_CLASS(klass)->finalize =
        openvrml_player_curl_browser_host_finalize;
    GTK_WIDGET_CLASS(klass)->realize =
        openvrml_player_curl_browser_host_realize;
    GTK_WIDGET_CLASS(klass)->unrealize =
        openvrml_player_curl_browser_host_unrealize;

    signals[shutdown_id] =
        g_signal_new("shutdown",
                     G_OBJECT_CLASS_TYPE(klass),
                     GSignalFlags(G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED),
                     0,
                     NULL, NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE, 0);

    GError * error = 0;
    scope_guard error_guard = make_guard(g_error_free, boost::ref(error));
    klass->connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
    if (!klass->connection) {
        g_critical("Failed to open connection to bus: %s", error->message);
        return;
    }

    klass->host_name = g_strdup_printf("org.openvrml.BrowserHost-%u",
                                       getpid());

    DBusGProxy * driver_proxy =
        dbus_g_proxy_new_for_name(klass->connection,
                                  DBUS_SERVICE_DBUS,
                                  DBUS_PATH_DBUS,
                                  DBUS_INTERFACE_DBUS);
    scope_guard driver_proxy_guard = make_guard(g_object_unref, driver_proxy);
    boost::ignore_unused_variable_warning(driver_proxy_guard);

    guint request_ret;
    if (!org_freedesktop_DBus_request_name(driver_proxy,
                                           klass->host_name,
                                           0, &request_ret,
                                           &error)) {
        g_critical("Request for name \"%s\" failed: %s",
                   klass->host_name, error->message);
        return;
    }

    GParamSpec * pspec =
        g_param_spec_pointer("object-path",
                             "object path",
                             "D-Bus object path",
                             G_PARAM_READABLE);
    g_object_class_install_property(G_OBJECT_CLASS(klass),
                                    object_path_id,
                                    pspec);

    g_type_class_add_private(G_OBJECT_CLASS(klass),
                             sizeof (OpenvrmlPlayerCurlBrowserHostPrivate));

    dbus_g_object_type_install_info(
        OPENVRML_PLAYER_TYPE_CURL_BROWSER_HOST,
        &dbus_glib_openvrml_player_curl_browser_host_object_info);
    error_guard.dismiss();
}

void openvrml_player_curl_browser_host_get_property(GObject * const obj,
                                                    const guint property_id,
                                                    GValue * const value,
                                                    GParamSpec * const pspec)
{
    OpenvrmlPlayerCurlBrowserHost * host =
        OPENVRML_PLAYER_CURL_BROWSER_HOST(obj);
    switch (property_id) {
    case object_path_id:
        g_value_set_string(value, host->priv->path);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
    }
}

namespace {

    class G_GNUC_INTERNAL curl_stream_data {
        CURL * const handle_;
        std::vector<char> url_;
        DBusGProxy * browser_;
        bool initialized_;

    public:
        curl_stream_data(CURL * handle, const char * url, DBusGProxy * browser);

        CURL * handle() const;
        const char * url() const;
        DBusGProxy * browser() const;
        void initialize();
        bool initialized() const;
    };
}

void
openvrml_player_curl_browser_host_delete_curl_stream_data(void * stream_data)
{
    delete static_cast<curl_stream_data *>(stream_data);
}

void openvrml_player_curl_browser_host_realize(GtkWidget * const widget)
{
    g_return_if_fail(widget);
    g_return_if_fail(OPENVRML_PLAYER_IS_CURL_BROWSER_HOST(widget));

    GtkWidgetClass * klass =
        GTK_WIDGET_CLASS(
            g_type_class_peek(OPENVRML_PLAYER_TYPE_CURL_BROWSER_HOST));
    GtkWidgetClass * parent_class =
        GTK_WIDGET_CLASS(g_type_class_peek_parent(klass));
    parent_class->realize(widget);

    OpenvrmlPlayerCurlBrowserHost * const browser_host =
        OPENVRML_PLAYER_CURL_BROWSER_HOST(widget);
    OpenvrmlPlayerCurlBrowserHostClass * const browser_host_class =
        OPENVRML_PLAYER_CURL_BROWSER_HOST_CLASS(klass);

    GError * error = 0;
    scope_guard error_guard = make_guard(g_error_free, boost::ref(error));

    g_assert(browser_host->priv->path);
    browser_host->priv->browser = get_browser(browser_host_class->connection,
                                              browser_host_class->host_name,
                                              browser_host->priv->path,
                                              0,
                                              &error);
    if (!browser_host->priv->browser) {
        g_critical("Browser creation failed: %s", error->message);
        return;
    }

    guint64 plug_id = 0;
    if (!org_openvrml_Browser_get_id(browser_host->priv->browser, &plug_id,
                                     &error)) {
        g_critical("Call to org.openvrml.Browser.GetId failed: %s",
                   error->message);
        return;
    }

    gtk_socket_add_id(GTK_SOCKET(widget), GdkNativeWindow(plug_id));

    browser_host->priv->multi_handle = curl_multi_init();
    g_return_if_fail(browser_host->priv->multi_handle);
    scope_guard multi_handle_guard =
        make_guard(curl_multi_cleanup, browser_host->priv->multi_handle);

    try {
        browser_host->priv->curl_source =
            curl_source_new(browser_host->priv->multi_handle);
        scope_guard curl_source_guard =
            make_guard(g_source_unref, browser_host->priv->curl_source);

        g_source_set_callback(
            browser_host->priv->curl_source,
            openvrml_player_curl_browser_host_curl_source_callback,
            browser_host,
            0);
        guint source_id = g_source_attach(browser_host->priv->curl_source, 0);
        scope_guard source_attach_guard =
            make_guard(g_source_remove, source_id);

        browser_host->priv->stream_data =
            g_hash_table_new_full(
                g_direct_hash,
                g_direct_equal,
                0,
                openvrml_player_curl_browser_host_delete_curl_stream_data);
        g_return_if_fail(browser_host->priv->stream_data);

        source_attach_guard.dismiss();
        curl_source_guard.dismiss();
    } catch (std::bad_alloc & ex) {
        g_critical("%s", ex.what());
    }

    multi_handle_guard.dismiss();
    error_guard.dismiss();
}

void openvrml_player_curl_browser_host_unrealize(GtkWidget * const widget)
{
    g_return_if_fail(widget);
    g_return_if_fail(OPENVRML_PLAYER_IS_CURL_BROWSER_HOST(widget));

    OpenvrmlPlayerCurlBrowserHost * const browser_host =
        OPENVRML_PLAYER_CURL_BROWSER_HOST(widget);
    g_hash_table_unref(browser_host->priv->stream_data);
    g_source_unref(browser_host->priv->curl_source);
    curl_multi_cleanup(browser_host->priv->multi_handle);
    g_object_unref(browser_host->priv->browser);

    g_signal_emit(widget, signals[shutdown_id], 0);

    GtkWidgetClass * klass =
        GTK_WIDGET_CLASS(
            g_type_class_peek(OPENVRML_PLAYER_TYPE_CURL_BROWSER_HOST));
    GtkWidgetClass * parent_class =
        GTK_WIDGET_CLASS(g_type_class_peek_parent(klass));
    parent_class->unrealize(widget);
}

void openvrml_player_curl_browser_host_finalize(GObject * const obj)
{
    g_return_if_fail(obj);
    g_return_if_fail(OPENVRML_PLAYER_IS_CURL_BROWSER_HOST(obj));
    OpenvrmlPlayerCurlBrowserHost * const browser_host =
        OPENVRML_PLAYER_CURL_BROWSER_HOST(obj);
    g_free(browser_host->priv->path);
}

void
openvrml_player_curl_browser_host_load_url(
    OpenvrmlPlayerCurlBrowserHost * host,
    const char * url)
{
    g_assert(host);
    g_assert(OPENVRML_PLAYER_IS_CURL_BROWSER_HOST(host));
    g_assert(host->priv->browser);
    g_assert(DBUS_IS_G_PROXY(host->priv->browser));
    const char * urls[] = { url, 0 };
    dbus_g_proxy_call_no_reply(host->priv->browser,
                               "LoadUrl",
                               G_TYPE_STRV, &urls[0],
                               G_TYPE_STRV, 0,
                               G_TYPE_INVALID);
}

# define CURL_BROWSER_HOST_CURL_EASY_RETURN_VAL_IF_ERROR(code, val)     \
    if G_LIKELY((code) == CURLE_OK) {} else {                           \
        g_return_if_fail_warning(G_LOG_DOMAIN,                          \
                                 __PRETTY_FUNCTION__,                   \
                                 curl_easy_strerror(code));             \
        return (val);                                                   \
    }

# define CURL_BROWSER_HOST_CURL_MULTI_RETURN_IF_ERROR(code)     \
    if G_LIKELY((code) == CURLM_OK) {} else {                   \
        g_return_if_fail_warning(G_LOG_DOMAIN,                  \
                                 __PRETTY_FUNCTION__,           \
                                 curl_multi_strerror(code));    \
        return;                                                 \
    }

# define CURL_BROWSER_HOST_CURL_MULTI_RETURN_VAL_IF_ERROR(code, val)    \
    if G_LIKELY((code) == CURLM_OK) {} else {                           \
        g_return_if_fail_warning(G_LOG_DOMAIN,                          \
                                 __PRETTY_FUNCTION__,                   \
                                 curl_multi_strerror(code));            \
        return (val);                                                   \
    }

extern "C" {
    G_GNUC_INTERNAL
    size_t openvrml_player_curl_browser_host_curl_write(void * ptr,
                                                        size_t size,
                                                        size_t nmemb,
                                                        void * stream);
    G_GNUC_INTERNAL
    gboolean
    openvrml_player_curl_browser_host_curl_perform_init_callback(gpointer data);
}

namespace {

    typedef std::list<GPollFD> poll_fds_t;

    struct CURLSource {
        GSource source;
        CURLM * multi_handle;
        size_t outstanding_handles;
        fd_set read_fds;
        fd_set write_fds;
        fd_set exc_fds;
        int max_fd;
        poll_fds_t * poll_fds;
    };
}

gboolean
openvrml_player_curl_browser_host_curl_perform_init_callback(
    const gpointer data)
{
    CURLM * const multi_handle = static_cast<CURLM *>(data);

    int running_handles;
    CURLMcode perform_result;
    do {
        perform_result = curl_multi_perform(multi_handle, &running_handles);
    } while (perform_result == CURLM_CALL_MULTI_PERFORM);

    return false;
}

/**
 * @todo We need to do something with the GError here.
 */
int
openvrml_player_curl_browser_host_get_url(
    OpenvrmlPlayerCurlBrowserHost * const host,
    const char * const url,
    GError ** /* error */)
{
    CURL * const handle = curl_easy_init();
    try {
        curl_stream_data * const stream_data =
            new curl_stream_data(handle, url, host->priv->browser);
        scope_guard stream_data_guard =
            make_guard(
                openvrml_player_curl_browser_host_delete_curl_stream_data,
                stream_data);
        g_hash_table_insert(host->priv->stream_data, handle, stream_data);
        stream_data_guard.dismiss();

        CURLcode setopt_result;
        setopt_result = curl_easy_setopt(handle,
                                         CURLOPT_FAILONERROR, true);
        CURL_BROWSER_HOST_CURL_EASY_RETURN_VAL_IF_ERROR(setopt_result, false);
        setopt_result = curl_easy_setopt(handle,
                                         CURLOPT_ENCODING, "");
        CURL_BROWSER_HOST_CURL_EASY_RETURN_VAL_IF_ERROR(setopt_result, false);
        setopt_result = curl_easy_setopt(handle,
                                         CURLOPT_URL, stream_data->url());
        CURL_BROWSER_HOST_CURL_EASY_RETURN_VAL_IF_ERROR(setopt_result, false);

        setopt_result =
            curl_easy_setopt(handle,
                             CURLOPT_WRITEFUNCTION,
                             openvrml_player_curl_browser_host_curl_write);
        CURL_BROWSER_HOST_CURL_EASY_RETURN_VAL_IF_ERROR(setopt_result, false);

        setopt_result = curl_easy_setopt(handle,
                                         CURLOPT_WRITEDATA, stream_data);
        CURL_BROWSER_HOST_CURL_EASY_RETURN_VAL_IF_ERROR(setopt_result, false);

        const CURLMcode add_handle_result =
            curl_multi_add_handle(host->priv->multi_handle, handle);
        CURL_BROWSER_HOST_CURL_MULTI_RETURN_VAL_IF_ERROR(add_handle_result,
                                                         false);

        CURLSource * curl_source =
            static_cast<CURLSource *>(
                static_cast<void *>(host->priv->curl_source));
        ++curl_source->outstanding_handles;

        //
        // If there are no pending transfers, we need to get things started
        // with a call to curl_multi_perform.  However, calling
        // curl_multi_perform will result in a D-Bus call to NewStream in the
        // hosted process.  We don't want that to happen until the remote call
        // to the current function has completed; so, we push it into an idle
        // callback.
        //
        if (g_hash_table_size(host->priv->stream_data) == 1) {
            g_idle_add(
                openvrml_player_curl_browser_host_curl_perform_init_callback,
                host->priv->multi_handle);
        }
    } catch (std::bad_alloc & ex) {
        return 1;
    }
    return 0;
}

/**
 * @todo Implement me.
 */
void
openvrml_player_curl_browser_host_set_world_url(
    OpenvrmlPlayerCurlBrowserHost * /* host */,
    const char * /* url */,
    GError ** /* error */)
{}

extern "C" {
    G_GNUC_INTERNAL
    gboolean openvrml_player_curl_browser_host_curl_prepare(GSource * source,
                                                            gint * timeout);
    G_GNUC_INTERNAL
    gboolean openvrml_player_curl_browser_host_curl_check(GSource * source);
    G_GNUC_INTERNAL
    gboolean openvrml_player_curl_browser_host_curl_dispatch(GSource * source,
                                             GSourceFunc callback,
                                             gpointer user_data);
    G_GNUC_INTERNAL
    void openvrml_player_curl_browser_host_curl_finalize(GSource * source);
}

namespace {

    GSource * curl_source_new(CURLM * const multi_handle)
        throw (std::bad_alloc)
    {
        g_return_val_if_fail(multi_handle, 0);

        static GSourceFuncs curl_source_funcs = {
            openvrml_player_curl_browser_host_curl_prepare,
            openvrml_player_curl_browser_host_curl_check,
            openvrml_player_curl_browser_host_curl_dispatch,
            openvrml_player_curl_browser_host_curl_finalize,
            GSourceFunc(0),
            GSourceDummyMarshal(0)
        };

        CURLSource * const source =
            static_cast<CURLSource *>(
                static_cast<void *>(
                    g_source_new(&curl_source_funcs, sizeof (CURLSource))));

        source->multi_handle        = multi_handle;
        source->outstanding_handles = 0;
        FD_ZERO(&source->read_fds);
        FD_ZERO(&source->write_fds);
        FD_ZERO(&source->exc_fds);
        source->max_fd              = -1;
        source->poll_fds            = new poll_fds_t;

        return static_cast<GSource *>(static_cast<void *>(source));
    }

    G_GNUC_INTERNAL void reset_fds(CURLSource & curl_source)
    {
        FD_ZERO(&curl_source.read_fds);
        FD_ZERO(&curl_source.write_fds);
        FD_ZERO(&curl_source.exc_fds);
        curl_source.max_fd = -1;

        const CURLMcode result = curl_multi_fdset(curl_source.multi_handle,
                                                  &curl_source.read_fds,
                                                  &curl_source.write_fds,
                                                  &curl_source.exc_fds,
                                                  &curl_source.max_fd);
        CURL_BROWSER_HOST_CURL_MULTI_RETURN_IF_ERROR(result);

        if (curl_source.max_fd >= 0) {
            //
            // Resize the list of fds to be polled; initialize any new
            // entries.
            //
            const poll_fds_t::size_type prev_size =
                curl_source.poll_fds->size();
            curl_source.poll_fds->resize(curl_source.max_fd + 1);
            if (curl_source.poll_fds->size() > prev_size) {
                poll_fds_t::iterator pos = curl_source.poll_fds->begin();
                std::advance(pos, prev_size);
                for (gint fd = prev_size; pos != curl_source.poll_fds->end();
                     ++pos, ++fd) {
                    pos->fd      = fd;
                    pos->events  = 0;
                    pos->revents = 0;
                }
            }
        }

        for (poll_fds_t::iterator pos = curl_source.poll_fds->begin();
             pos != curl_source.poll_fds->end();
             ++pos) {
            gushort events = 0;
            if (FD_ISSET(pos->fd, &curl_source.read_fds)) {
                events |= G_IO_IN | G_IO_HUP | G_IO_ERR;
            }
            if (FD_ISSET(pos->fd, &curl_source.write_fds)) {
                events |= G_IO_OUT | G_IO_ERR;
            }
            if (FD_ISSET(pos->fd, &curl_source.exc_fds)) {
                events |= G_IO_ERR;
            }

            //
            // No change.
            //
            if (events == pos->events) { continue; }

            //
            // Changed; but already in the list of fds to poll.  Just update
            // the event flags.
            //
            if (events && pos->events) {
                pos->events = events;
                continue;
            }

            pos->events = events;

            if (events) {
                g_source_add_poll(&curl_source.source, &(*pos));
            } else {
                g_source_remove_poll(&curl_source.source, &(*pos));
                pos->revents = 0;
            }
        }
    }
}

gboolean openvrml_player_curl_browser_host_curl_prepare(GSource * const source,
                                      gint * const timeout_)
{
    CURLSource * const curl_source =
        static_cast<CURLSource *>(static_cast<void *>(source));

    reset_fds(*curl_source);

    long t;
    *timeout_ = (curl_multi_timeout(curl_source->multi_handle, &t) == CURLM_OK)
              ? gint(t)
              : -1;
    return *timeout_ != -1;
}

gboolean openvrml_player_curl_browser_host_curl_check(GSource * const source)
{
    CURLSource * const curl_source =
        static_cast<CURLSource *>(static_cast<void *>(source));

    FD_ZERO(&curl_source->read_fds);
    FD_ZERO(&curl_source->write_fds);
    FD_ZERO(&curl_source->exc_fds);

    bool events_pending = false;
    for (poll_fds_t::const_iterator pos = curl_source->poll_fds->begin();
         pos != curl_source->poll_fds->end();
         ++pos) {
        gushort revents = pos->revents;
        if (revents == 0) { continue; }

        events_pending = true;

        if (revents & (G_IO_IN | G_IO_PRI)) {
            FD_SET(pos->fd, &curl_source->read_fds);
        }
        if (revents & G_IO_OUT) {
            FD_SET(pos->fd, &curl_source->write_fds);
        }
        if (revents & (G_IO_ERR | G_IO_HUP)) {
            FD_SET(pos->fd, &curl_source->exc_fds);
        }
    }

    const bool ready_for_dispatch =
        events_pending || curl_source->outstanding_handles > 0;
    return ready_for_dispatch;
}

gboolean
openvrml_player_curl_browser_host_curl_dispatch(GSource * const source,
                                                const GSourceFunc callback,
                                                const gpointer user_data)
{
    CURLSource * const curl_source =
        static_cast<CURLSource *>(static_cast<void *>(source));
    int running_handles;
    CURLMcode perform_result;
    do {
        perform_result = curl_multi_perform(curl_source->multi_handle,
                                            &running_handles);
    } while (perform_result == CURLM_CALL_MULTI_PERFORM);

    if (running_handles == 0) { reset_fds(*curl_source); }

    if (callback) { (*callback)(user_data); }

    return true;
}

void openvrml_player_curl_browser_host_curl_finalize(GSource * const source)
{
    CURLSource * const curl_source =
        static_cast<CURLSource *>(static_cast<void *>(source));
    reset_fds(*curl_source);
    delete curl_source->poll_fds;
}

gboolean
openvrml_player_curl_browser_host_curl_source_callback(const gpointer data)
{
    OpenvrmlPlayerCurlBrowserHost * const browser_host =
        OPENVRML_PLAYER_CURL_BROWSER_HOST(data);
    CURLSource * const curl_source =
        static_cast<CURLSource *>(
            static_cast<void *>(browser_host->priv->curl_source));

    CURLMsg * msg;
    int msgs_in_queue;
    while ((msg = curl_multi_info_read(curl_source->multi_handle,
                                       &msgs_in_queue))) {
        if (msg->msg == CURLMSG_DONE) {
            curl_stream_data * stream_data =
                static_cast<curl_stream_data *>(
                    g_hash_table_lookup(browser_host->priv->stream_data,
                                        msg->easy_handle));
            g_assert(stream_data);
            //
            // If the stream data was never initialized, we need to call
            // NewStream before calling DestroyStream below.
            //
            if (!stream_data->initialized()) {
                const char * type = 0;
                CURLcode result = curl_easy_getinfo(msg->easy_handle,
                                                    CURLINFO_CONTENT_TYPE,
                                                    &type);
                if (!type) { type = "application/octet-stream"; }
                const char * url = 0;
                result = curl_easy_getinfo(msg->easy_handle,
                                           CURLINFO_EFFECTIVE_URL,
                                           &url);
                dbus_g_proxy_call_no_reply(
                    browser_host->priv->browser,
                    "NewStream",
                    G_TYPE_UINT64, reinterpret_cast<guint64>(msg->easy_handle),
                    G_TYPE_STRING, type,
                    G_TYPE_STRING, url,
                    G_TYPE_INVALID);
            }

            dbus_g_proxy_call_no_reply(
                browser_host->priv->browser,
                "DestroyStream",
                G_TYPE_UINT64, reinterpret_cast<guint64>(msg->easy_handle),
                G_TYPE_INVALID);

            g_hash_table_remove(browser_host->priv->stream_data,
                                msg->easy_handle);
            g_assert(curl_source->outstanding_handles > 0);
            --curl_source->outstanding_handles;

            //
            // Note that the call to curl_multi_remove_handle invalidates the
            // data pointed to by msg.  Save the easy_handle in a temporary
            // variable so we can use it with curl_easy_cleanup below.
            //
            CURL * const easy_handle = msg->easy_handle;
            const CURLMcode multi_remove_result =
                curl_multi_remove_handle(curl_source->multi_handle,
                                         easy_handle);
            CURL_BROWSER_HOST_CURL_MULTI_RETURN_VAL_IF_ERROR(
                multi_remove_result,
                false);
            //
            // If we get an error from curl_multi_remove_handle, the cleanup
            // won't happen; but that's probably safer than trying to go ahead
            // and do the cleanup in that case.
            //
            curl_easy_cleanup(easy_handle);
        }
    }
    return true;
}

size_t openvrml_player_curl_browser_host_curl_write(void * ptr,
                                                    const size_t size,
                                                    const size_t nmemb,
                                                    void * const stream)
{
    curl_stream_data & stream_data = *static_cast<curl_stream_data *>(stream);

    if (!stream_data.initialized()) {
        using boost::ref;

        const char * type = 0;
        CURLcode getinfo_result = curl_easy_getinfo(stream_data.handle(),
                                                    CURLINFO_CONTENT_TYPE,
                                                    &type);
        CURL_BROWSER_HOST_CURL_EASY_RETURN_VAL_IF_ERROR(getinfo_result, 0);

        GnomeVFSFileInfo * info = 0;
        scope_guard info_guard = make_guard(gnome_vfs_file_info_unref,
                                            ref(info));
        if (!type) {
            info = gnome_vfs_file_info_new();
            GnomeVFSResult get_file_info_result =
                gnome_vfs_get_file_info(stream_data.url(),
                                        info,
                                        GNOME_VFS_FILE_INFO_GET_MIME_TYPE);
            if (get_file_info_result != GNOME_VFS_OK) {
                g_critical("%s",
                           gnome_vfs_result_to_string(get_file_info_result));
            }
            type = gnome_vfs_file_info_get_mime_type(info);
        } else {
            info_guard.dismiss();
        }

        GError * error = 0;
        scope_guard error_guard = make_guard(g_error_free, boost::ref(error));
        gboolean new_stream_result =
            org_openvrml_Browser_new_stream(
                stream_data.browser(),
                guint64(stream_data.handle()),
                (type ? type : "application/octet-stream"),
                stream_data.url(),
                &error);
        if (!new_stream_result) {
            g_critical("Call to org.openvrml.Browser.NewStream failed: %s",
                       error->message);
            return 0;
        }

        stream_data.initialize();

        error_guard.dismiss();
    }

    //
    // We need to loop here just in case
    // (size * nmemb) > DBUS_MAXIMUM_ARRAY_LENGTH.
    //
    size_t bytes_written = 0;
    while (bytes_written < nmemb) {
        const size_t bytes_remaining = nmemb - bytes_written;
        const size_t bytes_to_write =
            std::min(size_t(DBUS_MAXIMUM_ARRAY_LENGTH),
                     bytes_remaining);

        GArray array = {};
        array.data = static_cast<char *>(ptr);
        array.len = bytes_to_write;

        dbus_g_proxy_call_no_reply(stream_data.browser(),
                                   "Write",
                                   G_TYPE_UINT64, guint64(stream_data.handle()),
                                   DBUS_TYPE_G_UCHAR_ARRAY, &array,
                                   G_TYPE_INVALID);
        g_assert(size == 1);
        ptr = static_cast<char *>(ptr) + bytes_to_write;
        bytes_written += bytes_to_write;
    }

    return size * nmemb;
}

namespace {

    DBusGProxy * get_browser(DBusGConnection * const connection,
                             const char * const host_name,
                             const char * const host_path,
                             const guint64 host_id,
                             GError ** const error)
        throw ()
    {
        DBusGProxy * browser_factory =
            dbus_g_proxy_new_for_name(connection,
                                      "org.openvrml.BrowserControl",
                                      "/org/openvrml/BrowserFactory",
                                      "org.openvrml.BrowserFactory");
        g_return_val_if_fail(browser_factory, 0);
        scope_guard browser_factory_guard =
            make_guard(g_object_unref, G_OBJECT(browser_factory));
        boost::ignore_unused_variable_warning(browser_factory_guard);

        char * browser_path = 0;
        if (!org_openvrml_BrowserFactory_create_control(
                browser_factory,
                host_name,
                host_path,
                host_id,
                false,
                &browser_path,
                error)) {
            return 0;
        }

        DBusGProxy * browser =
            dbus_g_proxy_new_for_name(connection,
                                      "org.openvrml.BrowserControl",
                                      browser_path,
                                      "org.openvrml.Browser");
        return browser;
    }

    curl_stream_data::curl_stream_data(CURL * const handle,
                                       const char * const url,
                                       DBusGProxy * const browser):
        handle_(handle),
        url_(url, url + strlen(url) + 1), // Get the trailing null.
        browser_(browser),
        initialized_(false)
    {
        g_assert(handle);
        g_assert(url);
        g_assert(browser);
    }

    CURL * curl_stream_data::handle() const
    {
        return this->handle_;
    }

    const char * curl_stream_data::url() const
    {
        return &this->url_.front();
    }

    DBusGProxy * curl_stream_data::browser() const
    {
        return this->browser_;
    }

    void curl_stream_data::initialize()
    {
        this->initialized_ = true;
    }

    bool curl_stream_data::initialized() const
    {
        return this->initialized_;
    }
}
