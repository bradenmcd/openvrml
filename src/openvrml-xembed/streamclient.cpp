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

# include "streamclient.h"

GType openvrml_xembed_stream_client_get_type()
{
    static GType type = 0;
    if (G_UNLIKELY(type == 0)) {
        type =
            g_type_register_static_simple(
                G_TYPE_INTERFACE,
                "OpenvrmlXembedStreamClient",
                sizeof (OpenvrmlXembedStreamClientInterface),
                0, // class_init
                0, // instance_size
                0, // instance_init
                G_TYPE_FLAG_ABSTRACT);
    }
    return type;
}

gboolean
openvrml_xembed_stream_client_new_stream(
    OpenvrmlXembedStreamClient * const stream_client,
    const guint64 stream_id,
    const char * const type,
    const char * const url,
    GError ** const error)
{
    return OPENVRML_XEMBED_STREAM_CLIENT_GET_INTERFACE(stream_client)
        ->new_stream(stream_client, stream_id, type, url, error);
}

gboolean
openvrml_xembed_stream_client_destroy_stream(
    OpenvrmlXembedStreamClient * const stream_client,
    const guint64 stream_id,
    GError ** const error)
{
    return OPENVRML_XEMBED_STREAM_CLIENT_GET_INTERFACE(stream_client)
        ->destroy_stream(stream_client, stream_id, error);
}

gboolean
openvrml_xembed_stream_client_write(
    OpenvrmlXembedStreamClient * const stream_client,
    const guint64 stream_id,
    const GArray * const data,
    GError ** const error)
{
    return OPENVRML_XEMBED_STREAM_CLIENT_GET_INTERFACE(stream_client)
        ->write(stream_client, stream_id, data, error);
}
