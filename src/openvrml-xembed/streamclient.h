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

# ifndef OPENVRML_XEMBED_STREAM_CLIENT_H
#   define OPENVRML_XEMBED_STREAM_CLIENT_H

#   include <glib-object.h>

#   define OPENVRML_XEMBED_TYPE_STREAM_CLIENT (openvrml_xembed_stream_client_get_type())
#   define OPENVRML_XEMBED_STREAM_CLIENT(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), OPENVRML_XEMBED_TYPE_STREAM_CLIENT, OpenvrmlXembedStreamClient))
#   define OPENVRML_XEMBED_IS_STREAM_CLIENT(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), OPENVRML_XEMBED_TYPE_STREAM_CLIENT))
#   define OPENVRML_XEMBED_STREAM_CLIENT_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE((inst), OPENVRML_XEMBED_TYPE_STREAM_CLIENT, OpenvrmlXembedStreamClientInterface))

G_BEGIN_DECLS

typedef struct OpenvrmlXembedStreamClient_ OpenvrmlXembedStreamClient;
typedef struct OpenvrmlXembedStreamClientInterface_ OpenvrmlXembedStreamClientInterface;

struct OpenvrmlXembedStreamClientInterface_ {
    GTypeInterface parent;

    gboolean (*new_stream)(OpenvrmlXembedStreamClient * stream_client,
                           guint64 stream_id, const char * type,
                           const char * url, GError ** error);
    gboolean (*destroy_stream)(OpenvrmlXembedStreamClient * stream_client,
                               guint64 stream_id, GError ** error);
    gboolean (*write)(OpenvrmlXembedStreamClient * stream_client,
                      guint64 stream_id, const GArray * data, GError ** error);
};

GType openvrml_xembed_stream_client_get_type(void);

gboolean
openvrml_xembed_stream_client_new_stream(
    OpenvrmlXembedStreamClient * stream_client, guint64 stream_id,
    const char * type, const char * url, GError ** error);

gboolean
openvrml_xembed_stream_client_destroy_stream(
    OpenvrmlXembedStreamClient * stream_client, guint64 stream_id,
    GError ** error);

gboolean
openvrml_xembed_stream_client_write(OpenvrmlXembedStreamClient * stream_client,
                                    guint64 stream_id, const GArray * data,
                                    GError ** error);

G_END_DECLS

# endif // ifndef OPENVRML_XEMBED_STREAM_CLIENT_H
