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
# include <glade/glade.h>
# include <glade/glade-build.h>
# include <openvrml-config.h>

# include "curlbrowserhost.h"
# include "filechooserdialog.h"

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

using namespace boost::multi_index::detail; // for scope_guard

extern "C" {
    //
    // GTK+ signal handlers; libglade needs them to have external linkage in
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
    void
    openvrml_player_connect_func(const gchar * handler_name,
                                 GObject * object,
                                 const gchar * signal_name,
                                 const gchar * signal_data,
                                 GObject * connect_object,
                                 gboolean after,
                                 gpointer user_data);

    G_GNUC_INTERNAL
    void openvrml_player_on_browserhost_realize(GtkWidget * widget,
                                                gpointer user_data);
    G_GNUC_INTERNAL
    GtkWidget *
    openvrml_player_build_file_chooser_dialog(GladeXML * xml,
                                              GType widget_type,
                                              GladeWidgetInfo * info);
    G_GNUC_INTERNAL
    GtkWidget *
    openvrml_player_build_curl_browser_host(GladeXML * xml,
                                            GType widget_type,
                                            GladeWidgetInfo * info);
}

namespace {
    const char app_name[] = "OpenVRML Player";
    const char app_id[] = "openvrml-player";

    struct signal_data {
        OpenvrmlPlayerCurlBrowserHost * browser_host;
        GtkWidget * location_entry;
        OpenvrmlPlayerFileChooserDialog * file_chooser;
    };

    G_GNUC_INTERNAL GladeXML * xml_new(GnomeProgram & program);
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

    glade_init();
    glade_register_widget(OPENVRML_PLAYER_TYPE_FILE_CHOOSER_DIALOG,
                          openvrml_player_build_file_chooser_dialog,
                          0,
                          0);
    glade_register_widget(OPENVRML_PLAYER_TYPE_CURL_BROWSER_HOST,
                          openvrml_player_build_curl_browser_host,
                          0,
                          0);

    GladeXML * const xml = xml_new(*program);

    GtkWidget * const app_window = glade_xml_get_widget(xml, "window");

    GtkWidget * const file_chooser_dialog =
        glade_xml_get_widget(xml, "filechooserdialog");

    signal_data data = {};
    data.browser_host =
        OPENVRML_PLAYER_CURL_BROWSER_HOST(
            glade_xml_get_widget(xml, "browserhost"));
    g_assert(OPENVRML_PLAYER_IS_CURL_BROWSER_HOST(data.browser_host));
    data.location_entry =
        GTK_WIDGET(glade_xml_get_widget(xml, "locationentry"));
    data.file_chooser =
        OPENVRML_PLAYER_FILE_CHOOSER_DIALOG(
            glade_xml_get_widget(xml, "filechooserdialog"));

    glade_xml_signal_autoconnect_full(xml, openvrml_player_connect_func, &data);

    gtk_window_set_transient_for(GTK_WINDOW(data.file_chooser),
                                 GTK_WINDOW(app_window));

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
}

namespace {

    GladeXML * xml_new(GnomeProgram & program)
    {
        using std::string;

        const gchar * const gladedir = g_getenv("OPENVRML_PLAYER_GLADEDIR");
        string glade_file_str;
        const gchar * glade_file = 0;
        if (gladedir) {
            glade_file_str = string(gladedir) + "/openvrml-player.glade";
            glade_file = glade_file_str.c_str();
        }

        if (!glade_file) {
            static const gboolean only_if_exists = true;
            glade_file =
                gnome_program_locate_file(&program,
                                          GNOME_FILE_DOMAIN_APP_DATADIR,
                                          "/glade/openvrml-player.glade",
                                          only_if_exists,
                                          0);
        }

        g_return_val_if_fail(glade_file, 0);

        return glade_xml_new(glade_file, 0, 0);
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

GtkWidget *
openvrml_player_build_file_chooser_dialog(GladeXML * /* xml */,
                                          GType /* widget_type */,
                                          GladeWidgetInfo * /* info */)
{
    GtkWidget * widget =
        GTK_WIDGET(g_object_new(OPENVRML_PLAYER_TYPE_FILE_CHOOSER_DIALOG, 0));
    return widget;
}

GtkWidget *
openvrml_player_build_curl_browser_host(GladeXML * /* xml */,
                                        GType /* widget_type */,
                                        GladeWidgetInfo * /* info */)
{
    GtkWidget * widget =
        GTK_WIDGET(g_object_new(OPENVRML_PLAYER_TYPE_CURL_BROWSER_HOST, 0));
    return widget;
}

void
openvrml_player_connect_func(const gchar * const handler_name,
                             GObject * const object,
                             const gchar * const signal_name,
                             const gchar * const signal_data,
                             GObject * const connect_object,
                             const gboolean after,
                             const gpointer user_data)
{
    static GModule * const this_module = g_module_open(0, GModuleFlags(0));
    gpointer handler;
    if (g_module_symbol(this_module, handler_name, &handler)) {
        if (connect_object) {
            g_signal_connect_object(
                object,
                signal_name,
                G_CALLBACK(handler),
                connect_object,
                GConnectFlags((after ? G_CONNECT_AFTER : 0)
                              | G_CONNECT_SWAPPED));
        } else {
            if (after) {
                g_signal_connect_after(object, signal_name, G_CALLBACK(handler),
                                       user_data);
            } else {
                g_signal_connect(object, signal_name, G_CALLBACK(handler),
                                 user_data);
            }
        }
    } else {
        g_warning("signal handler \"%s\" not found", handler_name);
    }
}
