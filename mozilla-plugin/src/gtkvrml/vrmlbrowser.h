//
// GTK+ OpenVRML widget
// Copyright 2004  Braden N. McDaniel
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

# ifndef GTKVRMLBROWSER_H
#   define GTKVRMLBROWSER_H

#   include <gtk/gtk.h>

#   ifdef __cplusplus
extern "C" {
#   endif

#   define GTK_TYPE_VRML_BROWSER            (gtk_vrml_browser_get_type())
#   define GTK_VRML_BROWSER(obj)            (GTK_CHECK_CAST((obj), GTK_TYPE_VRML_BROWSER, GtkVrmlBrowser))
#   define GTK_VRML_BROWSER_CLASS(klass)    (GTK_CHECK_CLASS_CAST(klass, GTK_TYPE_VRML_BROWSER, GtkVrmlBrowserClass))
#   define GTK_IS_VRML_BROWSER(obj)         (GTK_CHECK_TYPE((obj), GTK_TYPE_VRML_BROWSER))
#   define GTK_IS_VRML_BROWSER_CLASS(klass) (GTK_CHECK_CLASS_TYPE((klass), GTK_TYPE_VRML_BROWSER))
#   define GTK_VRML_BROWSER_GET_CLASS(obj)  (GTK_CHECK_GET_CLASS ((obj), GTK_TYPE_VRML_BROWSER, GtkVrmlBrowserClass))

typedef struct _GtkVrmlBrowser GtkVrmlBrowser;
typedef struct _GtkVrmlBrowserClass GtkVrmlBrowserClass;

struct _GtkVrmlBrowser {
     GtkDrawingArea drawing_area;
     void * browser;
     void * viewer;
};

struct _GtkVrmlBrowserClass {
     GtkDrawingAreaClass  parent_class;
};

GtkType gtk_vrml_browser_get_type();
GtkWidget * gtk_vrml_browser_new();
gboolean gtk_vrml_browser_load_url(GtkVrmlBrowser * vrml_browser,
                                   const char * url);
#   ifdef __cplusplus
} /* extern "C" */
#   endif

# endif // defined GTKVRMLBROWSER_H
