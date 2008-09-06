// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// Copyright 2006, 2007, 2008  Braden McDaniel
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

# include <curl/curl.h>
# include <string>
# include <vector>
# include <boost/concept_check.hpp>
# include <boost/lexical_cast.hpp>
# include <boost/multi_index/detail/scope_guard.hpp>
# include <boost/ref.hpp>
# include <dbus/dbus-glib.h>
# include <libgnomevfs/gnome-vfs.h>
# include <libgnomeui/libgnomeui.h>
# include <gtk/gtkbuilder.h>
# include <openvrml-config.h>

# include "curlbrowserhost.h"
# include "filechooserdialog.h"

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

using namespace boost::multi_index::detail; // for scope_guard

extern "C" {
    //
    // GTK+ signal handlers; GtkBuilder needs them to have external linkage in
    // order to connect them.
    //
    OPENVRML_API void openvrml_player_on_about_activated(GtkWindow * window);
    OPENVRML_API void
    openvrml_player_on_file_open_activated(GtkAction * action,
                                           gpointer user_data);
    OPENVRML_API void
    openvrml_player_on_locationentry_activated(GtkEntry * entry,
                                               gpointer user_data);
    OPENVRML_API void
    openvrml_player_on_filechooserdialog_response(GtkDialog * dialog,
                                                  gint arg1,
                                                  gpointer user_data);
    OPENVRML_API void openvrml_player_quit();

    G_GNUC_INTERNAL
    void openvrml_player_on_browserhost_realize(GtkWidget * widget,
                                                gpointer user_data);
}

namespace {
    const char app_name[] = "OpenVRML Player";
    const char app_id[] = "openvrml-player";

    struct signal_data {
        OpenvrmlPlayerCurlBrowserHost * browser_host;
        GtkWidget * location_entry;
        OpenvrmlPlayerFileChooserDialog * file_chooser;
    };

    G_GNUC_INTERNAL GtkBuilder * builder_new(GnomeProgram & program,
                                             GError ** error);
}

int main(int argc, char * argv[])
{
    using std::string;
    using std::vector;
    using boost::lexical_cast;
    using boost::ref;

    g_set_application_name(app_name);

    if (!gnome_vfs_init()) {
        g_critical("GnomeVFS initialization failed");
        return EXIT_FAILURE;
    }

    if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
        g_critical("libcurl initialization failed");
        return EXIT_FAILURE;
    }
    scope_guard curl_global_guard = make_guard(curl_global_cleanup);
    boost::ignore_unused_variable_warning(curl_global_guard);

    gchar ** remaining_args = 0;
    GOptionEntry option_entries[] = {
        {
            G_OPTION_REMAINING,
            '\0',
            0,
            G_OPTION_ARG_FILENAME_ARRAY,
            &remaining_args,
            "a URI for a VRML or X3D world",
            "[URI]"
        },
        { 0, '\0', 0, G_OPTION_ARG_NONE, 0, 0, 0 }
    };

    GOptionContext * const option_context =
        g_option_context_new("- render VRML/X3D worlds");
    static const gchar * const translation_domain = 0;
    g_option_context_add_main_entries(option_context,
                                      option_entries,
                                      translation_domain);

    GnomeProgram * const program =
        gnome_program_init(
            app_id,
            PACKAGE_VERSION,
            LIBGNOMEUI_MODULE,
            argc,
            argv,
            GNOME_PARAM_GOPTION_CONTEXT, option_context,
            GNOME_PARAM_APP_DATADIR, OPENVRML_PLAYER_PKGDATADIR_,
            GNOME_PARAM_NONE);

    GError * error = 0;
    scope_guard error_guard = make_guard(g_error_free, ref(error));
    GtkBuilder * const builder = builder_new(*program, &error);
    if (!builder) {
        g_critical("Failed to create UI builder: %s", error->message);
        return EXIT_FAILURE;
    }

    GtkWidget * const app_window =
        GTK_WIDGET(gtk_builder_get_object(builder, "window"));

    GtkWidget * const file_chooser_dialog =
        GTK_WIDGET(gtk_builder_get_object(builder, "filechooserdialog"));

    signal_data data = {};
    data.browser_host =
        OPENVRML_PLAYER_CURL_BROWSER_HOST(
            gtk_builder_get_object(builder, "browserhost"));
    g_assert(OPENVRML_PLAYER_IS_CURL_BROWSER_HOST(data.browser_host));
    data.location_entry =
        GTK_WIDGET(gtk_builder_get_object(builder, "locationentry"));
    data.file_chooser =
        OPENVRML_PLAYER_FILE_CHOOSER_DIALOG(
            gtk_builder_get_object(builder, "filechooserdialog"));

    gtk_window_set_transient_for(GTK_WINDOW(data.file_chooser),
                                 GTK_WINDOW(app_window));

    gtk_builder_connect_signals(builder, &data);

    g_signal_connect(file_chooser_dialog,
                     "response",
                     G_CALLBACK(openvrml_player_on_filechooserdialog_response),
                     data.location_entry);

    if (remaining_args && remaining_args[0]) {
        g_signal_connect(data.browser_host,
                         "realize",
                         G_CALLBACK(openvrml_player_on_browserhost_realize),
                         remaining_args[0]);
    }

    gtk_widget_show_all(app_window);

    gtk_main();

    error_guard.dismiss();
}

