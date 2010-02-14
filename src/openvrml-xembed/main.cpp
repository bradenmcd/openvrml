// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML XEmbed Control
//
// Copyright 2004, 2005, 2006, 2007, 2008, 2009, 2010  Braden McDaniel
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

// Must include before X11 headers.
# include <boost/numeric/conversion/converter.hpp>
# include "browserfactory.h"
# include <gtk/gtkgl.h>
# include <gtk/gtk.h>
# include <dbus/dbus-glib-bindings.h>
# include <dbus/dbus-glib-lowlevel.h>
# include <boost/function.hpp>
# include <boost/lexical_cast.hpp>
# include <boost/scope_exit.hpp>
# include <boost/ref.hpp>
# include <boost/scoped_ptr.hpp>
# include <boost/thread.hpp>
# include <iostream>
# include <cstring>
# include <unistd.h>

# ifdef HAVE_CONFIG_H
#   include "config.h"
# endif

extern "C"
G_GNUC_INTERNAL
gboolean
openvrml_xembed_name_owner_changed(DBusGProxy * proxy,
                                   const gchar * name,
                                   const gchar * old_owner,
                                   const gchar * new_owner,
                                   gpointer user_data);

namespace {

    const char application_name[] = "OpenVRML XEmbed Control";

    G_GNUC_INTERNAL
    DBusGConnection * bus_get(GMainContext * const context,
                              const DBusBusType type,
                              GError ** const error)
    {
        DBusError derror;
        dbus_error_init(&derror);
        DBusConnection * const connection = dbus_bus_get(type, &derror);
        if (!connection) {
            dbus_set_g_error(error, &derror);
            dbus_error_free(&derror);
            return 0;
        }
        dbus_connection_setup_with_g_main(connection, context);
        return dbus_connection_get_g_connection(connection);
    }

    struct G_GNUC_INTERNAL name_owner_changed_callback_data {
        GMainLoop * dbus_thread_main;
        OpenvrmlXembedBrowserFactory * browser_factory;
    };

    struct G_GNUC_INTERNAL dbus_thread_loop {
        dbus_thread_loop(GMainContext & main_thread_context,
                         GMainLoop & dbus_thread_main):
            main_thread_context_(main_thread_context),
            dbus_thread_main_(dbus_thread_main)
        {}

        void operator()() const
        {
            GError * error = 0;
            BOOST_SCOPE_EXIT((&error)) {
                if (error) { g_error_free(error); }
            } BOOST_SCOPE_EXIT_END

            DBusGConnection * const connection =
                bus_get(g_main_loop_get_context(&this->dbus_thread_main_),
                        DBUS_BUS_SESSION,
                        &error);
            if (!connection) {
                g_critical("Failed to get session bus: %s", error->message);
                return;
            }
            BOOST_SCOPE_EXIT((connection)) {
                dbus_g_connection_unref(connection);
            } BOOST_SCOPE_EXIT_END

            OpenvrmlXembedBrowserFactory * const browser_factory =
                OPENVRML_XEMBED_BROWSER_FACTORY(
                    g_object_new(
                        OPENVRML_XEMBED_TYPE_BROWSER_FACTORY,
                        "connection", connection,
                        "main-thread-context", &this->main_thread_context_,
                        static_cast<void *>(0)));
            BOOST_SCOPE_EXIT((browser_factory)) {
                g_object_unref(browser_factory);
            } BOOST_SCOPE_EXIT_END

            DBusGProxy * driver_proxy =
                dbus_g_proxy_new_for_name(connection,
                                          DBUS_SERVICE_DBUS,
                                          DBUS_PATH_DBUS,
                                          DBUS_INTERFACE_DBUS);
            BOOST_SCOPE_EXIT((driver_proxy)) {
                g_object_unref(driver_proxy);
            } BOOST_SCOPE_EXIT_END

            dbus_g_proxy_add_signal(driver_proxy,
                                    "NameOwnerChanged",
                                    G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                                    G_TYPE_INVALID);

            name_owner_changed_callback_data cb_data = {};
            cb_data.dbus_thread_main = &this->dbus_thread_main_;
            cb_data.browser_factory = browser_factory;

            dbus_g_proxy_connect_signal(
                driver_proxy,
                "NameOwnerChanged",
                G_CALLBACK(openvrml_xembed_name_owner_changed),
                &cb_data,
                0);

            g_main_loop_run(&this->dbus_thread_main_);
        }

