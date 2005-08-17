// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 2005  Braden McDaniel
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


# ifndef OPENVRML_GL_CONFIG_H
#   define OPENVRML_GL_CONFIG_H

#   if defined(OPENVRML_GL_BUILD_DLL)
#     define OPENVRML_GL_API __declspec(dllexport)
#   elif defined(OPENVRML_GL_USE_DLL)
#     define OPENVRML_GL_API __declspec(dllimport)
#   elif defined(OPENVRML_GL_BUILD_LIB)
#     define OPENVRML_GL_API
#   elif defined(OPENVRML_GL_USE_LIB)
#     define OPENVRML_GL_API
#   else
#     error Must define one of OPENVRML_GL_BUILD_DLL, OPENVRML_GL_USE_DLL, OPENVRML_GL_BUILD_LIB, or OPENVRML_GL_USE_LIB.
#   endif

#   define OPENVRML_GL_LOCAL

# endif // ifndef OPENVRML_GL_CONFIG_H
