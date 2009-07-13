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
# ifndef OPENVRML_PLAYER_CURL_BROWSER_HOST_H
#   define OPENVRML_PLAYER_CURL_BROWSER_HOST_H

#   include <dbus/dbus-glib.h>
#   include <gtk/gtksocket.h>
#   include <openvrml-config.h>

#   define OPENVRML_PLAYER_TYPE_CURL_BROWSER_HOST (openvrml_player_curl_browser_host_get_type())
#   define OPENVRML_PLAYER_CURL_BROWSER_HOST(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), OPENVRML_PLAYER_TYPE_CURL_BROWSER_HOST, OpenvrmlPlayerCurlBrowserHost))
#   define OPENVRML_PLAYER_CURL_BROWSER_HOST_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), OPENVRML_PLAYER_TYPE_CURL_BROWSER_HOST, OpenvrmlPlayerCurlBrowserHostClass))
#   define OPENVRML_PLAYER_IS_CURL_BROWSER_HOST(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), OPENVRML_PLAYER_TYPE_CURL_BROWSER_HOST))
#   define OPENVRML_PLAYER_IS_CURL_BROWSER_HOST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), OPENVRML_PLAYER_TYPE_CURL_BROWSER_HOST))
#   define OPENVRML_PLAYER_CURL_BROWSER_HOST_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), OPENVRML_PLAYER_TYPE_CURL_BROWSER_HOST, OpenvrmlPlayerCurlBrowserHostClass))

G_BEGIN_DECLS

typedef struct OpenvrmlPlayerCurlBrowserHost_ OpenvrmlPlayerCurlBrowserHost;
typedef struct OpenvrmlPlayerCurlBrowserHostPrivate_ OpenvrmlPlayerCurlBrowserHostPrivate;
typedef struct OpenvrmlPlayerCurlBrowserHostClass_ OpenvrmlPlayerCurlBrowserHostClass;

struct OpenvrmlPlayerCurlBrowserHost_ {
    GtkSocket parent;
    OpenvrmlPlayerCurlBrowserHostPrivate * priv;
};

struct OpenvrmlPlayerCurlBrowserHostClass_ {
    GtkSocketClass parent;
    DBusGConnection * connection;
    gchar * host_name;
};

OPENVRML_API GType openvrml_player_curl_browser_host_get_type();

void
openvrml_player_curl_browser_host_load_url(OpenvrmlPlayerCurlBrowserHost * host,
                                           const char * url);

int
openvrml_player_curl_browser_host_get_url(OpenvrmlPlayerCurlBrowserHost * host,
                                          const char * url, GError ** error);
void
openvrml_player_curl_browser_host_set_world_url(
    OpenvrmlPlayerCurlBrowserHost * host, const char * url, GError ** error);

G_END_DECLS

# endif // ifndef OPENVRML_PLAYER_CURL_BROWSER_HOST_H
