// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML XEmbed Control
//
// Copyright 2008, 2010  Braden McDaniel
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

# include <boost/intrusive_ptr.hpp>
# include <boost/scope_exit.hpp>
# include <boost/ref.hpp>
# include <gtk/gtk.h>
# include <dbus/dbus-glib-bindings.h>
# include <dbus/dbus-glib-lowlevel.h>
# include <map>
# include <memory>
# include <string>
# include <cstring>
# include <cstdlib>

# include "browserfactory.h"
# include "browser-factory-server-glue.h"
# include "browser.h"

G_DEFINE_TYPE(OpenvrmlXembedBrowserFactory,
              openvrml_xembed_browser_factory,
              G_TYPE_OBJECT)

extern "C" {
    G_GNUC_INTERNAL
    void openvrml_xembed_browser_factory_set_property(GObject * object,
                                                      guint property_id,
                                                      const GValue * value,
                                                      GParamSpec * pspec);
    G_GNUC_INTERNAL
    void openvrml_xembed_browser_factory_get_property(GObject * object,
                                                      guint property_id,
                                                      GValue * value,
                                                      GParamSpec * pspec);
}

namespace {
    G_GNUC_INTERNAL void intrusive_ptr_add_ref(GObject * const obj)
    {
        g_object_ref(obj);
    }

    G_GNUC_INTERNAL void intrusive_ptr_release(GObject * const obj)
    {
        g_object_unref(obj);
    }
}

//
// hosts maps the unique bus identifiers of hosts to a controls_map_t; a
// controls_map_t maps an object path to an OpenvrmlXembedBrowser instance.
//

typedef std::map<std::string, boost::intrusive_ptr<GObject> > controls_map_t;
typedef std::map<std::string, controls_map_t > hosts_map_t;

struct OpenvrmlXembedBrowserFactoryPrivate_ {
    DBusGConnection * connection;
    GMainContext * main_thread_context;
    hosts_map_t * hosts;
};

# define OPENVRML_XEMBED_BROWSER_FACTORY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), OPENVRML_XEMBED_TYPE_BROWSER_FACTORY, OpenvrmlXembedBrowserFactoryPrivate))

extern "C"
G_GNUC_INTERNAL
GObject *
openvrml_xembed_browser_factory_constructor(
    const GType type,
    const guint n_construct_properties,
    GObjectConstructParam * const construct_properties)
{
    GObject * obj;
    {
        OpenvrmlXembedBrowserFactoryClass * const klass =
            OPENVRML_XEMBED_BROWSER_FACTORY_CLASS(
                g_type_class_peek(OPENVRML_XEMBED_TYPE_BROWSER_FACTORY));
        GObjectClass * const parent_class =
            G_OBJECT_CLASS(g_type_class_peek_parent(klass));
        obj = parent_class->constructor(type,
                                        n_construct_properties,
                                        construct_properties);
    }

    try {
        using std::auto_ptr;
        OpenvrmlXembedBrowserFactory * const browser_factory =
            OPENVRML_XEMBED_BROWSER_FACTORY(obj);
        auto_ptr<hosts_map_t> hosts(new hosts_map_t);
        browser_factory->priv->hosts = hosts.release();

        g_assert(browser_factory->priv->connection);

        dbus_g_connection_register_g_object(browser_factory->priv->connection,
                                            "/org/openvrml/BrowserFactory",
                                            G_OBJECT(browser_factory));

        DBusGProxy * driver_proxy =
            dbus_g_proxy_new_for_name(browser_factory->priv->connection,
                                      DBUS_SERVICE_DBUS,
                                      DBUS_PATH_DBUS,
                                      DBUS_INTERFACE_DBUS);
        BOOST_SCOPE_EXIT((driver_proxy)) {
            g_object_unref(driver_proxy);
        } BOOST_SCOPE_EXIT_END

        GError * error = 0;
        BOOST_SCOPE_EXIT((&error)) {
            if (error) { g_error_free(error); }
        } BOOST_SCOPE_EXIT_END
        guint request_ret;
        if (!org_freedesktop_DBus_request_name(
                driver_proxy,
                "org.openvrml.BrowserControl",
                0, &request_ret,
                &error)) {
            g_critical("Request for name \"org.openvrml.BrowserControl\" "
                       "failed: %s", error->message);
            return 0;
        }
    } catch (std::bad_alloc &) {
        return 0;
    }

    return obj;
}

