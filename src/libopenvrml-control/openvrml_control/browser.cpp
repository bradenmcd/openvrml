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

# include "browser.h"
# include <openvrml/browser.h>
# include <boost/lexical_cast.hpp>
# include <iostream>

openvrml_control::unknown_stream::unknown_stream(const std::string & uri):
    std::logic_error("no stream corresponding to URI: " + uri)
{}

openvrml_control::unknown_stream::unknown_stream(const uint64_t stream_id):
    std::logic_error("no stream corresponding to stream ID: "
                     + boost::lexical_cast<std::string>(stream_id))
{}

openvrml_control::unknown_stream::~unknown_stream() throw ()
{}

openvrml_control::browser::resource_fetcher::
resource_fetcher(browser_host & control_host,
                 openvrml_xembed::uninitialized_plugin_streambuf_map &
                     uninitialized_plugin_streambuf_map,
                 openvrml_xembed::plugin_streambuf_map & plugin_streambuf_map):
    control_host_(control_host),
    uninitialized_plugin_streambuf_map_(uninitialized_plugin_streambuf_map),
    plugin_streambuf_map_(plugin_streambuf_map)
{}

openvrml_control::browser::resource_fetcher::~resource_fetcher()
    OPENVRML_NOTHROW
{
    this->thread_group_.join_all();
}

void
openvrml_control::browser::resource_fetcher::
create_thread(const boost::function0<void> & threadfunc)
{
    this->thread_group_.create_thread(threadfunc);
}

std::auto_ptr<openvrml::resource_istream>
openvrml_control::browser::resource_fetcher::
do_get_resource(const std::string & uri)
{
    using openvrml_xembed::plugin_streambuf;

    class plugin_resource_istream : public openvrml::resource_istream {
        const boost::shared_ptr<plugin_streambuf> streambuf_;
        resource_fetcher & resource_fetcher_;

    public:
        plugin_resource_istream(const std::string & uri,
                                resource_fetcher & fetcher):
            openvrml::resource_istream(0),
            streambuf_(
                new plugin_streambuf(
                    uri,
                    fetcher.uninitialized_plugin_streambuf_map_,
                    fetcher.plugin_streambuf_map_)),
            resource_fetcher_(fetcher)
        {
            this->rdbuf(this->streambuf_.get());
            fetcher.uninitialized_plugin_streambuf_map_.insert(
                uri, this->streambuf_);

            //
            // browser_host::get_url could throw; let it.
            //
            const int get_url_result = fetcher.control_host_.get_url(uri);

            if (get_url_result != 0) {
                this->setstate(ios_base::badbit);
            }
            this->streambuf_->set_get_url_result(get_url_result);
        }

    private:
        virtual const std::string do_url() const OPENVRML_NOTHROW
        {
            return this->streambuf_->url();
        }

        virtual const std::string do_type() const OPENVRML_NOTHROW
        {
            return this->streambuf_->type();
        }

        virtual bool do_data_available() const OPENVRML_NOTHROW
        {
            return this->streambuf_->data_available();
        }
    };
    return std::auto_ptr<openvrml::resource_istream>(
        new plugin_resource_istream(uri, *this));
}

openvrml_control::browser::browser_listener::browser_listener(browser & b):
    browser_(b)
{}

void
openvrml_control::browser::browser_listener::
do_browser_changed(const openvrml::browser_event & event)
{
    switch (event.id()) {
    case openvrml::browser_event::initialized:
        {
            boost::mutex::scoped_lock lock(this->browser_.initialized_mutex_);
            this->browser_.initialized_ = true;
        }
        break;
    default:
        break;
    }
}

namespace {
    //
    // We don't already know what the URI of the initial stream is until we
    // start getting that data from the browser.  This is a placeholder that
    // is used to identify the plugin_streambuf that will be used to receive
    // the initial stream data.
    //
    const char initial_stream_uri[] = "x-openvrml-initial:";

    struct OPENVRML_LOCAL initial_stream_reader {
        initial_stream_reader(
            const boost::shared_ptr<openvrml_xembed::plugin_streambuf> &
                streambuf,
            openvrml::browser & browser):
            streambuf_(streambuf),
            browser_(browser)
        {}

        void operator()() const throw ()
        {
            using openvrml_xembed::plugin_streambuf;

            class plugin_istream : public openvrml::resource_istream {
                boost::shared_ptr<plugin_streambuf> streambuf_;

            public:
                explicit plugin_istream(
                    const boost::shared_ptr<plugin_streambuf> & streambuf):
                    openvrml::resource_istream(streambuf.get()),
                    streambuf_(streambuf)
                {}

                virtual ~plugin_istream() throw ()
                {}

            private:
                virtual const std::string do_url() const throw (std::bad_alloc)
                {
                    return this->streambuf_->url();
                }

                virtual const std::string do_type() const
                    throw (std::bad_alloc)
                {
                    return this->streambuf_->type();
                }

                virtual bool do_data_available() const throw ()
                {
                    return this->streambuf_->data_available();
                }
            } in(this->streambuf_);

            this->browser_.set_world(in);
        }

