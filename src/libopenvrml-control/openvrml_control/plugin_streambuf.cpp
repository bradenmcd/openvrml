// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML Control
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

# include "plugin_streambuf.h"

openvrml_control::plugin_streambuf::
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

openvrml_control::plugin_streambuf::state_id
openvrml_control::plugin_streambuf::state() const
{
    boost::mutex::scoped_lock lock(this->mutex_);
    return this->state_;
}

void openvrml_control::plugin_streambuf::set_get_url_result(const int result)
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

void openvrml_control::plugin_streambuf::init(const size_t stream_id,
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

void openvrml_control::plugin_streambuf::fail()
{
    boost::mutex::scoped_lock lock(this->mutex_);
    const bool succeeded = this->uninitialized_map_.erase(*this);
    assert(succeeded);
    this->buf_.set_eof();
    this->streambuf_initialized_or_failed_.notify_all();
}

const std::string & openvrml_control::plugin_streambuf::url() const
{
    boost::mutex::scoped_lock lock(this->mutex_);
    while (this->state_ != plugin_streambuf::initialized) {
        this->streambuf_initialized_or_failed_.wait(lock);
    }
    return this->url_;
}

const std::string & openvrml_control::plugin_streambuf::type() const
{
    boost::mutex::scoped_lock lock(this->mutex_);
    while (this->state_ != plugin_streambuf::initialized) {
        this->streambuf_initialized_or_failed_.wait(lock);
    }
    return this->type_;
}

bool openvrml_control::plugin_streambuf::data_available() const
{
    //
    // It may seem a bit counterintuitive to return true here if the stream
    // has been destroyed; however, if we don't return true in this case,
    // clients may never get EOF from the stream.
    //
    return this->buf_.buffered() > 0 || this->buf_.eof();
}

openvrml_control::plugin_streambuf::int_type
openvrml_control::plugin_streambuf::underflow()
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

const boost::shared_ptr<openvrml_control::plugin_streambuf>
openvrml_control::uninitialized_plugin_streambuf_map::
find(const std::string & url) const
{
    openvrml::read_write_mutex::scoped_read_lock lock(this->mutex_);
    map_t::const_iterator pos = this->map_.find(url);
    return pos == this->map_.end()
        ? boost::shared_ptr<plugin_streambuf>()
        : pos->second;
}

void
openvrml_control::uninitialized_plugin_streambuf_map::
insert(const std::string & url,
       const boost::shared_ptr<plugin_streambuf> & streambuf)
{
    openvrml::read_write_mutex::scoped_write_lock lock(this->mutex_);
    this->map_.insert(make_pair(url, streambuf));
}

struct openvrml_control::uninitialized_plugin_streambuf_map::map_entry_matches_streambuf :
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
openvrml_control::uninitialized_plugin_streambuf_map::
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

size_t openvrml_control::uninitialized_plugin_streambuf_map::size() const
{
    openvrml::read_write_mutex::scoped_read_lock lock(this->mutex_);
    return this->map_.size();
}

bool openvrml_control::uninitialized_plugin_streambuf_map::empty() const
{
    openvrml::read_write_mutex::scoped_read_lock lock(this->mutex_);
    return this->map_.empty();
}

const boost::shared_ptr<openvrml_control::plugin_streambuf>
openvrml_control::uninitialized_plugin_streambuf_map::front() const
{
    openvrml::read_write_mutex::scoped_read_lock lock(this->mutex_);
    assert(!this->map_.empty());
    return this->map_.begin()->second;
}


const boost::shared_ptr<openvrml_control::plugin_streambuf>
openvrml_control::plugin_streambuf_map::find(const size_t id) const
{
    openvrml::read_write_mutex::scoped_read_lock lock(this->mutex_);
    map_t::const_iterator pos = this->map_.find(id);
    return pos == this->map_.end()
        ? boost::shared_ptr<plugin_streambuf>()
        : pos->second;
}

bool
openvrml_control::plugin_streambuf_map::
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
bool openvrml_control::plugin_streambuf_map::erase(const size_t id)
{
    openvrml::read_write_mutex::scoped_write_lock lock(this->mutex_);
    return this->map_.erase(id) > 0;
}
