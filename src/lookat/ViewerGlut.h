// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#ifndef VIEWERGLUT_H
#define VIEWERGLUT_H

#include <OpenVRML/GL/viewer.h>

/**
 * GLUT version of OpenGL class for display of VRML models.
 */
class ViewerGlut : public OpenVRML::GL::viewer {
    int d_window;
    bool d_timerPending;

public:
    ViewerGlut(OpenVRML::browser & browser);
    virtual ~ViewerGlut();

    // Public so glut callbacks can access
    void timerUpdate();

protected:
    //
    // Window system specific methods
    //
    virtual void post_redraw();
    virtual void set_cursor(cursor_style c);
    virtual void swap_buffers();
    virtual void set_timer(double);
};

#endif // VIEWERGLUT_H