extern "C"
G_GNUC_INTERNAL
void
openvrml_xembed_browser_factory_finalize(GObject * const obj)
{
    OpenvrmlXembedBrowserFactory * browser_factory =
        OPENVRML_XEMBED_BROWSER_FACTORY(obj);
    delete browser_factory->priv->hosts;

    OpenvrmlXembedBrowserFactoryClass * const klass =
        OPENVRML_XEMBED_BROWSER_FACTORY_CLASS(
            g_type_class_peek(OPENVRML_XEMBED_TYPE_BROWSER_FACTORY));
    GObjectClass * const parent_class =
        G_OBJECT_CLASS(g_type_class_peek_parent(klass));
    parent_class->finalize(obj);
}


void
openvrml_xembed_browser_factory_init(
    OpenvrmlXembedBrowserFactory * const browser_factory)
{
    browser_factory->priv =
        OPENVRML_XEMBED_BROWSER_FACTORY_GET_PRIVATE(browser_factory);
}

namespace {
    enum browser_factory_property_id {
        connection_id = 1,
        main_thread_context_id
    };
}

void
openvrml_xembed_browser_factory_class_init(
    OpenvrmlXembedBrowserFactoryClass * klass)
{
    GObjectClass * const gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->constructor  = openvrml_xembed_browser_factory_constructor;
    gobject_class->finalize     = openvrml_xembed_browser_factory_finalize;
    gobject_class->set_property = openvrml_xembed_browser_factory_set_property;
    gobject_class->get_property = openvrml_xembed_browser_factory_get_property;

    g_type_class_add_private(G_OBJECT_CLASS(klass),
                             sizeof (OpenvrmlXembedBrowserFactoryPrivate));

    GParamSpec * pspec =
        g_param_spec_boxed("connection",
                           "Session bus connection",
                           "Set the session bus",
                           DBUS_TYPE_G_CONNECTION,
                           GParamFlags(G_PARAM_CONSTRUCT_ONLY
                                       | G_PARAM_READWRITE));
    g_object_class_install_property(gobject_class, connection_id, pspec);

    pspec = g_param_spec_pointer("main-thread-context",
                                 "Main thread context",
                                 "Default GMainContext for the main thread",
                                 GParamFlags(G_PARAM_CONSTRUCT_ONLY
                                             | G_PARAM_READWRITE));
    g_object_class_install_property(
        gobject_class, main_thread_context_id, pspec);

    dbus_g_object_type_install_info(
        OPENVRML_XEMBED_TYPE_BROWSER_FACTORY,
        &dbus_glib_openvrml_xembed_browser_factory_object_info);
}

