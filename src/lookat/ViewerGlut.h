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

#include <OpenVRML/GL/ViewerOpenGL.h>

/**
 * GLUT version of OpenGL class for display of VRML models.
 */
class ViewerGlut : public ViewerOpenGL {

public:

  ViewerGlut(VrmlScene *);
  virtual ~ViewerGlut();

  // Public so glut callbacks can access
  void timerUpdate();

protected:

  // Window system specific methods

  virtual void wsPostRedraw();
  virtual void wsSetCursor( CursorStyle c);
  virtual void wsSwapBuffers();
  virtual void wsSetTimer( double );

private:

  int d_window;
  bool d_timerPending;

};

#endif // VIEWERGLUT_H


