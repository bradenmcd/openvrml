// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML XEmbed Control
//
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

# include <boost/concept_check.hpp>
# include <boost/intrusive_ptr.hpp>
# include <boost/multi_index/detail/scope_guard.hpp>
# include <boost/ref.hpp>
# include <gtk/gtk.h>
# include <dbus/dbus-glib-bindings.h>
# include <dbus/dbus-glib-lowlevel.h>
# include <map>
# include <memory>
# include <cstring>
# include <cstdlib>

# include "browserfactory.h"
# include "browser-factory-server-glue.h"
# include "browser.h"

using namespace boost::multi_index::detail; // for scope_guard

# define OPENVRML_XEMBED_ERROR openvrml_xembed_error_quark()
GQuark openvrml_xembed_error_quark()
{
    return g_quark_from_static_string("openvrml-xembed-error-quark");
}

enum OpenvrmlXembedError {
    OPENVRML_XEMBED_ERROR_FAILED,
    OPENVRML_XEMBED_ERROR_NO_MEMORY
};

G_DEFINE_TYPE(OpenvrmlXembedBrowserFactory,
              openvrml_xembed_browser_factory,
              G_TYPE_OBJECT);

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
    DBusGProxy * driver_proxy;
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
    g_object_unref(browser_factory->priv->driver_proxy);

    OpenvrmlXembedBrowserFactoryClass * const klass =
        OPENVRML_XEMBED_BROWSER_FACTORY_CLASS(
            g_type_class_peek(OPENVRML_XEMBED_TYPE_BROWSER_FACTORY));
    GObjectClass * const parent_class =
        G_OBJECT_CLASS(g_type_class_peek_parent(klass));
    parent_class->finalize(obj);
}


extern "C"
G_GNUC_INTERNAL
gboolean
openvrml_xembed_browser_factory_name_owner_changed(DBusGProxy * proxy,
                                                   const gchar * name,
                                                   const gchar * old_owner,
                                                   const gchar * new_owner,
                                                   gpointer user_data);
void
openvrml_xembed_browser_factory_init(
    OpenvrmlXembedBrowserFactory * const browser_factory)
{
    browser_factory->priv =
        OPENVRML_XEMBED_BROWSER_FACTORY_GET_PRIVATE(browser_factory);

    GError * error = 0;
    scope_guard error_guard = make_guard(g_error_free, boost::ref(error));
    OpenvrmlXembedBrowserFactoryClass * browser_factory_class =
        OPENVRML_XEMBED_BROWSER_FACTORY_GET_CLASS(browser_factory);

    dbus_g_connection_register_g_object(browser_factory_class->connection,
                                        "/org/openvrml/BrowserFactory",
                                        G_OBJECT(browser_factory));

    browser_factory->priv->driver_proxy =
        dbus_g_proxy_new_for_name(browser_factory_class->connection,
                                  DBUS_SERVICE_DBUS,
                                  DBUS_PATH_DBUS,
                                  DBUS_INTERFACE_DBUS);

    guint request_ret;
    if (!org_freedesktop_DBus_request_name(browser_factory->priv->driver_proxy,
                                           "org.openvrml.BrowserControl",
                                           0, &request_ret,
                                           &error)) {
        g_critical("Request for name \"org.openvrml.BrowserControl\" "
                   "failed: %s", error->message);
        return;
    }

    dbus_g_proxy_add_signal(browser_factory->priv->driver_proxy,
                            "NameOwnerChanged",
                            G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                            G_TYPE_INVALID);

    dbus_g_proxy_connect_signal(
        browser_factory->priv->driver_proxy,
        "NameOwnerChanged",
        G_CALLBACK(openvrml_xembed_browser_factory_name_owner_changed),
        browser_factory,
        0);

    error_guard.dismiss();
}

void
openvrml_xembed_browser_factory_class_init(
    OpenvrmlXembedBrowserFactoryClass * klass)
{
    G_OBJECT_CLASS(klass)->constructor =
        openvrml_xembed_browser_factory_constructor;
    G_OBJECT_CLASS(klass)->finalize =
        openvrml_xembed_browser_factory_finalize;

    GError * error = 0;
    scope_guard error_guard = make_guard(g_error_free, boost::ref(error));

    klass->connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
    if (!klass->connection) {
        g_critical("Failed to get session bus: %s", error->message);
        return;
    }


    g_type_class_add_private(G_OBJECT_CLASS(klass),
                             sizeof (OpenvrmlXembedBrowserFactoryPrivate));

    dbus_g_object_type_install_info(
        OPENVRML_XEMBED_TYPE_BROWSER_FACTORY,
        &dbus_glib_openvrml_xembed_browser_factory_object_info);

    error_guard.dismiss();
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
    scope_guard error_guard = make_guard(g_error_free, boost::ref(error));

    DBusGProxy * host =
        dbus_g_proxy_new_for_name_owner(
            OPENVRML_XEMBED_BROWSER_FACTORY_GET_CLASS(
                control_factory)->connection,
            host_name,
            host_obj_path,
            "org.openvrml.BrowserHost",
            &error);
    if (!host) {
        dbus_g_method_return_error(context, error);
        return false;
    }

    OpenvrmlXembedBrowser * const browser =
        OPENVRML_XEMBED_BROWSER(
            openvrml_xembed_browser_new(host,
                                        host_id,
                                        expect_initial_stream));
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
        OPENVRML_XEMBED_BROWSER_FACTORY_GET_CLASS(control_factory)->connection,
        control_obj_path,
        G_OBJECT(browser));

    char * const sender = dbus_g_method_get_sender(context);
    scope_guard sender_guard = make_guard(g_free, sender);
    boost::ignore_unused_variable_warning(sender_guard);
    bool succeeded;
    try {
        using std::make_pair;
        succeeded =
            (*control_factory->priv->hosts)[sender].insert(
                make_pair(control_obj_path,
                          boost::intrusive_ptr<GObject>(G_OBJECT(browser))))
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

    //
    // The plug needs to be realized before it gets shown.  Explicitly
    // realizing here seems like the most convenient way to ensure this
    // happens in the correct order.
    //
    gtk_widget_realize(GTK_WIDGET(browser));
    gtk_widget_show_all(GTK_WIDGET(browser));

    dbus_g_method_return(context, control_obj_path);

    error_guard.dismiss();

    return true;
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

gboolean
openvrml_xembed_browser_factory_name_owner_changed(
    DBusGProxy * /* proxy */,
    const gchar * /* name */,
    const gchar * const old_owner,
    const gchar * const new_owner,
    const gpointer user_data)
{
    OpenvrmlXembedBrowserFactory * const browser_factory =
        OPENVRML_XEMBED_BROWSER_FACTORY(user_data);

    //
    // If there's no new owner, the existing owner is simply leaving (i.e.,
    // terminating.  Clean up resources associated with that host.  If that
    // was the last host, quit.
    //
    size_t erased = 0;
    if (strlen(new_owner) == 0) {
        erased = browser_factory->priv->hosts->erase(old_owner);
        g_debug("erased references to %s", old_owner);
    }

    if (erased > 0 && browser_factory->priv->hosts->empty()) {
        gtk_main_quit();
    }

    return false;
}