namespace {

    GtkBuilder * builder_new(GnomeProgram & program, GError ** error)
    {
        using std::string;

        const gchar * const uidir = g_getenv("OPENVRML_PLAYER_UIDIR");
        string ui_file_str;
        const gchar * ui_file = 0;
        if (uidir) {
            ui_file_str = string(uidir) + "/openvrml-player.ui";
            ui_file = ui_file_str.c_str();
        }

        if (!ui_file) {
            static const gboolean only_if_exists = true;
            ui_file =
                gnome_program_locate_file(&program,
                                          GNOME_FILE_DOMAIN_APP_DATADIR,
                                          "/ui/openvrml-player.ui",
                                          only_if_exists,
                                          0);
        }

        g_return_val_if_fail(ui_file, 0);

        GtkBuilder * builder = gtk_builder_new();
        if (!gtk_builder_add_from_file(builder, ui_file, error)) {
            return 0;
        }

        return builder;
    }
}

void openvrml_player_on_about_activated(GtkWindow * const parent)
{
    const gchar website[] = "http://openvrml.org";
    const gchar * authors[] = {
        "Braden McDaniel <braden@endoframe.com>",
        0
    };
    const gchar copyright[] = "Copyright 2008";
    const gchar license[] =
        "This program is free software; you can redistribute it and/or modify "
        "it under the terms of the GNU General Public License as published by "
        "the Free Software Foundation; either version 3 of the License, or "
        "(at your option) any later version.\n\n"

        "This program is distributed in the hope that it will be useful, but "
        "WITHOUT ANY WARRANTY; without even the implied warranty of "
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU "
        "General Public License for more details.\n\n"

        "You should have received a copy of the GNU General Public License "
        "along with this program; if not, see <http://www.gnu.org/licenses/>.";

    gtk_show_about_dialog(parent,
                          "name", app_name,
                          "version", PACKAGE_VERSION,
                          "website", website,
                          "authors", authors,
                          "copyright", copyright,
                          "license", license,
                          "wrap-license", true,
                          NULL);
}

void
openvrml_player_on_file_open_activated(GtkAction * /* action */,
                                       const gpointer user_data)
{
    const signal_data * const data = static_cast<signal_data *>(user_data);
    g_assert(OPENVRML_PLAYER_IS_CURL_BROWSER_HOST(data->browser_host));

    const gint response = gtk_dialog_run(GTK_DIALOG(data->file_chooser));
    if (response == GTK_RESPONSE_ACCEPT) {
        gchar * uri =
            gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(data->file_chooser));
        g_return_if_fail(uri);
        scope_guard uri_guard = make_guard(g_free, uri);
        boost::ignore_unused_variable_warning(uri_guard);
        openvrml_player_curl_browser_host_load_url(data->browser_host, uri);
    }

    gtk_widget_hide(GTK_WIDGET(data->file_chooser));
}

void openvrml_player_on_locationentry_activated(GtkEntry * const entry,
                                                const gpointer user_data)
{
    const signal_data * const data = static_cast<signal_data *>(user_data);
    g_assert(OPENVRML_PLAYER_IS_CURL_BROWSER_HOST(data->browser_host));
    const gchar * const uri = gtk_entry_get_text(entry);
    openvrml_player_curl_browser_host_load_url(data->browser_host, uri);
}

void openvrml_player_on_filechooserdialog_response(GtkDialog * const dialog,
                                                   const gint arg1,
                                                   const gpointer user_data)
{
    if (arg1 == GTK_RESPONSE_ACCEPT) {
        GtkEntry * const location_entry = static_cast<GtkEntry *>(user_data);
        gchar * uri = 0;
        uri = gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(dialog));
        g_return_if_fail(uri);
        scope_guard uri_guard = make_guard(g_free, uri);
        boost::ignore_unused_variable_warning(uri_guard);
        gtk_entry_set_text(location_entry, uri);
    }
}

void openvrml_player_quit()
{
    gtk_main_quit();
}

void openvrml_player_on_browserhost_realize(GtkWidget * const widget,
                                            const gpointer user_data)
{
    openvrml_player_curl_browser_host_load_url(
        OPENVRML_PLAYER_CURL_BROWSER_HOST(widget),
        static_cast<const char *>(user_data));
}
