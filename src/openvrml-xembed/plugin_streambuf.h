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

    //
    // plugin_streambuf Life Cycle
    //
    // A plugin_streambuf is first created in GtkVrmlBrowser's
    // resource_fetcher::do_get_resource implementation (which is
    // called whenever libopenvrml needs to load a stream).
    //
    // Step 1: requested_plugin_streambuf_map_
    //
    // Upon creation, the plugin_streambuf is inserted into the
    // requested_plugin_streambuf_map_.  do_get_resource does not
    // complete until the result of asking the host application to
    // resolve the URL is known; i.e.,
    // plugin_streambuf::get_url_result.  get_url_result blocks until
    // the response is received from the host application; i.e., until
    // plugin_streambuf::set_get_url_result has been called.
    // set_get_url_result removes the plugin_streambuf from the
    // requested_plugin_streambuf_map_.
    //
    // Step 2: uninitialized_plugin_streambuf_map_
    //
    // If plugin_streambuf::set_get_url_result is given a result code
    // indicating success (i.e., 0), the plugin_streambuf is inserted
    // in the uninitialized_plugin_streambuf_map_.  When a new-stream
    // command is received for a URL, a plugin_streambuf matching that
    // URL is gotten from the uninitialized_plugin_streambuf_map_ and
    // plugin_streambuf::init is called on it.  init removes the
    // plugin_streambuf from the uninitialized_plugin_streambuf_map_
    // and inserts it in the plugin_streambuf_map_.
    //
    // Step 3: plugin_streambuf_map_
    //
    // The plugin_streambuf_map_ comprises plugin_streambufs that are
    // being written to in response to write commands and read from by
    // stream readers in libopenvrml.  Once the host application is
    // done sending write commands for a stream, it is expected that
    // it will send a destroy-stream command.  In response to
    // destroy-stream, bounded_buffer<>::set_eof is called on the
    // plugin_streambuf's underlying bounded_buffer<> and the
    // plugin_streambuf is removed from the plugin_streambuf_map_.
    //
    // Once the last reference to the resource_istream corresponding
    // to the plugin_streambuf is removed, the plugin_streambuf is
    // deleted.
    //

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

    extern class requested_plugin_streambuf_map :
        std::multimap<std::string, boost::shared_ptr<plugin_streambuf> >,
        boost::noncopyable {

        struct map_entry_matches_streambuf;

        typedef std::multimap<std::string,
                              boost::shared_ptr<plugin_streambuf> >
            base_t;

        mutable openvrml::read_write_mutex mutex_;

    public:
        const boost::shared_ptr<plugin_streambuf>
        find(const std::string & url) const;
        void insert(const std::string & url,
                    const boost::shared_ptr<plugin_streambuf> & streambuf);
        bool erase(const plugin_streambuf & streambuf);
    } requested_plugin_streambuf_map_;

    extern class uninitialized_plugin_streambuf_map : boost::noncopyable {
        struct map_entry_matches_streambuf;

        mutable openvrml::read_write_mutex mutex_;
        typedef std::multimap<std::string, boost::shared_ptr<plugin_streambuf> >
            map_t;
        map_t map_;

    public:
        const boost::shared_ptr<plugin_streambuf>
        find(const std::string & url) const;
        void insert(const std::string & url,
                    const boost::shared_ptr<plugin_streambuf> & streambuf);
        bool erase(const plugin_streambuf & url);
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
