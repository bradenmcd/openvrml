/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
 *
 * OpenVRML XEmbed Control
 *
 * Copyright 2004, 2005, 2006, 2007, 2008  Braden N. McDaniel
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this library; if not, see <http://www.gnu.org/licenses/>.
 */

# ifndef OPENVRML_XEMBED_BROWSER_H
#   define OPENVRML_XEMBED_BROWSER_H

#   include <gtk/gtkplug.h>
#   include <dbus/dbus-glib.h>
#   include "streamclient.h"

G_BEGIN_DECLS

#   define OPENVRML_XEMBED_ERROR openvrml_xembed_error_quark()

GQuark openvrml_xembed_error_quark(void);

typedef enum {
    OPENVRML_XEMBED_ERROR_FAILED,
    OPENVRML_XEMBED_ERROR_NO_MEMORY,
    OPENVRML_XEMBED_ERROR_UNKNOWN_STREAM
} OpenvrmlXembedError;

#   define OPENVRML_XEMBED_TYPE_BROWSER            (openvrml_xembed_browser_get_type ())
#   define OPENVRML_XEMBED_BROWSER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), OPENVRML_XEMBED_TYPE_BROWSER, OpenvrmlXembedBrowser))
#   define OPENVRML_XEMBED_BROWSER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), OPENVRML_XEMBED_TYPE_BROWSER, OpenvrmlXembedBrowserClass))
#   define OPENVRML_XEMBED_IS_BROWSER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OPENVRML_XEMBED_TYPE_BROWSER))
#   define OPENVRML_XEMBED_IS_BROWSER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), OPENVRML_XEMBED_TYPE_BROWSER))
#   define OPENVRML_XEMBED_BROWSER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), OPENVRML_XEMBED_TYPE_BROWSER, OpenvrmlXembedBrowserClass))

typedef struct OpenvrmlXembedBrowser_ OpenvrmlXembedBrowser;
typedef struct OpenvrmlXembedBrowserPrivate_ OpenvrmlXembedBrowserPrivate;
typedef struct OpenvrmlXembedBrowserClass_ OpenvrmlXembedBrowserClass;

struct OpenvrmlXembedBrowser_ {
    GObject parent;
    OpenvrmlXembedBrowserPrivate * priv;
};

struct OpenvrmlXembedBrowserClass_ {
    GObjectClass parent;
};

OpenvrmlXembedBrowser *
openvrml_xembed_browser_new(DBusGProxy * host_proxy,
                            gboolean expect_initial_stream,
                            GMainContext * gtk_thread_context,
                            GdkNativeWindow socket_id);

GType openvrml_xembed_browser_get_type(void) G_GNUC_CONST;

guint64 openvrml_xembed_browser_get_id(OpenvrmlXembedBrowser * browser);
gboolean
openvrml_xembed_browser_new_stream(OpenvrmlXembedStreamClient * browser,
                                   guint64 stream_id,
                                   const char * type,
                                   const char * url,
                                   GError ** error);
gboolean
openvrml_xembed_browser_destroy_stream(OpenvrmlXembedStreamClient * browser,
                                       guint64 stream_id,
                                       GError ** error);
gboolean openvrml_xembed_browser_write(OpenvrmlXembedStreamClient * browser,
                                       guint64 stream_id,
                                       const GArray * data,
                                       GError ** error);
gboolean openvrml_xembed_browser_load_url(OpenvrmlXembedBrowser * browser,
                                          const char ** url,
                                          const char ** parameter,
                                          GError ** error);
gchar * openvrml_xembed_browser_get_world_url(OpenvrmlXembedBrowser * browser,
                                              GError ** error);

G_GNUC_INTERNAL
GObject *
openvrml_xembed_browser_constructor(
    GType type,
    guint n_construct_properties,
    GObjectConstructParam * construct_properties);


#   define OPENVRML_XEMBED_TYPE_BROWSER_PLUG            (openvrml_xembed_browser_plug_get_type ())
#   define OPENVRML_XEMBED_BROWSER_PLUG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), OPENVRML_XEMBED_TYPE_BROWSER_PLUG, OpenvrmlXembedBrowserPlug))
#   define OPENVRML_XEMBED_BROWSER_PLUG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), OPENVRML_XEMBED_TYPE_BROWSER_PLUG, OpenvrmlXembedBrowserPlugClass))
#   define OPENVRML_XEMBED_IS_BROWSER_PLUG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OPENVRML_XEMBED_TYPE_BROWSER_PLUG))
#   define OPENVRML_XEMBED_IS_BROWSER_PLUG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), OPENVRML_XEMBED_TYPE_BROWSER_PLUG))
#   define OPENVRML_XEMBED_BROWSER_PLUG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), OPENVRML_XEMBED_TYPE_BROWSER_PLUG, OpenvrmlXembedBrowserPlugClass))

typedef struct OpenvrmlXembedBrowserPlug_ OpenvrmlXembedBrowserPlug;
typedef struct OpenvrmlXembedBrowserPlugPrivate_ OpenvrmlXembedBrowserPlugPrivate;
typedef struct OpenvrmlXembedBrowserPlugClass_ OpenvrmlXembedBrowserPlugClass;

struct OpenvrmlXembedBrowserPlug_ {
    GtkPlug parent;
    OpenvrmlXembedBrowserPlugPrivate * priv;
};

struct OpenvrmlXembedBrowserPlugClass_ {
    GtkPlugClass parent;
};

GtkWidget * openvrml_xembed_browser_plug_new(OpenvrmlXembedBrowser * browser,
                                             GdkNativeWindow socket_id);
GType openvrml_xembed_browser_plug_get_type(void) G_GNUC_CONST;

G_END_DECLS

# endif /* ifndef OPENVRML_XEMBED_BROWSER_H */
