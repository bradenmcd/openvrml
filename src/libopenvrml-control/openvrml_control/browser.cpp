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
# include <boost/enable_shared_from_this.hpp>
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

//
// plugin_streambuf Life Cycle
//
// A plugin_streambuf is first created in
// browser::resource_fetcher::do_get_resource (which is called whenever
// libopenvrml needs to load a stream).
//
// Step 1: Requested plugin_streambuf
//
// Upon creation, the plugin_streambuf is inserted into the
// unintialized_plugin_streambuf_map with an initial plugin_streambuf::state
// of requested.  do_get_resource does not complete until the result of asking
// the host application to resolve the URL is known; i.e.,
// plugin_streambuf::get_url_result.  get_url_result blocks until the response
// is received from the host application; i.e., until
// plugin_streambuf::set_get_url_result has been called.
//
// Step 2: Uninitialized plugin_streambuf
//
// If plugin_streambuf::set_get_url_result is given a result code indicating
// success (i.e., 0), the plugin_streambuf::state is changed to uninitialized
// (otherwise, the stream is removed from the
// uninitialized_plugin_streambuf_map and we're done).  When
// browser::new_stream is called, a plugin_streambuf matching that URL is
// gotten from the uninitialized_plugin_streambuf_map and
// plugin_streambuf::init is called on it.  init removes the plugin_streambuf
// from the uninitialized_plugin_streambuf_map_ and inserts it in the
// plugin_streambuf_map with a plugin_streambuf::state of initialized.
//
// Step 3: Initialized plugin_streambuf (plugin_streambuf_map)
//
// The plugin_streambuf_map comprises plugin_streambufs that are being written
// to in response to browser::write calls and read from by stream readers in
// libopenvrml.  Once the host is done calling browser::write for a stream, it
// is expected that it will call browser::destroy_stream.  In response to this
// call, bounded_buffer<>::set_eof is called on the plugin_streambuf's
// underlying bounded_buffer<> and the plugin_streambuf is removed from the
// plugin_streambuf_map.
//
// Once the last reference to the resource_istream corresponding to the
// plugin_streambuf is removed, the plugin_streambuf is deleted.
//

namespace {

    template <typename CharT, size_t BufferSize>
    class bounded_buffer {
        mutable boost::mutex mutex_;
        boost::condition buffer_not_full_, buffer_not_empty_or_eof_;

        CharT buf_[BufferSize];
        size_t begin_, end_, buffered_;
        bool eof_;

    public:
        typedef CharT char_type;
        typedef typename std::char_traits<char_type> traits_type;
        typedef typename traits_type::int_type int_type;

        bounded_buffer();
        void put(const char_type & c);
        int_type get();
        size_t buffered() const;
        void set_eof();
        bool eof() const;
    };

    template <typename CharT, size_t BufferSize>
    bounded_buffer<CharT, BufferSize>::bounded_buffer():
        begin_(0),
        end_(0),
        buffered_(0),
        eof_(false)
    {}

    template <typename CharT, size_t BufferSize>
    void bounded_buffer<CharT, BufferSize>::put(const char_type & c)
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        while (this->buffered_ == BufferSize) {
            this->buffer_not_full_.wait(lock);
        }
        this->buf_[this->end_] = c;
        this->end_ = (this->end_ + 1) % BufferSize;
        ++this->buffered_;
        this->buffer_not_empty_or_eof_.notify_one();
    }

    template <typename CharT, size_t BufferSize>
    typename bounded_buffer<CharT, BufferSize>::int_type
    bounded_buffer<CharT, BufferSize>::get()
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        while (this->buffered_ == 0 && !this->eof_) {
            this->buffer_not_empty_or_eof_.wait(lock);
        }
        if (this->buffered_ == 0 && this->eof_) {
            return traits_type::eof();
        }
        const int_type c = traits_type::to_int_type(this->buf_[this->begin_]);
        this->begin_ = (this->begin_ + 1) % BufferSize;
        --this->buffered_;
        this->buffer_not_full_.notify_one();
        assert(!traits_type::eq_int_type(c, traits_type::eof()));
        return c;
    }

    template <typename CharT, size_t BufferSize>
    size_t bounded_buffer<CharT, BufferSize>::buffered() const
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        return this->buffered_;
    }

    template <typename CharT, size_t BufferSize>
    void bounded_buffer<CharT, BufferSize>::set_eof()
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        this->eof_ = true;
        this->buffer_not_empty_or_eof_.notify_one();
    }

    template <typename CharT, size_t BufferSize>
    bool bounded_buffer<CharT, BufferSize>::eof() const
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        return this->eof_;
    }
}

