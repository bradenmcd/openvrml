//
// OpenVRML
//
// Copyright (C) 1998-2000  Chris Morley
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

#ifndef CONFIG_H
#define CONFIG_H

#define PACKAGE_VERSION  "openvrml-0.13.0"

#define OPENVRML_HAVE_ZLIB
#define OPENVRML_HAVE_LIBJPEG
#define OPENVRML_HAVE_LIBPNG
#define OPENVRML_HAVE_GLUT
#define OPENVRML_HAVE_SPIDERMONKEY
#define OPENVRML_HAVE_JNI
#define OPENVRML_HAVE_FREETYPEFONTS

#define FONTSDIR                 "c:/winnt/fonts"
#define SERIFPLAIN               FONTSDIR"/times.ttf"
#define SERIFBOLD                FONTSDIR"/timesbd.ttf"
#define SERIFITALIC              FONTSDIR"/timesi.ttf"
#define SERIFBOLDITALIC          FONTSDIR"/timesbi.ttf"
#define SANSPLAIN                FONTSDIR"/arial.ttf"
#define SANSBOLD                 FONTSDIR"/arialbd.ttf"
#define SANSITALIC               FONTSDIR"/ariali.ttf"
#define SANSBOLDITALIC           FONTSDIR"/arialbi.ttf"
#define TYPEWRITERPLAIN          FONTSDIR"/cour.ttf"
#define TYPEWRITERBOLD           FONTSDIR"/courbd.ttf"
#define TYPEWRITERITALIC         FONTSDIR"/couri.ttf"
#define TYPEWRITERBOLDITALIC     FONTSDIR"/courbi.ttf"

#define OPENVRML_GL_H   <GL/gl.h>
#define OPENVRML_GLU_H  <GL/glu.h>
#define OPENVRML_GLUT_H <GL/glut.h>

#endif	/* CONFIG_H */
