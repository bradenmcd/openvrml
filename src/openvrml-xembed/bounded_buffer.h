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

#   include <string>
#   include <boost/thread/mutex.hpp>
#   include <boost/thread/condition.hpp>

namespace openvrml_player {

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
        size_t write(const char_type * buf, size_t size);
        int_type get();
        size_t read(char_type * buf, size_t size);
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
    size_t
    bounded_buffer<CharT, BufferSize>::write(const char_type * const buf,
                                             const size_t size)
    {
        boost::mutex::scoped_lock lock(this->mutex_);

        using std::copy;

        while (this->buffered_ == BufferSize) {
            this->buffer_not_full_.wait(lock);
        }
        const size_t space_at_back = BufferSize - this->end_;
        size_t num_written;
        if (size < space_at_back) {
            copy(buf, buf + size, &this->buf_[this->end_]);
            num_written = size;
        } else {
            using std::min;
            copy(buf, buf + space_at_back, &this->buf_[this->end_]);
            const size_t space_at_front = this->begin_;
            const size_t remaining_to_write = min(space_at_front,
                                                  size - space_at_back);
            copy(buf + space_at_back, buf + space_at_back + remaining_to_write,
                 this->buf_);
            num_written = space_at_back + remaining_to_write;
        }
        this->end_ = (this->end_ + num_written) % BufferSize;
        this->buffered_ += num_written;
        this->buffer_not_empty_or_eof_.notify_one();
        return num_written;
    }

    template <typename CharT, size_t BufferSize>
    size_t
    bounded_buffer<CharT, BufferSize>::read(char_type * const buf,
                                            const size_t size)
    {
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

# endif // ifndef OPENVRML_PLAYER_BOUNDED_BUFFER_H