class OPENVRML_LOCAL openvrml_control::browser::plugin_streambuf :
    public boost::enable_shared_from_this<
        openvrml_control::browser::plugin_streambuf>,
    public std::streambuf {

    friend class openvrml_control::browser;

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

openvrml_control::browser::plugin_streambuf::
plugin_streambuf(const std::string & requested_url,
                 uninitialized_plugin_streambuf_map & uninitialized_map,
                 plugin_streambuf_map & map):
    state_(requested),
    get_url_result_(-1),
    url_(requested_url),
    i_(0),
    c_('\0'),
    uninitialized_map_(uninitialized_map),
    map_(map)
{
    //
    // This is really just here to emphasize that c_ must not be EOF.
    //
    this->i_ = traits_type::not_eof(this->i_);
    this->c_ =
        traits_type::to_char_type(
            traits_type::not_eof(traits_type::to_int_type(this->c_)));

    this->setg(&this->c_, &this->c_, &this->c_);
}

openvrml_control::browser::plugin_streambuf::state_id
openvrml_control::browser::plugin_streambuf::state() const
{
    boost::mutex::scoped_lock lock(this->mutex_);
    return this->state_;
}

void
openvrml_control::browser::plugin_streambuf::
set_get_url_result(const int result)
{
    boost::mutex::scoped_lock lock(this->mutex_);
    assert(this->get_url_result_ == -1);
    this->get_url_result_ = result;

    //
    // If result is nonzero, the resource fetch failed early (i.e., before
    // actually getting the stream.  In that case, nothing else should be
    // playing with this.  Removing this streambuf from uninitialized_map_
    // below may (and probably will) result in destruction of this instance.
    //
    // So, anyone waiting on received_get_url_result_ should be doing so
    // because the fetching code is trying to do something with this streambuf
    // because the fetch is in-progress (i.e., succeeding).  If you're waiting
    // on this condition and result could possibly indicate failure, you're
    // doing it wrong.
    //
    if (result == 0) {
        this->state_ = plugin_streambuf::uninitialized;
        this->received_get_url_result_.notify_all();
    } else {
        this->uninitialized_map_.erase(*this);
    }
}

void
openvrml_control::browser::plugin_streambuf::
init(const size_t stream_id,
     const std::string & received_url,
     const std::string & type)
{
    assert(stream_id);
    assert(!received_url.empty());
    assert(!type.empty());

    boost::mutex::scoped_lock lock(this->mutex_);
    while (this->state_ != plugin_streambuf::uninitialized) {
        this->received_get_url_result_.wait(lock);
    }

    assert(this->state_ == uninitialized);

    bool succeeded = this->uninitialized_map_.erase(*this);
    assert(succeeded);
    this->url_ = received_url;
    this->type_ = type;
    this->state_ = plugin_streambuf::initialized;
    const boost::shared_ptr<plugin_streambuf> this_ = shared_from_this();
    succeeded = this->map_.insert(stream_id, this_);
    assert(succeeded);
    this->streambuf_initialized_or_failed_.notify_all();
}

void openvrml_control::browser::plugin_streambuf::fail()
{
    boost::mutex::scoped_lock lock(this->mutex_);
    const bool succeeded = this->uninitialized_map_.erase(*this);
    assert(succeeded);
    this->buf_.set_eof();
    this->streambuf_initialized_or_failed_.notify_all();
}

const std::string & openvrml_control::browser::plugin_streambuf::url() const
{
    boost::mutex::scoped_lock lock(this->mutex_);
    while (this->state_ != plugin_streambuf::initialized) {
        this->streambuf_initialized_or_failed_.wait(lock);
    }
    return this->url_;
}

const std::string & openvrml_control::browser::plugin_streambuf::type() const
{
    boost::mutex::scoped_lock lock(this->mutex_);
    while (this->state_ != plugin_streambuf::initialized) {
        this->streambuf_initialized_or_failed_.wait(lock);
    }
    return this->type_;
}

bool openvrml_control::browser::plugin_streambuf::data_available() const
{
    //
    // It may seem a bit counterintuitive to return true here if the stream
    // has been destroyed; however, if we don't return true in this case,
    // clients may never get EOF from the stream.
    //
    return this->buf_.buffered() > 0 || this->buf_.eof();
}

openvrml_control::browser::plugin_streambuf::int_type
openvrml_control::browser::plugin_streambuf::underflow()
{
    boost::mutex::scoped_lock lock(this->mutex_);
    while (this->state_ != plugin_streambuf::initialized) {
        this->streambuf_initialized_or_failed_.wait(lock);
    }

    if (traits_type::eq_int_type(this->i_, traits_type::eof())) {
        return traits_type::eof();
    }

    this->i_ = this->buf_.get();
    this->c_ = traits_type::to_char_type(this->i_);

    if (traits_type::eq_int_type(this->i_, traits_type::eof())) {
        return traits_type::eof();
    }

    this->setg(&this->c_, &this->c_, &this->c_ + 1);
    return traits_type::to_int_type(*this->gptr());
}

const boost::shared_ptr<openvrml_control::browser::plugin_streambuf>
openvrml_control::browser::uninitialized_plugin_streambuf_map::
find(const std::string & url) const
{
    openvrml::read_write_mutex::scoped_read_lock lock(this->mutex_);
    map_t::const_iterator pos = this->map_.find(url);
    return pos == this->map_.end()
        ? boost::shared_ptr<plugin_streambuf>()
        : pos->second;
}

void
openvrml_control::browser::uninitialized_plugin_streambuf_map::
insert(const std::string & url,
       const boost::shared_ptr<plugin_streambuf> & streambuf)
{
    openvrml::read_write_mutex::scoped_write_lock lock(this->mutex_);
    this->map_.insert(make_pair(url, streambuf));
}

struct OPENVRML_LOCAL openvrml_control::browser::uninitialized_plugin_streambuf_map::map_entry_matches_streambuf :
    std::unary_function<bool, map_t::value_type> {

    explicit map_entry_matches_streambuf(const plugin_streambuf * streambuf):
        streambuf_(streambuf)
    {}

    bool operator()(const map_t::value_type & entry) const
    {
        return this->streambuf_ == entry.second.get();
    }

private:
    const plugin_streambuf * const streambuf_;
};

bool
openvrml_control::browser::uninitialized_plugin_streambuf_map::
erase(const plugin_streambuf & streambuf)
{
    openvrml::read_write_mutex::scoped_read_write_lock lock(this->mutex_);
    const map_t::iterator pos =
        std::find_if(this->map_.begin(), this->map_.end(),
                     map_entry_matches_streambuf(&streambuf));
    if (pos == this->map_.end()) { return false; }
    lock.promote();
    this->map_.erase(pos);
    return true;
}

size_t
openvrml_control::browser::uninitialized_plugin_streambuf_map::size() const
{
    openvrml::read_write_mutex::scoped_read_lock lock(this->mutex_);
    return this->map_.size();
}

bool
openvrml_control::browser::uninitialized_plugin_streambuf_map::empty() const
{
    openvrml::read_write_mutex::scoped_read_lock lock(this->mutex_);
    return this->map_.empty();
}

const boost::shared_ptr<openvrml_control::browser::plugin_streambuf>
openvrml_control::browser::uninitialized_plugin_streambuf_map::front() const
{
    openvrml::read_write_mutex::scoped_read_lock lock(this->mutex_);
    assert(!this->map_.empty());
    return this->map_.begin()->second;
}


const boost::shared_ptr<openvrml_control::browser::plugin_streambuf>
openvrml_control::browser::plugin_streambuf_map::find(const size_t id) const
{
    openvrml::read_write_mutex::scoped_read_lock lock(this->mutex_);
    map_t::const_iterator pos = this->map_.find(id);
    return pos == this->map_.end()
        ? boost::shared_ptr<plugin_streambuf>()
        : pos->second;
}

bool
openvrml_control::browser::plugin_streambuf_map::
insert(const size_t id,
       const boost::shared_ptr<plugin_streambuf> & streambuf)
{
    openvrml::read_write_mutex::scoped_write_lock lock(this->mutex_);
    return this->map_.insert(make_pair(id, streambuf)).second;
}

/**
 * @brief Erase the entry corresponding to @p id.
 *
 * @return @c true if an entry was removed; @c false otherwise.
 */
bool openvrml_control::browser::plugin_streambuf_map::erase(const size_t id)
{
    openvrml::read_write_mutex::scoped_write_lock lock(this->mutex_);
    return this->map_.erase(id) > 0;
}

openvrml_control::browser::resource_fetcher::
resource_fetcher(browser_host & control_host,
                 uninitialized_plugin_streambuf_map &
                     uninitialized_plugin_streambuf_map,
                 plugin_streambuf_map & plugin_streambuf_map):
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

}

struct OPENVRML_LOCAL openvrml_control::browser::initial_stream_reader {
    initial_stream_reader(
        const boost::shared_ptr<plugin_streambuf> & streambuf,
        openvrml::browser & browser):
        streambuf_(streambuf),
        browser_(browser)
        {}

    void operator()() const throw ()
    {
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
    boost::shared_ptr<plugin_streambuf> streambuf_;
    openvrml::browser & browser_;
};

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
    return this->do_get_url(url);
}
