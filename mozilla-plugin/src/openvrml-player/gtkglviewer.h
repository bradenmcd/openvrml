// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML Mozilla plug-in
// Copyright 2004, 2005, 2006  Braden N. McDaniel
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

# ifndef OPENVRML_PLAYER_GTKGLVIEWER_H
#   define OPENVRML_PLAYER_GTKGLVIEWER_H

#   include <gtk/gtk.h>
#   include <gtk/gtkgl.h>
#   include <openvrml/gl/viewer.h>

namespace openvrml_player {

    class GtkGLViewer : public openvrml::gl::viewer {
        static GdkGLConfig * gl_config;

        GtkWidget * drawing_area;
        guint timer;

    public:
        bool redrawNeeded;

        explicit GtkGLViewer(GtkContainer & container);
        virtual ~GtkGLViewer() throw ();

        void timer_update();

    protected:
        //
        // Implement pure virtual methods from openvrml::gl::viewer.
        //
        virtual void post_redraw();
        virtual void set_cursor(openvrml::gl::viewer::cursor_style);
        virtual void swap_buffers();
        virtual void set_timer(double);
    };
}

# endif // ifndef OPENVRML_PLAYER_GTKGLVIEWER_H
