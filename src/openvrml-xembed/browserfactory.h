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

# ifndef OPENVRML_XEMBED_BROWSER_FACTORY_H
#   define OPENVRML_XEMBED_BROWSER_FACTORY_H

#   include <dbus/dbus-glib.h>

#   define OPENVRML_XEMBED_TYPE_BROWSER_FACTORY (openvrml_xembed_browser_factory_get_type())
#   define OPENVRML_XEMBED_BROWSER_FACTORY(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), OPENVRML_XEMBED_TYPE_BROWSER_FACTORY, OpenvrmlXembedBrowserFactory))
#   define OPENVRML_XEMBED_BROWSER_FACTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), OPENVRML_XEMBED_TYPE_BROWSER_FACTORY, OpenvrmlXembedBrowserFactoryClass))
#   define OPENVRML_XEMBED_IS_BROWSER_FACTORY(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), OPENVRML_XEMBED_TYPE_BROWSER_FACTORY))
#   define OPENVRML_XEMBED_IS_BROWSER_FACTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), OPENVRML_XEMBED_TYPE_BROWSER_FACTORY))
#   define OPENVRML_XEMBED_BROWSER_FACTORY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), OPENVRML_XEMBED_TYPE_BROWSER_FACTORY, OpenvrmlXembedBrowserFactoryClass))

G_BEGIN_DECLS

typedef struct OpenvrmlXembedBrowserFactory_ OpenvrmlXembedBrowserFactory;
typedef struct OpenvrmlXembedBrowserFactoryPrivate_ OpenvrmlXembedBrowserFactoryPrivate;
typedef struct OpenvrmlXembedBrowserFactoryClass_ OpenvrmlXembedBrowserFactoryClass;

GType openvrml_xembed_browser_factory_get_type();

struct OpenvrmlXembedBrowserFactory_ {
    GObject parent;
    OpenvrmlXembedBrowserFactoryPrivate * priv;
};

struct OpenvrmlXembedBrowserFactoryClass_ {
    GObjectClass parent;
    DBusGConnection * connection;
};

gboolean
openvrml_xembed_browser_factory_create_control(
    OpenvrmlXembedBrowserFactory * factory,
    const char * host_name,
    const char * host_obj_path,
    guint host_id,
    gboolean expect_initial_stream,
    DBusGMethodInvocation * context);

G_END_DECLS

# endif // ifndef OPENVRML_XEMBED_BROWSER_FACTORY_H
