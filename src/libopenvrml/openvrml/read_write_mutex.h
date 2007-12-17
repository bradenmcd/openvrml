// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2007  Braden McDaniel
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 3 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, see <http://www.gnu.org/licenses/>.
//

# ifndef OPENVRML_READ_WRITE_MUTEX_H
#   define OPENVRML_READ_WRITE_MUTEX_H

#   include <openvrml-common.h>
#   include <boost/thread/mutex.hpp>
#   include <boost/thread/condition.hpp>

namespace openvrml {

    class OPENVRML_API read_write_mutex : boost::noncopyable {
        boost::mutex mutex_;
        boost::condition read_, write_;
        std::size_t readers_active_, readers_waiting_, writers_waiting_;
        bool writing_;

    public:
        class scoped_read_write_lock;

        class scoped_read_lock : boost::noncopyable {
            friend class scoped_read_write_lock;

            read_write_mutex & mutex_;
            boost::mutex::scoped_lock lock_;

        public:
            explicit scoped_read_lock(read_write_mutex & mutex);
            ~scoped_read_lock();
        };

        class scoped_write_lock : boost::noncopyable {
            read_write_mutex & mutex_;
            boost::mutex::scoped_lock lock_;

        public:
            explicit scoped_write_lock(read_write_mutex & mutex);
            ~scoped_write_lock();
        };

        class scoped_read_write_lock : scoped_read_lock {
        public:
            explicit scoped_read_write_lock(read_write_mutex & mutex);
            ~scoped_read_write_lock();

            void promote();
            void demote();
        };

        read_write_mutex();
    };
}

# endif // OPENVRML_READ_WRITE_MUTEX_H
