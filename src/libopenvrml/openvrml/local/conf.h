// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2009  Braden McDaniel
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

# ifndef OPENVRML_LOCAL_CONF_H
#   define OPENVRML_LOCAL_CONF_H

#   include <openvrml-common.h>
#   include <stdexcept>
#   include <string>

namespace openvrml {

    namespace local {

        namespace conf {

            OPENVRML_LOCAL const std::string datadir()
                OPENVRML_THROW2(std::runtime_error, std::bad_alloc);
            OPENVRML_LOCAL const std::string node_path()
                OPENVRML_THROW2(std::runtime_error, std::bad_alloc);
            OPENVRML_LOCAL const std::string script_path()
                OPENVRML_THROW2(std::runtime_error, std::bad_alloc);
        }
    }
}

# endif // ifndef OPENVRML_LOCAL_CONF_H
