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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <iostream>

# if HAVE_APPLE_OPENGL_FRAMEWORK
#   include <GLUT/glut.h>
# else
#   include <GL/glut.h>
# endif

# include "ViewerGlut.h"

using namespace openvrml;

/**
 * @class ViewerGlut
 *
 * GLUT version of OpenGL class for display of VRML models.
 */

extern "C" {
    typedef void (*GlutDisplay)();
    typedef void (*GlutKeyboard)(unsigned char, int, int);
    typedef void (*GlutSpecial)(int, int, int);
    typedef void (*GlutMouse)(int, int, int, int);
    typedef void (*GlutMotion)(int, int);
    typedef void (*GlutPassiveMotion)(int, int);
    typedef void (*GlutReshape)(int, int);
    typedef void (*GlutTimer)(int);
}

// Last mouse button pressed.
static int lastButton = 0;

// Map from GLUT window number to Viewer*
#define MAX_WINDOWS 10

// This could be a list<> if anyone cared...
static ViewerGlut* viewers[MAX_WINDOWS] = { 0 };

static ViewerGlut* getViewer(int w)
{
  return (w <= MAX_WINDOWS) ? viewers[w-1] : 0;
}


//  These callbacks locate the viewer object then call the
//  viewer-specific versions.

extern "C" {

    //  Do redraw callback.

    void display()
    {
        ViewerGlut * viewer = getViewer(glutGetWindow());
        if (viewer) { viewer->redraw(); }
    }

    //  Keyboard button callback

    void keyboard(unsigned char key, int x, int y)
    {
        ViewerGlut * viewer = getViewer(glutGetWindow());

        gl::viewer::event_info e = { gl::viewer::event_key_down, key, x, y };
        if (viewer) { viewer->input(&e); }
    }

    //  More keyboard buttons callback

    void specialKey(int key, int x, int y)
    {
        gl::viewer::event_info e = { gl::viewer::event_key_down, 0, x, y };

        switch (key) {
        case GLUT_KEY_HOME:  e.what = gl::viewer::key_home; break;
        case GLUT_KEY_LEFT:  e.what = gl::viewer::key_left; break;
        case GLUT_KEY_UP:    e.what = gl::viewer::key_up; break;
        case GLUT_KEY_RIGHT: e.what = gl::viewer::key_right; break;
        case GLUT_KEY_DOWN:  e.what = gl::viewer::key_down; break;
        case GLUT_KEY_PAGE_DOWN: e.what = gl::viewer::key_page_down; break;
        case GLUT_KEY_PAGE_UP: e.what = gl::viewer::key_page_up; break;
        default: return;
        }

        ViewerGlut * viewer = getViewer(glutGetWindow());

        if (viewer) { viewer->input(&e); }
    }

    //  Mouse button callback

    void mouse(int button, int state, int x, int y)
    {
        ViewerGlut *viewer = getViewer( glutGetWindow() );

        gl::viewer::event_info e = { gl::viewer::event_mouse_click,
                                     button,
                                     x,
                                     y };
        if (state == GLUT_UP) e.event = gl::viewer::event_mouse_release;
        lastButton = button;
        if (viewer) viewer->input( &e );
    }

    //  Mouse drag motion callback (button down)

    void motion(int x, int y)
    {
        ViewerGlut * viewer = getViewer(glutGetWindow());
        gl::viewer::event_info e = { gl::viewer::event_mouse_drag,
                                     lastButton, x, y };
        if (viewer) { viewer->input(&e); }
    }

    //  Mouse motion callback

    void passiveMotion(int x, int y)
    {
        ViewerGlut *viewer = getViewer( glutGetWindow() );
        gl::viewer::event_info e = { gl::viewer::event_mouse_move,
                                     lastButton, x, y };
        if (viewer) viewer->input( &e );
    }
}

#if USE_STENCIL_SHAPE

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include "glutint.h"

static GC gc = 0;
static Pixmap shape_mask = 0;
static unsigned int shapew = 0, shapeh = 0, bwidth = 0;
static XImage *shape_image = 0;
static char *stencil_data = 0;

