// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2009, 2010  Braden McDaniel
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

# include "error.h"
# include <boost/scope_exit.hpp>

# ifdef _WIN32
void openvrml::local::throw_runtime_error_from_win32_system_error(LONG result)
    OPENVRML_THROW1(std::runtime_error)
{
    static const LPCVOID source = 0;
    LPTSTR buf = 0;
    BOOST_SCOPE_EXIT((&buf)) {
        LocalFree(buf);
    } BOOST_SCOPE_EXIT_END
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM
                  | FORMAT_MESSAGE_ALLOCATE_BUFFER,
                  source,
                  static_cast<DWORD>(result),
                  LANG_USER_DEFAULT,
                  reinterpret_cast<LPTSTR>(&buf),
                  static_cast<DWORD>(0),
                  0);

    throw std::runtime_error(buf);
}
# endif
