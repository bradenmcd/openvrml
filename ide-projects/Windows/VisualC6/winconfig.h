#ifndef WINCONFIG_H
#define WINCONFIG_H
/*
  OpenVrml library
  Copyright (C) 1998 Chris Morley
  See the file COPYING for license details.
  winconfig.h
*/

#define OPENVRML_MAJOR_VERSION	0
#define OPENVRML_MINOR_VERSION	12
#define OPENVRML_MICRO_VERSION	0

// Disable warning messages about truncation from const double to float
# pragma warning( disable : 4305 )  
// Disable warning messages about truncating names in debug info
# pragma warning( disable : 4786 )  

#define OPENVRML_HAVE_LIBJPEG
#define OPENVRML_HAVE_LIBPNG
#define OPENVRML_HAVE_SPIDERMONKEY
/* #define OPENVRML_HAVE_JNI */

#define OPENVRML_GL_H   <GL/gl.h>
#define OPENVRML_GLU_H  <GL/glu.h>
#define OPENVRML_GLUT_H <GL/glut.h>

#define OPENVRML_JSAPI_H <jsapi.h>

#endif	/* WINCONFIG_H */
