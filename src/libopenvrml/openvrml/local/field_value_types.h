// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2008  Braden McDaniel
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 3 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, see <http://www.gnu.org/licenses/>.
//

# ifndef OPENVRML_LOCAL_FIELD_VALUE_TYPES_H
#   define OPENVRML_LOCAL_FIELD_VALUE_TYPES_H

#   include <openvrml-common.h>
#   include <boost/mpl/vector.hpp>

namespace openvrml {

    class sfbool;
    class sfcolor;
    class sfcolorrgba;
    class sfdouble;
    class sffloat;
    class sfimage;
    class sfint32;
    class sfnode;
    class sfrotation;
    class sfstring;
    class sftime;
    class sfvec2d;
    class sfvec2f;
    class sfvec3d;
    class sfvec3f;
    class mfbool;
    class mfcolor;
    class mfcolorrgba;
    class mfdouble;
    class mffloat;
    class mfimage;
    class mfint32;
    class mfnode;
    class mfrotation;
    class mfstring;
    class mftime;
    class mfvec2d;
    class mfvec2f;
    class mfvec3d;
    class mfvec3f;

    namespace local {

        typedef boost::mpl::vector<openvrml::sfbool,
                                   openvrml::sfcolor,
                                   openvrml::sfcolorrgba,
                                   openvrml::sfdouble,
                                   openvrml::sffloat,
                                   openvrml::sfimage,
                                   openvrml::sfint32,
                                   openvrml::sfnode,
                                   openvrml::sfrotation,
                                   openvrml::sfstring,
                                   openvrml::sftime,
                                   openvrml::sfvec2d,
                                   openvrml::sfvec2f,
                                   openvrml::sfvec3d,
                                   openvrml::sfvec3f,
                                   openvrml::mfbool,
                                   openvrml::mfcolor,
                                   openvrml::mfcolorrgba,
                                   openvrml::mfdouble,
                                   openvrml::mffloat,
                                   openvrml::mfimage,
                                   openvrml::mfint32,
                                   openvrml::mfnode,
                                   openvrml::mfrotation,
                                   openvrml::mfstring,
                                   openvrml::mftime,
                                   openvrml::mfvec2d,
                                   openvrml::mfvec2f,
                                   openvrml::mfvec3d,
                                   openvrml::mfvec3f>
        field_value_types;
    }
}

# endif // ifndef OPENVRML_LOCAL_FIELD_VALUE_TYPES_H
