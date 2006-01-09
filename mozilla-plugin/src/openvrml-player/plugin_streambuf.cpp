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

# include <glib.h>
# include "plugin_streambuf.h"

openvrml_player::plugin_streambuf::plugin_streambuf(const std::string & url):
    initialized_(false),
    url_(url),
    c_(traits_type::not_eof(this->c_)),
    npstream_destroyed_(false)
{
    this->setg(&this->c_, &this->c_, &this->c_);
}

void openvrml_player::plugin_streambuf::init(const std::string & type)
{
    boost::mutex::scoped_lock lock(this->mutex_);
    this->type_ = type;
    this->initialized_ = true;
    this->streambuf_initialized_.notify_all();
}

const std::string & openvrml_player::plugin_streambuf::url() const
{
    //
    // No need to lock or wait on init here; this->url_ is set in the
    // constructor and cannot be changed.
    //
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
    return this->buf_.buffered() > 0;
}

openvrml_player::plugin_streambuf::int_type
openvrml_player::plugin_streambuf::underflow()
{
    boost::mutex::scoped_lock lock(this->mutex_);
    while (!this->initialized_) {
        this->streambuf_initialized_.wait(lock);
    }
    if (this->c_ == traits_type::eof()) { return traits_type::eof(); }
    this->c_ = this->buf_.get();
    this->setg(&this->c_, &this->c_, &this->c_ + 1);
    return *this->gptr();
}

void openvrml_player::plugin_streambuf::npstream_destroyed()
{
    this->npstream_destroyed_ = true;
}


openvrml_player::uninitialized_plugin_streambuf_set
openvrml_player::uninitialized_plugin_streambuf_set_;

const boost::shared_ptr<openvrml_player::plugin_streambuf>
openvrml_player::uninitialized_plugin_streambuf_set::
find(const std::string & url) const
{
    boost::mutex::scoped_lock lock(this->mutex_);
    set_t::const_iterator pos = find_if(this->set_.begin(),
                                        this->set_.end(),
                                        plugin_streambuf_has_url(url));
    return pos == this->set_.end()
        ? boost::shared_ptr<plugin_streambuf>()
        : *pos;
}

bool
openvrml_player::uninitialized_plugin_streambuf_set::
insert(const boost::shared_ptr<openvrml_player::plugin_streambuf> & streambuf)
{
    boost::mutex::scoped_lock lock(this->mutex_);
    return this->set_.insert(streambuf).second;
}

bool
openvrml_player::uninitialized_plugin_streambuf_set::
erase(const boost::shared_ptr<openvrml_player::plugin_streambuf> & streambuf)
{
    boost::mutex::scoped_lock lock(this->mutex_);
    return this->set_.erase(streambuf) == 1;
}

size_t openvrml_player::uninitialized_plugin_streambuf_set::size() const
{
    boost::mutex::scoped_lock lock(this->mutex_);
    return this->set_.size();
}

const boost::shared_ptr<openvrml_player::plugin_streambuf>
openvrml_player::uninitialized_plugin_streambuf_set::front() const
{
    boost::mutex::scoped_lock lock(this->mutex_);
    g_assert(!this->set_.empty());
    return *this->set_.begin();
}


openvrml_player::plugin_streambuf_map_t openvrml_player::plugin_streambuf_map;
