// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML Mozilla plug-in
// Copyright 2004, 2005, 2006  Braden N. McDaniel
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

# ifndef OPENVRML_PLAYER_COMMAND_ISTREAM_H
#   define OPENVRML_PLAYER_COMMAND_ISTREAM_H

#   include <streambuf>
#   include <glib.h>

#   include "bounded_buffer.h"

extern "C" gboolean command_data_available(GIOChannel * source,
                                           GIOCondition condition,
                                           gpointer data);

namespace openvrml_player {

    class command_streambuf : boost::noncopyable, public std::streambuf {
        friend gboolean (::command_data_available)(GIOChannel * source,
                                                   GIOCondition condition,
                                                   gpointer data);

        bounded_buffer<char_type, 16384> source_buffer_;
        char_type c_;

    protected:
        virtual int_type underflow();

    public:
        command_streambuf();
    };


    class command_istream : boost::noncopyable, public std::istream {
        command_streambuf buf_;

    public:
        command_istream();
    };
}

# endif // ifndef OPENVRML_PLAYER_COMMAND_ISTREAM_H
