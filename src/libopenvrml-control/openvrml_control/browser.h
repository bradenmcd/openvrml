// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML Control
//
// Copyright 2009  Braden N. McDaniel
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

# ifndef OPENVRML_CONTROL_BROWSER_H
#   define OPENVRML_CONTROL_BROWSER_H

#   include <openvrml/browser.h>

namespace openvrml_control {

    class unknown_stream : public std::logic_error {
    public:
        explicit unknown_stream(const std::string & uri);
        explicit unknown_stream(uint64_t stream_id);
        virtual ~unknown_stream() throw ();
    };

    class browser_host;

    class browser {
        class plugin_streambuf;
        struct initial_stream_reader;
        struct load_url;

        class uninitialized_plugin_streambuf_map : boost::noncopyable {
            struct map_entry_matches_streambuf;

            mutable openvrml::read_write_mutex mutex_;
            typedef std::multimap<std::string,
                                  boost::shared_ptr<plugin_streambuf> >
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
            typedef std::map<size_t, boost::shared_ptr<plugin_streambuf> >
                map_t;
            map_t map_;

        public:
            const boost::shared_ptr<plugin_streambuf> find(size_t id) const;
            bool insert(size_t id,
                        const boost::shared_ptr<plugin_streambuf> & streambuf);
            bool erase(size_t id);
        };

        class resource_fetcher : public openvrml::resource_fetcher {
            browser_host & control_host_;
            uninitialized_plugin_streambuf_map &
                uninitialized_plugin_streambuf_map_;
            plugin_streambuf_map & plugin_streambuf_map_;
            boost::thread_group thread_group_;

        public:
            resource_fetcher(
                browser_host & control_host,
                uninitialized_plugin_streambuf_map &
                    uninitialized_plugin_streambuf_map,
                plugin_streambuf_map & plugin_streambuf_map);
            virtual ~resource_fetcher() OPENVRML_NOTHROW;

            void create_thread(const boost::function0<void> & threadfunc);

        private:
            virtual std::auto_ptr<openvrml::resource_istream>
            do_get_resource(const std::string & uri);
        };

        class browser_listener : public openvrml::browser_listener {
            browser & browser_;

        public:
            explicit browser_listener(browser & b);

        private:
            virtual void
            do_browser_changed(const openvrml::browser_event & event);
        };

        uninitialized_plugin_streambuf_map
            uninitialized_streambuf_map_;
        plugin_streambuf_map streambuf_map_;
        resource_fetcher fetcher_;
        browser_listener listener_;
        openvrml::browser browser_;
        browser_host & host_;
        boost::scoped_ptr<boost::thread> initial_stream_reader_thread_;
        bool expect_initial_stream_, got_initial_stream_;
        mutable boost::mutex initialized_mutex_;
        bool initialized_;

    public:
        browser(browser_host & host, bool expect_initial_stream);
        ~browser() OPENVRML_NOTHROW;

        bool initialized() const;

        void new_stream(uint64_t stream_id,
                        const std::string & type, const std::string & uri)
            OPENVRML_THROW1(unknown_stream);
        void destroy_stream(uint64_t stream_id)
            OPENVRML_THROW1(unknown_stream);
        void write(uint64_t stream_id, const unsigned char * data, size_t size)
            OPENVRML_THROW1(unknown_stream);

        void load_uri(const std::vector<std::string> & uri,
                      const std::vector<std::string> & parameter)
            OPENVRML_THROW2(boost::thread_resource_error, std::bad_alloc);
        const std::string world_url();

        bool add_listener(openvrml::browser_listener & listener);
        bool remove_listener(openvrml::browser_listener & listener);

        void viewer(openvrml::viewer * v);
    };


    class browser_host {
    public:
        virtual ~browser_host() = 0;

        int get_url(const std::string & url);

    private:
        virtual int do_get_url(const std::string & url) = 0;
    };
}

# endif // ifndef OPENVRML_CONTROL_BROWSER_H