void openvrml_xembed_browser_factory_set_property(GObject * const object,
                                                  const guint property_id,
                                                  const GValue * const value,
                                                  GParamSpec * const pspec)
{
    OpenvrmlXembedBrowserFactory * const browser_factory =
        OPENVRML_XEMBED_BROWSER_FACTORY(object);

    switch (property_id) {
    case connection_id:
        if (browser_factory->priv->connection) {
            dbus_g_connection_unref(browser_factory->priv->connection);
        }
        browser_factory->priv->connection =
            static_cast<DBusGConnection *>(g_value_get_boxed(value)); 
        break;
    case main_thread_context_id:
        browser_factory->priv->main_thread_context =
            static_cast<GMainContext *>(g_value_get_pointer(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

void openvrml_xembed_browser_factory_get_property(GObject * const object,
                                                  const guint property_id,
                                                  GValue * const value,
                                                  GParamSpec * const pspec)
{
    OpenvrmlXembedBrowserFactory * const browser_factory =
        OPENVRML_XEMBED_BROWSER_FACTORY(object);

    switch (property_id) {
    case connection_id:
        g_value_set_boxed(value, browser_factory->priv->connection);
        break;
    case main_thread_context_id:
        g_value_set_pointer(value, browser_factory->priv->main_thread_context);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

extern "C"
G_GNUC_INTERNAL
gboolean
openvrml_xembed_browser_factory_on_host_shutdown_event(DBusGProxy * proxy,
                                                       gpointer user_data);

namespace {
    struct on_host_shutdown_data {
        OpenvrmlXembedBrowserFactory * factory;
        gchar * host_name;
        gchar * control_obj_path;
    };
}

extern "C"
G_GNUC_INTERNAL
void delete_on_host_shutdown_data(gpointer data, GClosure * /* closure */)
{
    on_host_shutdown_data * const d =
        static_cast<on_host_shutdown_data *>(data);
    g_free(d->control_obj_path);
    g_free(d->host_name);
    delete d;
}

gboolean
openvrml_xembed_browser_factory_create_control(
    OpenvrmlXembedBrowserFactory * const control_factory,
    const char * const host_name,
    const char * const host_obj_path,
    const guint64 host_id,
    const gboolean expect_initial_stream,
    DBusGMethodInvocation * const context)
{
    GError * error = 0;
    BOOST_SCOPE_EXIT((&error)) {
        if (error) { g_error_free(error); }
    } BOOST_SCOPE_EXIT_END

    DBusGProxy * host =
        dbus_g_proxy_new_for_name_owner(
            control_factory->priv->connection,
            host_name,
            host_obj_path,
            "org.openvrml.BrowserHost",
            &error);
    if (!host) {
        dbus_g_method_return_error(context, error);
        return false;
    }

    OpenvrmlXembedBrowser * const browser =
        openvrml_xembed_browser_new(host,
                                    expect_initial_stream,
                                    control_factory->priv->main_thread_context,
                                    host_name,
                                    host_id);

    if (!browser) {
        error = g_error_new(OPENVRML_XEMBED_ERROR,
                            OPENVRML_XEMBED_ERROR_NO_MEMORY,
                            "out of memory");
        dbus_g_method_return_error(context, error);
        return false;
    }

    static size_t control_count = 0;
    char * control_obj_path = g_strdup_printf("/org/openvrml/Browser/%lu",
                                              control_count++);
    dbus_g_connection_register_g_object(
        control_factory->priv->connection,
        control_obj_path,
        G_OBJECT(browser));

    char * const sender = dbus_g_method_get_sender(context);
    BOOST_SCOPE_EXIT((sender)) {
        g_free(sender);
    } BOOST_SCOPE_EXIT_END
    bool succeeded;
    try {
        using std::make_pair;
        static const bool add_ref = false;
        succeeded =
            (*control_factory->priv->hosts)[sender].insert(
                make_pair(control_obj_path,
                          boost::intrusive_ptr<GObject>(G_OBJECT(browser),
                                                        add_ref)))
            .second;
        g_debug("inserted reference to %s", sender);
    } catch (std::bad_alloc & ex) {
        error = g_error_new(OPENVRML_XEMBED_ERROR,
                            OPENVRML_XEMBED_ERROR_NO_MEMORY,
                            "out of memory");
        dbus_g_method_return_error(context, error);
        return false;
    }

    g_return_val_if_fail(succeeded, false);

    on_host_shutdown_data * const data = new on_host_shutdown_data;
    data->factory = control_factory;
    //
    // Note that D-Bus frees the return value; so we need to strdup one to
    // keep.
    //
    data->host_name        = g_strdup(sender);
    data->control_obj_path = g_strdup(control_obj_path);

    dbus_g_proxy_add_signal(host, "Shutdown", G_TYPE_INVALID);

    dbus_g_proxy_connect_signal(
        host,
        "Shutdown",
        G_CALLBACK(openvrml_xembed_browser_factory_on_host_shutdown_event),
        data,
        delete_on_host_shutdown_data);

    dbus_g_method_return(context, control_obj_path);

    return true;
}

guint
openvrml_xembed_browser_factory_remove_hosts_for_owner(
    OpenvrmlXembedBrowserFactory * const browser_factory,
    const gchar * const id)
{
    return browser_factory->priv->hosts->erase(id);
}

gboolean
openvrml_xembed_browser_factory_has_hosts(
    OpenvrmlXembedBrowserFactory * const browser_factory)
{
    return !browser_factory->priv->hosts->empty();
}

gboolean
openvrml_xembed_browser_factory_on_host_shutdown_event(DBusGProxy * /* proxy */,
                                                       const gpointer user_data)
{
    on_host_shutdown_data * const data =
        static_cast<on_host_shutdown_data *>(user_data);

    const hosts_map_t::iterator pos =
        data->factory->priv->hosts->find(data->host_name);
    g_return_val_if_fail(pos != data->factory->priv->hosts->end(), false);

    const size_t erased = pos->second.erase(data->control_obj_path);
    g_return_val_if_fail(erased > 0, false);

    return false;
}
