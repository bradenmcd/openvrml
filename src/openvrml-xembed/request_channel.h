// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML XEmbed Control
//
// Copyright 2007  Braden McDaniel
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along
// with this library; if not, see <http://www.gnu.org/licenses/>.
//

# ifndef OPENVRML_XEMBED_REQUEST_CHANNEL_H
#   define OPENVRML_XEMBED_REQUEST_CHANNEL_H

namespace openvrml_xembed {

    bool write_request_chars(const gchar * buf, gssize count,
                             gsize * bytes_written);
}

# endif // ifndef OPENVRML_XEMBED_REQUEST_CHANNEL_H