static void resizeshapemask (void)
{
  Display *display = __glutDisplay;
  Window window = __glutCurrentWindow->win;

  Window root;
  int x, y;
  unsigned int h, w, depth;
  XGCValues gcvalues;

  /* Create a pixmap the same size as the window */
  XGetGeometry( display, window, &root, &x, &y,
		&w, &h, &bwidth, &depth );

  if (! shape_mask || w != shapew || h != shapeh)
    {
      if (gc) XFreeGC(display, gc);
      if (shape_mask) XFreePixmap(display, shape_mask);
      if (stencil_data) delete [] stencil_data;

      shapew = w; shapeh = h;
      gcvalues.foreground = 1;
      gcvalues.background = 0;
      shape_mask = XCreatePixmap( display, root, shapew, shapeh, 1 );
      gc = XCreateGC(display, shape_mask, GCForeground | GCBackground, &gcvalues);
      stencil_data = new char[shapew*shapeh];
    }
}


static void doshape (void)
{
  Display *display = __glutDisplay;
  Window window = __glutCurrentWindow->win;

  if (! shape_mask) resizeshapemask();

  if (! shape_image)
    {
      shape_image = XGetImage(display, shape_mask, 0, 0, shapew, shapeh,
			      1, XYPixmap);
      if (! shape_image->data) {
	XDestroyImage(shape_image);
	shape_image = 0;
	return;
      }
    }

  glReadPixels(0, 0, shapew, shapeh, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE,
	       stencil_data);

  // yuck
  int i,j,k = 0;
  for (j=0; j<shapeh; ++j)
    {
      for (i=0; i<shapew; ++i)
	{
	  XPutPixel(shape_image, i, shapeh-j-1,
		    (unsigned long) stencil_data[k++]);
	}
    }

  XPutImage(display, shape_mask, gc, shape_image, 0, 0, 0, 0, shapew, shapeh);

  XShapeCombineMask(display,window,ShapeBounding, -bwidth, -bwidth,
		    shape_mask,
		    ShapeSet );
}

#endif

//  Window resize callback

static void reshape(int width, int height)
{
  ViewerGlut *viewer = getViewer( glutGetWindow() );
  if (viewer) viewer->resize(width, height);
#if USE_STENCIL_SHAPE
  resizeshapemask();
#endif
}

ViewerGlut::ViewerGlut(openvrml::browser & browser):
    gl::viewer(browser)
{
#if USE_STENCIL_SHAPE
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_STENCIL);
#else
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
#endif

#if 0
    if (!glutGet(GLenum(GLUT_DISPLAY_MODE_POSSIBLE))) { exit(1); }
#endif

    glutCreateWindow("VRML97 OpenGL Viewer");
    d_window = glutGetWindow();
    d_timerPending = false;

    // If d_window is not "small", convert viewers to a map...
    if (d_window > MAX_WINDOWS) {
        std::cout << "ViewerGlut: too many windows." << std::endl;
        return;
    }

    viewers[d_window-1] = this;

    // Register callbacks
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKey);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutPassiveMotionFunc(passiveMotion);
    glutReshapeFunc(reshape);
}


ViewerGlut::~ViewerGlut()
{
    viewers[d_window-1] = 0;
    glutDestroyWindow(d_window);
}


void ViewerGlut::post_redraw()
{
    glutPostRedisplay();
}

void ViewerGlut::set_cursor(const cursor_style c)
{
    static const int cursorMap[] = { GLUT_CURSOR_INHERIT,
                                     GLUT_CURSOR_INFO,
                                     GLUT_CURSOR_CYCLE,
                                     GLUT_CURSOR_UP_DOWN,
                                     GLUT_CURSOR_CROSSHAIR
    };
    glutSetCursor(cursorMap[c]);
}

void ViewerGlut::swap_buffers() {
#if USE_STENCIL_SHAPE
    doshape();
#endif
    glutSwapBuffers();
}

// Timer callback calls the viewer update() method.
extern "C" void timer(int)
{
    ViewerGlut * viewer = getViewer(glutGetWindow());
    if (viewer) { viewer->timerUpdate(); }
}

void ViewerGlut::timerUpdate()
{
    this->d_timerPending = false;
    this->update(0.0);
}

// I think glut timers are one-shot deals.
void ViewerGlut::set_timer(const double t)
{
    if (!this->d_timerPending) {
        unsigned millis = unsigned(1000.0 * t);
        glutTimerFunc(millis, timer, 1);
        this->d_timerPending = true;
    }
}
