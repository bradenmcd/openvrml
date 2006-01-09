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

# include <iostream>
# include <sstream>
# include <glib.h>
# include "browser.h"
# include "plugin_streambuf.h"

openvrml_player::browser::browser(GIOChannel * const request_channel):
    openvrml::browser(std::cout, std::cerr),
    request_channel_(request_channel)
{}

std::auto_ptr<openvrml::resource_istream>
openvrml_player::browser::do_get_resource(const std::string & uri)
{
    class plugin_resource_istream : public openvrml::resource_istream {
        boost::shared_ptr<plugin_streambuf> streambuf_;
        GIOChannel * request_channel_;

    public:
        explicit plugin_resource_istream(const std::string & uri,
                                         GIOChannel * const request_channel):
            openvrml::resource_istream(0),
            streambuf_(new plugin_streambuf(uri)),
            request_channel_(request_channel)
        {
            using std::ostringstream;

            this->rdbuf(this->streambuf_.get());
            bool succeeded =
                uninitialized_plugin_streambuf_set_.insert(this->streambuf_);
            g_assert(succeeded);

            ostringstream request;
            request << "get-url " << uri << '\n';
            gsize bytes_written;
            g_io_channel_write_chars(this->request_channel_,
                                     request.str().data(),
                                     request.str().length(),
                                     &bytes_written,
                                     0);
            g_io_channel_flush(this->request_channel_, 0);
        }

    private:
        virtual const std::string do_url() const throw ()
        {
            return this->streambuf_->url();
        }

        virtual const std::string do_type() const throw ()
        {
            return this->streambuf_->type();
        }

        virtual bool do_data_available() const throw ()
        {
            return this->streambuf_->data_available();
        }
    };
    return std::auto_ptr<openvrml::resource_istream>(
        new plugin_resource_istream(uri, this->request_channel_));
}
