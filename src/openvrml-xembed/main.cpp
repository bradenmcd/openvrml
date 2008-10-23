// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML XEmbed Control
//
// Copyright 2004, 2005, 2006, 2007, 2008  Braden McDaniel
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
# include <boost/lexical_cast.hpp>
# include <boost/multi_index/detail/scope_guard.hpp>
# include <unistd.h>
// Must include before X11 headers.
# include <boost/numeric/conversion/converter.hpp>
# include "browserfactory.h"
# include <gtk/gtkgl.h>
# include <gtk/gtk.h>
# include <boost/concept_check.hpp>
# include <boost/ref.hpp>

# ifdef HAVE_CONFIG_H
#   include "config.h"
# endif

using namespace boost::multi_index::detail; // for scope_guard

namespace {

    const char application_name[] = "OpenVRML XEmbed Control";
}

int main(int argc, char * argv[])
{
    using std::cout;
    using std::endl;
    using boost::ref;

    g_thread_init(0);
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
    scope_guard error_guard = make_guard(g_error_free, ref(error));

    GOptionContext * const context =
        g_option_context_new("- render VRML worlds");
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
        error_guard.dismiss();
        return EXIT_SUCCESS;
    }

    DBusGConnection * bus = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
    if (!bus) {
        g_printerr(error->message);
        return EXIT_FAILURE;
    }

    OpenvrmlXembedBrowserFactory * browser_factory =
        OPENVRML_XEMBED_BROWSER_FACTORY(
            g_object_new(OPENVRML_XEMBED_TYPE_BROWSER_FACTORY, 0));
//    scope_guard browser_factory_guard =
//        make_guard(g_object_unref, G_OBJECT(browser_factory));
//    boost::ignore_unused_variable_warning(browser_factory_guard);

    gdk_threads_enter();
    gtk_main();
    gdk_threads_leave();

    error_guard.dismiss();
}
