// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
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

# include <glib.h>
# include "plugin_streambuf.h"

openvrml_player::plugin_streambuf::
plugin_streambuf(const std::string & requested_url):
    get_url_result_(-1),
    initialized_(false),
    url_(requested_url),
    i_(0),
    c_('\0')
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

void openvrml_player::plugin_streambuf::set_get_url_result(const int result)
{
    boost::mutex::scoped_lock lock(this->mutex_);
    assert(this->get_url_result_ == -1);
    this->get_url_result_ = result;
    this->received_get_url_result_.notify_all();
}

int openvrml_player::plugin_streambuf::get_url_result() const
{
    boost::mutex::scoped_lock lock(this->mutex_);
    while (this->get_url_result_ == -1) {
        this->received_get_url_result_.wait(lock);
    }
    return this->get_url_result_;
}

void openvrml_player::plugin_streambuf::init(const size_t stream_id,
                                             const std::string & received_url,
                                             const std::string & type)
{
    boost::mutex::scoped_lock lock(this->mutex_);
    bool succeeded = uninitialized_plugin_streambuf_map_.erase(this->url_);
    g_assert(succeeded);
    this->url_ = received_url;
    this->type_ = type;
    this->initialized_ = true;
    const boost::shared_ptr<plugin_streambuf> this_ = shared_from_this();
    succeeded = plugin_streambuf_map.insert(make_pair(stream_id, this_))
        .second;
    g_assert(succeeded);
    this->streambuf_initialized_.notify_all();
}

const std::string & openvrml_player::plugin_streambuf::url() const
{
    boost::mutex::scoped_lock lock(this->mutex_);
    while (!this->initialized_) {
        this->streambuf_initialized_.wait(lock);
    }
    return this->url_;
}

const std::string & openvrml_player::plugin_streambuf::type() const
{
    boost::mutex::scoped_lock lock(this->mutex_);
    while (!this->initialized_) {
        this->streambuf_initialized_.wait(lock);
    }
    return this->type_;
}

bool openvrml_player::plugin_streambuf::data_available() const
{
    //
    // It may seem a bit counterintuitive to return true here if the stream
    // has been destroyed; however, if we don't return true in this case,
    // clients may never get EOF from the stream.
    //
    return this->buf_.buffered() > 0 || this->buf_.eof();
}

openvrml_player::plugin_streambuf::int_type
openvrml_player::plugin_streambuf::underflow()
{
    boost::mutex::scoped_lock lock(this->mutex_);
    while (!this->initialized_) {
        this->streambuf_initialized_.wait(lock);
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


openvrml_player::uninitialized_plugin_streambuf_map
openvrml_player::uninitialized_plugin_streambuf_map_;

const boost::shared_ptr<openvrml_player::plugin_streambuf>
openvrml_player::uninitialized_plugin_streambuf_map::
find(const std::string & url) const
{
    boost::mutex::scoped_lock lock(this->mutex_);
    map_t::const_iterator pos = this->map_.find(url);
    return pos == this->map_.end()
        ? boost::shared_ptr<plugin_streambuf>()
        : pos->second;
}

void
openvrml_player::uninitialized_plugin_streambuf_map::
insert(const std::string & url,
       const boost::shared_ptr<plugin_streambuf> & streambuf)
{
    boost::mutex::scoped_lock lock(this->mutex_);
    this->map_.insert(make_pair(url, streambuf));
}

/**
 * @brief Erase the first entry corresponding to @p url.
 *
 * The map may have multiple entries corresponding to @p url if the same
 * resource has been requested multiple times.  A single call to @c erase will
 * only remove one of them.
 *
 * @return @c true if an entry was removed; @c false otherwise.
 */
bool
openvrml_player::uninitialized_plugin_streambuf_map::
erase(const std::string & url)
{
    boost::mutex::scoped_lock lock(this->mutex_);
    const map_t::iterator pos = this->map_.find(url);
    if (pos == this->map_.end()) { return false; }
    this->map_.erase(pos);
    return true;
}

size_t openvrml_player::uninitialized_plugin_streambuf_map::size() const
{
    boost::mutex::scoped_lock lock(this->mutex_);
    return this->map_.size();
}

const boost::shared_ptr<openvrml_player::plugin_streambuf>
openvrml_player::uninitialized_plugin_streambuf_map::front() const
{
    boost::mutex::scoped_lock lock(this->mutex_);
    g_assert(!this->map_.empty());
    return this->map_.begin()->second;
}


openvrml_player::plugin_streambuf_map_t openvrml_player::plugin_streambuf_map;
