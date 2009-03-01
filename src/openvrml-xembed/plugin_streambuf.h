// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML XEmbed Control
//
// Copyright 2004, 2005, 2006, 2007, 2008  Braden N. McDaniel
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

# ifndef OPENVRML_XEMBED_PLUGIN_STREAMBUF_H
#   define OPENVRML_XEMBED_PLUGIN_STREAMBUF_H

#   include <map>
#   include <streambuf>
#   include <boost/noncopyable.hpp>
#   include <boost/shared_ptr.hpp>
#   include <boost/enable_shared_from_this.hpp>
#   include <openvrml/read_write_mutex.h>
#   include "bounded_buffer.h"
#   include "browser.h"

namespace openvrml_xembed {

    //
    // plugin_streambuf Life Cycle
    //
    // A plugin_streambuf is first created in GtkVrmlBrowser's
    // resource_fetcher::do_get_resource implementation (which is
    // called whenever libopenvrml needs to load a stream).
    //
    // Step 1: Requested plugin_streambuf
    //
    // Upon creation, the plugin_streambuf is inserted into the
    // unintialized_plugin_streambuf_map with an initial
    // plugin_streambuf::state of requested.  do_get_resource does not
    // complete until the result of asking the host application to resolve the
    // URL is known; i.e., plugin_streambuf::get_url_result.  get_url_result
    // blocks until the response is received from the host application; i.e.,
    // until plugin_streambuf::set_get_url_result has been called.
    //
    // Step 2: Uninitialized plugin_streambuf
    //
    // If plugin_streambuf::set_get_url_result is given a result code
    // indicating success (i.e., 0), the plugin_streambuf::state is changed to
    // uninitialized (otherwise, the stream is removed from the
    // uninitialized_plugin_streambuf_map and we're done).  When
    // openvrml_xembed_stream_client_new_stream is called on the
    // GtkVrmlBrowser, a plugin_streambuf matching that URL is gotten from the
    // uninitialized_plugin_streambuf_map and plugin_streambuf::init is called
    // on it.  init removes the plugin_streambuf from the
    // uninitialized_plugin_streambuf_map_ and inserts it in the
    // plugin_streambuf_map with a plugin_streambuf::state of initialized.
    //
    // Step 3: Initialized plugin_streambuf (plugin_streambuf_map)
    //
    // The plugin_streambuf_map comprises plugin_streambufs that are being
    // written to in response to openvrml_xembed_stream_client_write calls and
    // read from by stream readers in libopenvrml.  Once the host is done
    // calling openvrml_xembed_stream_client_write for a stream, it is
    // expected that it will call
    // openvrml_xembed_stream_client_destroy_stream.  In response to this
    // call, bounded_buffer<>::set_eof is called on the plugin_streambuf's
    // underlying bounded_buffer<> and the plugin_streambuf is removed from
    // the plugin_streambuf_map.
    //
    // Once the last reference to the resource_istream corresponding
    // to the plugin_streambuf is removed, the plugin_streambuf is
    // deleted.
    //

    class uninitialized_plugin_streambuf_map;
    class plugin_streambuf_map;

    class plugin_streambuf :
        public boost::enable_shared_from_this<plugin_streambuf>,
        public std::streambuf {

        friend
        GObject *
        (::openvrml_xembed_browser_constructor)(
            GType type,
            guint n_construct_properties,
            GObjectConstructParam * construct_properties);
        friend
        gboolean
        (::openvrml_xembed_browser_destroy_stream)(OpenvrmlXembedStreamClient *,
                                                   guint64, GError **);
        friend
        gboolean
        (::openvrml_xembed_browser_write)(OpenvrmlXembedStreamClient *, guint64,
                                          const GArray *, GError **);

    public:
        enum state_id {
            requested,
            uninitialized,
            initialized
        };

    private:
        state_id state_;
        mutable boost::mutex mutex_;
        int get_url_result_;
        mutable boost::condition received_get_url_result_;
        mutable boost::condition streambuf_initialized_or_failed_;
        std::string url_;
        std::string type_;
        bounded_buffer<char_type, 16384> buf_;
        int_type i_;
        char_type c_;
        uninitialized_plugin_streambuf_map & uninitialized_map_;
        plugin_streambuf_map & map_;

    protected:
        virtual int_type underflow();

    public:
        plugin_streambuf(const std::string & requested_url,
                         uninitialized_plugin_streambuf_map & uninitialized_map,
                         plugin_streambuf_map & map);
        state_id state() const;
        void set_get_url_result(int result);
        void init(size_t stream_id,
                  const std::string & received_url,
                  const std::string & type);
        void fail();
        const std::string & url() const;
        const std::string & type() const;
        bool data_available() const;
    };

    class uninitialized_plugin_streambuf_map : boost::noncopyable {
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
        bool erase(const plugin_streambuf & streambuf);
        size_t size() const;
        bool empty() const;
        const boost::shared_ptr<plugin_streambuf> front() const;
    };

    class plugin_streambuf_map : boost::noncopyable {
        mutable openvrml::read_write_mutex mutex_;
        typedef std::map<size_t, boost::shared_ptr<plugin_streambuf> > map_t;
        map_t map_;

    public:
        const boost::shared_ptr<plugin_streambuf> find(size_t id) const;
        bool insert(size_t id,
                    const boost::shared_ptr<plugin_streambuf> & streambuf);
        bool erase(size_t id);
    };
}

# endif // ifndef OPENVRML_XEMBED_PLUGIN_STREAMBUF_H
