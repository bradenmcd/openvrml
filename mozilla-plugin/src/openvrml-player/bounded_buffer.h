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

# ifndef OPENVRML_PLAYER_BOUNDED_BUFFER_H
#   define OPENVRML_PLAYER_BOUNDED_BUFFER_H

#   include <boost/thread/mutex.hpp>
#   include <boost/thread/condition.hpp>

namespace openvrml_player {

    template <typename T, size_t BufferSize>
    class bounded_buffer {
        mutable boost::mutex mutex_;
        boost::condition buffer_not_full_, buffer_not_empty_;

        T buf_[BufferSize];
        size_t begin_, end_, buffered_;

    public:
        bounded_buffer();
        void put(const T & c);
        const T get();
        size_t buffered() const;
    };

    template <typename T, size_t BufferSize>
    bounded_buffer<T, BufferSize>::bounded_buffer():
        begin_(0),
        end_(0),
        buffered_(0)
    {}

    template <typename T, size_t BufferSize>
    void bounded_buffer<T, BufferSize>::put(const T & c)
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        while (this->buffered_ == BufferSize) {
            this->buffer_not_full_.wait(lock);
        }
        this->buf_[this->end_] = c;
        this->end_ = (this->end_ + 1) % BufferSize;
        ++this->buffered_;
        this->buffer_not_empty_.notify_one();
    }

    template <typename T, size_t BufferSize>
    const T bounded_buffer<T, BufferSize>::get()
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        while (this->buffered_ == 0) {
            this->buffer_not_empty_.wait(lock);
        }
        T c = this->buf_[this->begin_];
        this->begin_ = (this->begin_ + 1) % BufferSize;
        --this->buffered_;
        this->buffer_not_full_.notify_one();
        return c;
    }

    template <typename T, size_t BufferSize>
    size_t bounded_buffer<T, BufferSize>::buffered() const
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        return this->buffered_;
    }
}

# endif // ifndef OPENVRML_PLAYER_BOUNDED_BUFFER_H