    private:
        GMainContext & main_thread_context_;
        GMainLoop & dbus_thread_main_;
    };
}

int main(int argc, char * argv[])
{
    using std::cout;
    using std::endl;
    using boost::ref;

    g_thread_init(0);
    dbus_g_thread_init();
    gdk_threads_init();

    g_set_application_name(application_name);

    gtk_init(&argc, &argv);
    gtk_gl_init(&argc, &argv);

    gboolean version = false;
    GOptionEntry options[] = {
        {
            "version",
            'v',
            0,
            G_OPTION_ARG_NONE,
            &version,
            "Print the version information and exit",
            0
        },
        { 0, '\0', 0, G_OPTION_ARG_NONE, 0, 0, 0 }
    };

    GError * error = 0;
    BOOST_SCOPE_EXIT((&error)) {
        if (error) { g_error_free(error); }
    } BOOST_SCOPE_EXIT_END

    GOptionContext * const context =
        g_option_context_new("- render VRML/X3D worlds");
    const gchar * const translation_domain = 0;
    g_option_context_add_main_entries(context, options, translation_domain);
    g_option_context_add_group(context, gtk_get_option_group(true));
    gboolean succeeded = g_option_context_parse(context, &argc, &argv, &error);
    if (!succeeded) {
        if (error) { g_critical(error->message); }
        return EXIT_FAILURE;
    }

    if (version) {
        cout << application_name << ' ' << PACKAGE_VERSION << endl;
        return EXIT_SUCCESS;
    }

    GMainContext * const main_context = g_main_context_default();
    g_assert(main_context);

    using boost::function;
    using boost::scoped_ptr;
    using boost::thread;

    GMainContext * dbus_thread_context = g_main_context_new();
    BOOST_SCOPE_EXIT((dbus_thread_context)) {
        g_main_context_unref(dbus_thread_context);
    } BOOST_SCOPE_EXIT_END
    GMainLoop * dbus_thread_main = g_main_loop_new(dbus_thread_context, false);
    BOOST_SCOPE_EXIT((dbus_thread_main)) {
        g_main_loop_unref(dbus_thread_main);
    } BOOST_SCOPE_EXIT_END

    function<void ()> dbus_thread_func = dbus_thread_loop(*main_context,
                                                          *dbus_thread_main);
    const scoped_ptr<thread> dbus_thread(new thread(dbus_thread_func));

    gdk_threads_enter();
    gtk_main();
    gdk_threads_leave();

    //
    // The D-Bus thread's main loop was quit at the same time as Gtk's was
    // quit in openvrml_xembed_name_owner_changed.
    //
    dbus_thread->join();
}

//
// Note that this callback gets called for every name owner change on the
// bus--including those completely unrelated to us.
//
gboolean
openvrml_xembed_name_owner_changed(
    DBusGProxy * /* proxy */,
    const gchar * /* name */,
    const gchar * const old_owner,
    const gchar * const new_owner,
    const gpointer user_data)
{
    const name_owner_changed_callback_data * const data =
        static_cast<name_owner_changed_callback_data *>(user_data);

    //
    // If there's no new owner, the existing owner is simply leaving (i.e.,
    // terminating).  Clean up resources associated with that host.  If that
    // was the last host, quit.
    //
    size_t erased = 0;
    if (strlen(new_owner) == 0) {
        erased =
            openvrml_xembed_browser_factory_remove_hosts_for_owner(
                data->browser_factory,
                old_owner);
        if (erased > 0) { g_debug("erased references to %s", old_owner); }
    }

    const bool browser_factory_has_hosts =
        openvrml_xembed_browser_factory_has_hosts(data->browser_factory);
    if (erased > 0 && !browser_factory_has_hosts) {
        //
        // Quit both the D-Bus thread (where this callback was called from)
        // and the main thread.  The D-Bus thread gets joined in main after
        // Gtk's main loop completes.
        //
        g_main_loop_quit(data->dbus_thread_main);
        gdk_threads_enter();
        gtk_main_quit();
        gdk_threads_leave();
    }

    return false;
}
