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

# ifndef OPENVRML_XEMBED_PLUGIN_STREAMBUF_H
#   define OPENVRML_XEMBED_PLUGIN_STREAMBUF_H

#   include <map>
#   include <set>
#   include <streambuf>
#   include <boost/noncopyable.hpp>
#   include <boost/shared_ptr.hpp>
#   include <boost/enable_shared_from_this.hpp>
#   include <openvrml/read_write_mutex.h>
#   include "bounded_buffer.h"

namespace openvrml_xembed {

    class command_istream_reader;

    class plugin_streambuf :
        public boost::enable_shared_from_this<plugin_streambuf>,
        public std::streambuf {

        friend class command_istream_reader;

        mutable boost::mutex mutex_;
        int get_url_result_;
        mutable boost::condition received_get_url_result_;
        bool initialized_;
        mutable boost::condition streambuf_initialized_or_failed_;
        std::string url_;
        std::string type_;
        bounded_buffer<char_type, 16384> buf_;
        int_type i_;
        char_type c_;

    protected:
        virtual int_type underflow();

    public:
        explicit plugin_streambuf(const std::string & requested_url);
        void set_get_url_result(int result);
        int get_url_result() const;
        void init(size_t stream_id,
                  const std::string & received_url,
                  const std::string & type);
        void fail();
        const std::string & url() const;
        const std::string & type() const;
        bool data_available() const;
    };

    extern class uninitialized_plugin_streambuf_map : boost::noncopyable {
        mutable openvrml::read_write_mutex mutex_;
        typedef std::multimap<std::string, boost::shared_ptr<plugin_streambuf> >
            map_t;
        map_t map_;

    public:
        const boost::shared_ptr<plugin_streambuf>
        find(const std::string & url) const;
        void insert(const std::string & url,
                    const boost::shared_ptr<plugin_streambuf> & streambuf);
        bool erase(const std::string & url);
        size_t size() const;
        bool empty() const;
        const boost::shared_ptr<plugin_streambuf> front() const;
    } uninitialized_plugin_streambuf_map_;

    extern class plugin_streambuf_map : boost::noncopyable {
        mutable openvrml::read_write_mutex mutex_;
        typedef std::map<size_t, boost::shared_ptr<plugin_streambuf> > map_t;
        map_t map_;

    public:
        const boost::shared_ptr<plugin_streambuf> find(size_t id) const;
        bool insert(size_t id,
                    const boost::shared_ptr<plugin_streambuf> & streambuf);
        bool erase(size_t id);
    } plugin_streambuf_map_;
}

# endif // ifndef OPENVRML_XEMBED_PLUGIN_STREAMBUF_H
