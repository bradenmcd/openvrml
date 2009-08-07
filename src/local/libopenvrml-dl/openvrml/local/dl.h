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

# ifndef OPENVRML_LOCAL_DL_H
#   define OPENVRML_LOCAL_DL_H

#   include <openvrml-common.h>
#   ifdef _WIN32
#     include <windows.h>
#   else
#     include <ltdl.h>
#   endif
#   include <boost/filesystem/path.hpp>
#   include <string>
#   include <vector>

namespace openvrml {

    namespace local {

        namespace dl {

            OPENVRML_LOCAL int init();

            OPENVRML_LOCAL int exit();
            
            OPENVRML_LOCAL
            int
            foreachfile(
                const std::vector<boost::filesystem::path> & search_path,
                int (*func)(const std::string & filename, void * data),
                void * data);

            OPENVRML_LOCAL
            int prepend_to_searchpath(const boost::filesystem::path & dir);

#   ifdef _WIN32
            typedef HMODULE handle;
#   else
            typedef lt_dlhandle handle;
#   endif

            OPENVRML_LOCAL handle open(const std::string & filename);

            OPENVRML_LOCAL const std::string error();

            OPENVRML_LOCAL int close(handle);

            OPENVRML_LOCAL void * sym(handle, const std::string & name);
        }
    }
}

# endif // ifndef OPENVRML_LOCAL_DL_H
