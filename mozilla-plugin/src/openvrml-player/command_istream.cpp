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

# include <cassert>
# include <cerrno>
# include "command_istream.h"

openvrml_player::command_streambuf::command_streambuf():
    c_('\0')
{
    this->setg(&this->c_, &this->c_, &this->c_);
}

openvrml_player::command_streambuf::int_type
openvrml_player::command_streambuf::underflow()
{
    const int_type i = this->source_buffer_.get();
    if (traits_type::eq_int_type(i, traits_type::eof())) {
        return traits_type::eof();
    }
    this->c_ = traits_type::to_char_type(i);
    this->setg(&this->c_, &this->c_, &this->c_ + 1);
    return i;
}


openvrml_player::command_istream::command_istream():
    std::istream(&this->buf_)
{}

gboolean command_data_available(GIOChannel * source,
                                GIOCondition,
                                gpointer data)
{
    using namespace openvrml_player;

    typedef command_istream::traits_type traits_type;

    command_streambuf & streambuf =
        *static_cast<command_streambuf *>(data);

    do {
        gchar c;
        gsize bytes_read;
        GError * error = 0;
        const GIOStatus status =
            g_io_channel_read_chars(source, &c, 1, &bytes_read, &error);
        if (status == G_IO_STATUS_ERROR) {
            if (error) {
                g_warning(error->message);
                g_error_free(error);
            }
            return false;
        }
        if (status == G_IO_STATUS_EOF) {
            streambuf.source_buffer_.set_eof();
            return false;
        }
        if (status == G_IO_STATUS_AGAIN) { continue; }
        g_return_val_if_fail(status == G_IO_STATUS_NORMAL, false);

        g_assert(bytes_read == 1);

        streambuf.source_buffer_.put(c);

    } while (g_io_channel_get_buffer_condition(source) & G_IO_IN);

    return true;
}
