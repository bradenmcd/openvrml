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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

#include "read_write_mutex.h"
#include <cassert>

/**
 * @file openvrml/read_write_mutex.h
 *
 * @brief A read/write mutex.
 */

/**
 * @class openvrml::read_write_mutex openvrml/read_write_mutex.h
 *
 * @brief A read/write mutex with an interface that follows the
 *        patterns established in the Boost thread library.
 *
 * This read/write mutex is implemented in terms of @c boost::mutex
 * and @c boost::condition.
 */

/**
 * @internal
 *
 * @var boost::mutex openvrml::read_write_mutex::mutex_
 *
 * @brief The underlying mutex.
 */

/**
 * @internal
 *
 * @var boost::condition openvrml::read_write_mutex::read_
 *
 * @brief When writing is done, this condition is signaled to indicate
 *        that readers can acquire the lock.
 */

/**
 * @internal
 *
 * @var boost::condition openvrml::read_write_mutex::write_
 *
 * @brief When reading is done, this condition is signaled to indicate
 *        that a writer can acquire the lock.
 */

/**
 * @internal
 *
 * @var std::size_t openvrml::read_write_mutex::readers_active_
 *
 * @brief The number of readers currently sharing the lock.
 */

/**
 * @internal
 *
 * @var std::size_t openvrml::read_write_mutex::readers_waiting_
 *
 * @brief The number of readers currently waiting for the lock.
 */

/**
 * @internal
 *
 * @var std::size_t openvrml::read_write_mutex::writers_waiting_
 *
 * @brief The number of writers currently waiting for the lock.
 */

/**
 * @internal
 *
 * @var bool openvrml::read_write_mutex::writing_
 *
 * @brief @c true if a writer has the lock; @c false otherwise.
 */

/**
 * @class openvrml::read_write_mutex::scoped_read_lock openvrml/read_write_mutex.h
 *
 * @brief Lock the mutex for read access.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex & openvrml::read_write_mutex::scoped_read_lock::mutex_
 *
 * @brief The @c read_write_mutex being locked.
 */

/**
 * @internal
 *
 * @var boost::mutex::scoped_lock openvrml::read_write_mutex::scoped_read_lock::lock_
 *
 * @brief A lock for @c read_write_mutex::mutex_.
 */

/**
 * @brief Construct.
 *
 * Acquire the lock.
 *
 * @param[in] mutex a @c read_write_mutex.
 */
openvrml::read_write_mutex::scoped_read_lock::
scoped_read_lock(read_write_mutex & mutex):
    mutex_(mutex),
    lock_(mutex.mutex_)
{
    if (this->mutex_.writing_) {
        ++this->mutex_.readers_waiting_;
        while (this->mutex_.writing_) {
            this->mutex_.read_.wait(this->lock_);
        }
        --this->mutex_.readers_waiting_;
    }
    ++this->mutex_.readers_active_;
    this->lock_.unlock();
}

/**
 * @brief Destroy.
 *
 * Release the lock.
 */
openvrml::read_write_mutex::scoped_read_lock::~scoped_read_lock()
{
    this->lock_.lock();
    --this->mutex_.readers_active_;
    if (this->mutex_.readers_active_ == 0
        && this->mutex_.writers_waiting_ != 0) {
        this->mutex_.write_.notify_one();
    }
}

/**
 * @class openvrml::read_write_mutex::scoped_write_lock openvrml/read_write_mutex.h
 *
 * @brief Lock the mutex for write access.
 */

/**
 * @internal
 *
 * @var openvrml::read_write_mutex & openvrml::read_write_mutex::scoped_write_lock::mutex_
 *
 * @brief The @c read_write_mutex being locked.
 */

/**
 * @internal
 *
 * @var boost::mutex::scoped_lock openvrml::read_write_mutex::scoped_write_lock::lock_
 *
 * @brief A lock for @c read_write_mutex::mutex_.
 */

/**
 * @brief Construct.
 *
 * Acquire the lock.
 *
 * @param[in] mutex a @c read_write_mutex.
 */
openvrml::read_write_mutex::scoped_write_lock::
scoped_write_lock(read_write_mutex & mutex):
    mutex_(mutex),
    lock_(mutex.mutex_)
{
    if (this->mutex_.writing_ || this->mutex_.readers_active_ != 0) {
        ++this->mutex_.writers_waiting_;
        while (this->mutex_.writing_ || this->mutex_.readers_active_ != 0) {
            this->mutex_.write_.wait(this->lock_);
        }
        --this->mutex_.writers_waiting_;
    }
    this->mutex_.writing_ = true;
    this->lock_.unlock();
}

/**
 * @brief Destroy.
 *
 * Release the lock.
 */
openvrml::read_write_mutex::scoped_write_lock::~scoped_write_lock()
{
    this->lock_.lock();
    this->mutex_.writing_ = false;
    if (this->mutex_.readers_waiting_ != 0) {
        this->mutex_.read_.notify_all();
    } else if (this->mutex_.writers_waiting_ != 0) {
        this->mutex_.write_.notify_one();
    }
}

/**
 * @brief Construct.
 */
openvrml::read_write_mutex::read_write_mutex():
    readers_active_(0),
    readers_waiting_(0),
    writers_waiting_(0),
    writing_(false)
{}


/**
 * @class openvrml::read_write_mutex::scoped_read_write_lock openvrml/read_write_mutex.h
 *
 * @brief Lock the mutex for read/write access.
 *
 * Upon construction, the associated mutex is locked for reading.  The lock
 * can be &ldquo;promoted&rdquo; to a write lock by calling @c #promote.
 */

/**
 * @brief Construct.
 *
 * @param[in] mutex a @c read_write_mutex.
 */
openvrml::read_write_mutex::scoped_read_write_lock::
scoped_read_write_lock(read_write_mutex & mutex):
    scoped_read_lock(mutex)
{}

/**
 * @brief Destroy.
 */
openvrml::read_write_mutex::scoped_read_write_lock::~scoped_read_write_lock()
{
    this->demote();
}

/**
 * @brief Promote the lock to a write lock.
 */
void openvrml::read_write_mutex::scoped_read_write_lock::promote()
{
    this->lock_.lock();
    assert(!this->mutex_.writing_);
    if (this->mutex_.readers_active_ != 1) {
        ++this->mutex_.writers_waiting_;
        while (this->mutex_.readers_active_ != 1) {
            this->mutex_.write_.wait(this->lock_);
        }
        --this->mutex_.writers_waiting_;
    }
    this->mutex_.writing_ = true;
    this->lock_.unlock();
}

/**
 * @brief Demote the lock from a write lock back to a read lock.
 *
 * If the lock has not previously been promoted, this function has no effect.
 */
void openvrml::read_write_mutex::scoped_read_write_lock::demote()
{
    this->lock_.lock();
    this->mutex_.writing_ = false;
    if (this->mutex_.readers_waiting_ != 0) {
        this->mutex_.read_.notify_all();
    } else if (this->mutex_.writers_waiting_ != 0) {
        this->mutex_.read_.notify_one();
    }
    this->lock_.unlock();
}
