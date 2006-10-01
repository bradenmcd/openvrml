/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; -*-
 *
 * OpenVRML Mozilla plug-in
 * Copyright 2004, 2005, 2006  Braden N. McDaniel
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

# ifndef GTK_VRML_BROWSER_H
#   define GTK_VRML_BROWSER_H

#   include <gtk/gtkdrawingarea.h>
#   include <gtk/gtkgl.h>

G_BEGIN_DECLS

#   define GTK_TYPE_VRML_BROWSER            (gtk_vrml_browser_get_type ())
#   define GTK_VRML_BROWSER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_VRML_BROWSER, GtkVrmlBrowser))
#   define GTK_VRML_BROWSER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_VRML_BROWSER, GtkVrmlBrowserClass))
#   define GTK_IS_VRML_BROWSER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_VRML_BROWSER))
#   define GTK_IS_VRML_BROWSER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_VRML_BROWSER))
#   define GTK_VRML_BROWSER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_VRML_BROWSER, GtkVrmlBrowserClass))

typedef struct GtkVrmlBrowser_      GtkVrmlBrowser;
typedef struct GtkVrmlBrowserClass_ GtkVrmlBrowserClass;

struct GtkVrmlBrowser_ {
    GtkDrawingArea drawing_area;
    void *viewer;
};

struct GtkVrmlBrowserClass_ {
    GtkDrawingAreaClass parent_class;
};


GType gtk_vrml_browser_get_type(void) G_GNUC_CONST;
GtkWidget *gtk_vrml_browser_new(GIOChannel *request_channel);
void gtk_vrml_browser_load_url(GtkVrmlBrowser *vrml_browser,
                               const gchar **url,
                               const char **parameter);

G_END_DECLS

#   ifdef __cplusplus

namespace openvrml {
    class resource_istream;
}

//
// Since OpenVRML has a C++ interface, exposing this to C is Hard.
//
void gtk_vrml_browser_set_world(GtkVrmlBrowser * vrml_browser,
                                openvrml::resource_istream & in);
#   endif

# endif /* ifndef GTK_VRML_BROWSER_H */
