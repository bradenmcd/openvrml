CXXFLAGS = -O2 -Wall
LDFLAGS = -O2
INCLUDES = @GL_CFLAGS@

vrml97glincludedir = $(includedir)/vrml97gl

lib_LTLIBRARIES = libvrml97gl.la

vrml97glinclude_HEADERS = ViewerOpenGL.h

noinst_HEADERS = OpenGLEvent.h

libvrml97gl_la_SOURCES = ViewerOpenGL.cpp OpenGLEvent.cpp