    private:
        boost::shared_ptr<openvrml_xembed::plugin_streambuf> streambuf_;
        openvrml::browser & browser_;
    };
}

openvrml_control::browser::browser(browser_host & host,
                                   const bool expect_initial_stream):
    fetcher_(host,
             this->uninitialized_streambuf_map_,
             this->streambuf_map_),
    listener_(*this),
    browser_(this->fetcher_, std::cout, std::cerr),
    host_(host),
    expect_initial_stream_(expect_initial_stream),
    got_initial_stream_(false)
{
    this->browser_.add_listener(this->listener_);

    //
    // If necessary, create the initial stream.
    //
    if (expect_initial_stream) {
        using boost::function0;
        using boost::shared_ptr;
        using openvrml_xembed::plugin_streambuf;

        const shared_ptr<plugin_streambuf> initial_stream(
            new plugin_streambuf(::initial_stream_uri,
                                 this->uninitialized_streambuf_map_,
                                 this->streambuf_map_));
        initial_stream->state_ = plugin_streambuf::uninitialized;
        this->uninitialized_streambuf_map_.insert(::initial_stream_uri,
                                                  initial_stream);

        const function0<void> initial_stream_reader_func =
            initial_stream_reader(initial_stream, this->browser_);

        this->initial_stream_reader_thread_.reset(
            new boost::thread(initial_stream_reader_func));
    }
}

openvrml_control::browser::~browser() OPENVRML_NOTHROW
{
    if (this->expect_initial_stream_) {
        this->initial_stream_reader_thread_->join();
    }
    this->browser_.remove_listener(this->listener_);
}

bool openvrml_control::browser::initialized() const
{
    boost::mutex::scoped_lock lock(this->initialized_mutex_);
    return this->initialized_;
}

void openvrml_control::browser::new_stream(const uint64_t stream_id,
                                           const std::string & type,
                                           const std::string & uri)
    OPENVRML_THROW1(unknown_stream)
{
    using namespace openvrml_xembed;
    using boost::shared_ptr;

    shared_ptr<plugin_streambuf> streambuf =
        this->uninitialized_streambuf_map_.find(uri);

    if (!streambuf) {
        if (!this->got_initial_stream_) {
            assert(this->uninitialized_streambuf_map_.size() == 1);
            streambuf = this->uninitialized_streambuf_map_.front();
            this->got_initial_stream_ = true;
        } else {
            throw unknown_stream(uri);
        }
    }
    assert(streambuf->state() != plugin_streambuf::initialized);
    streambuf->init(stream_id, uri, type);
}

void openvrml_control::browser::destroy_stream(const uint64_t stream_id)
    OPENVRML_THROW1(unknown_stream)
{
    using namespace openvrml_xembed;
    using boost::shared_ptr;

    const shared_ptr<plugin_streambuf> streambuf =
        this->streambuf_map_.find(stream_id);
    if (!streambuf) { throw unknown_stream(stream_id); }
    streambuf->buf_.set_eof();
    this->streambuf_map_.erase(stream_id);
}

void openvrml_control::browser::write(const uint64_t stream_id,
                                      const unsigned char * const data,
                                      const size_t size)
    OPENVRML_THROW1(unknown_stream)
{
    using namespace openvrml_xembed;
    using boost::shared_ptr;

    const shared_ptr<plugin_streambuf> streambuf =
        this->streambuf_map_.find(stream_id);
    if (!streambuf) { throw unknown_stream(stream_id); }
    for (size_t i = 0; i < size; ++i) { streambuf->buf_.put(data[i]); }
}

struct OPENVRML_LOCAL openvrml_control::browser::load_url {
    load_url(browser & b,
             const std::vector<std::string> & url,
             const std::vector<std::string> & parameter):
        browser_(b),
        url_(url),
        parameter_(parameter)
    {}

    void operator()() const OPENVRML_NOTHROW
    {
        try {
            {
                boost::mutex::scoped_lock
                    lock(this->browser_.initialized_mutex_);
                this->browser_.initialized_ = false;
            }
            this->browser_.browser_.load_url(this->url_, this->parameter_);
        } catch (std::exception & ex) {
            this->browser_.browser_.err(ex.what());
        }
    }

 private:
    browser & browser_;
    const std::vector<std::string> url_, parameter_;
};

void
openvrml_control::browser::load_uri(const std::vector<std::string> & uri,
                                    const std::vector<std::string> & parameter)
    OPENVRML_THROW2(boost::thread_resource_error, std::bad_alloc)
{
    this->fetcher_.create_thread(load_url(*this, uri, parameter));
}

const std::string openvrml_control::browser::world_url()
{
    return this->browser_.world_url();
}

bool
openvrml_control::browser::add_listener(openvrml::browser_listener & listener)
{
    return this->browser_.add_listener(listener);
}

bool
openvrml_control::browser::
remove_listener(openvrml::browser_listener & listener)
{
    return this->browser_.remove_listener(listener);
}

void openvrml_control::browser::viewer(openvrml::viewer * v)
{
    this->browser_.viewer(v);
}

openvrml_control::browser_host::~browser_host()
{}

int openvrml_control::browser_host::get_url(const std::string & url)
{
    this->do_get_url(url);
}
