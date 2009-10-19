// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2009  Braden McDaniel
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 3 of the License, or (at
// your option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, see <http://www.gnu.org/licenses/>.
//

# ifndef OPENVRML_LOCAL_ERROR_H
#   define OPENVRML_LOCAL_ERROR_H

#   include <openvrml-common.h>
#   include <stdexcept>
#   ifdef _WIN32
#     include <windows.h>
#   endif

namespace openvrml {

    namespace local {

#   ifdef _WIN32
        OPENVRML_LOCAL
        void throw_runtime_error_from_win32_system_error(LONG error)
            OPENVRML_THROW1(std::runtime_error);
#   endif
    }
}

# endif // ifndef OPENVRML_LOCAL_ERROR_H
