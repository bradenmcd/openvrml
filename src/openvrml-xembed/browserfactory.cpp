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
# include <boost/multi_index/detail/scope_guard.hpp>
# include <boost/ref.hpp>
# include <dbus/dbus-glib-bindings.h>
# include <cstdlib>

# include "browserfactory.h"
# include "browser-factory-server-glue.h"
# include "browser.h"

using namespace boost::multi_index::detail; // for scope_guard

G_DEFINE_TYPE(OpenvrmlXembedBrowserFactory,
              openvrml_xembed_browser_factory,
              G_TYPE_OBJECT);

void
openvrml_xembed_browser_factory_init(
    OpenvrmlXembedBrowserFactory * control_factory)
{
    GError * error = 0;
    scope_guard error_guard = make_guard(g_error_free, boost::ref(error));
    OpenvrmlXembedBrowserFactoryClass * control_factory_class =
        OPENVRML_XEMBED_BROWSER_FACTORY_GET_CLASS(control_factory);

    dbus_g_connection_register_g_object(control_factory_class->connection,
                                        "/org/openvrml/BrowserFactory",
                                        G_OBJECT(control_factory));

    DBusGProxy * driver_proxy =
        dbus_g_proxy_new_for_name(control_factory_class->connection,
                                  DBUS_SERVICE_DBUS,
                                  DBUS_PATH_DBUS,
                                  DBUS_INTERFACE_DBUS);
    scope_guard driver_proxy_guard = make_guard(g_object_unref, driver_proxy);
    boost::ignore_unused_variable_warning(driver_proxy_guard);

    guint request_ret;
    if (!org_freedesktop_DBus_request_name(driver_proxy,
                                           "org.openvrml.BrowserControl",
                                           0, &request_ret,
                                           &error)) {
        g_critical("Request for name \"org.openvrml.BrowserControl\" "
                   "failed: %s", error->message);
        return;
    }

    control_factory->controls =
        g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_object_unref);

    error_guard.dismiss();
}

void
openvrml_xembed_browser_factory_class_init(
    OpenvrmlXembedBrowserFactoryClass * klass)
{
    GError * error = 0;
    scope_guard error_guard = make_guard(g_error_free, boost::ref(error));

    klass->connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
    if (!klass->connection) {
        g_critical("Failed to get session bus: %s", error->message);
        return;
    }

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
        gchar * control_obj_path;
    };
}

extern "C"
G_GNUC_INTERNAL
void delete_on_host_shutdown_data(gpointer data, GClosure * /* closure */)
{
    delete static_cast<on_host_shutdown_data *>(data);
}

char *
openvrml_xembed_browser_factory_create_control(
    OpenvrmlXembedBrowserFactory * const control_factory,
    const char * const host_name,
    const char * const host_obj_path,
    const guint host_id,
    const gboolean expect_initial_stream,
    GError ** const error)
{
    DBusGProxy * host =
        dbus_g_proxy_new_for_name_owner(
            OPENVRML_XEMBED_BROWSER_FACTORY_GET_CLASS(
                control_factory)->connection,
            host_name,
            host_obj_path,
            "org.openvrml.BrowserHost",
            error);
    if (!host) { return 0; }

    OpenvrmlXembedBrowser * const browser =
        OPENVRML_XEMBED_BROWSER(
            openvrml_xembed_browser_new(host,
                                        host_id,
                                        expect_initial_stream));
    g_return_val_if_fail(browser, 0);

    static size_t control_count = 0;
    char * control_obj_path = g_strdup_printf("/org/openvrml/Browser/%lu",
                                              control_count++);
    dbus_g_connection_register_g_object(
        OPENVRML_XEMBED_BROWSER_FACTORY_GET_CLASS(control_factory)->connection,
        control_obj_path,
        G_OBJECT(browser));

    on_host_shutdown_data * const data = new on_host_shutdown_data;
    data->factory = control_factory;
    //
    // Note that D-Bus frees the return value; so we need to strdup the one
    // in the hash map.
    //
    data->control_obj_path = g_strdup(control_obj_path);

    dbus_g_proxy_add_signal(host, "Shutdown", G_TYPE_INVALID);

    dbus_g_proxy_connect_signal(
        host,
        "Shutdown",
        G_CALLBACK(openvrml_xembed_browser_factory_on_host_shutdown_event),
        data,
        delete_on_host_shutdown_data);

    g_hash_table_insert(control_factory->controls,
                        data->control_obj_path, // was strdup'd above
                        g_object_ref(browser));

    //
    // The plug needs to be realized before it gets shown.  Explicitly
    // realizing here seems like the most convenient way to ensure this
    // happens in the correct order.
    //
    gtk_widget_realize(GTK_WIDGET(browser));
    gtk_widget_show_all(GTK_WIDGET(browser));

    return control_obj_path;
}

gboolean
openvrml_xembed_browser_factory_on_host_shutdown_event(DBusGProxy * /* proxy */,
                                                       const gpointer user_data)
{
    on_host_shutdown_data * const data =
        static_cast<on_host_shutdown_data *>(user_data);

    gboolean removed = g_hash_table_remove(data->factory->controls,
                                           data->control_obj_path);
    g_assert(removed);

    return false;
}
