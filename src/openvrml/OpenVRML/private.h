// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 2001  Braden McDaniel
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

# ifndef OPENVRML_PRIVATE_H
#   define OPENVRML_PRIVATE_H

#   ifndef NDEBUG
#     include <iostream>
#   endif

#   ifdef NDEBUG
#     define OPENVRML_VERIFY_(condition_)
#   else
#     define OPENVRML_VERIFY_(condition_) \
            do { \
                if (!(condition_)) { \
                    std::cerr << __FILE__ << ":" << __LINE__ \
                              << ": verification (" #condition_ ") failed" \
                              << std::endl; \
                } \
            } while (false)
#   endif

#   ifdef NDEBUG
#     define OPENVRML_PRINT_EXCEPTION_(ex_)
#   else
#     define OPENVRML_PRINT_EXCEPTION_(ex_) \
            std::cerr << __FILE__ << ":" << __LINE__ << ": " \
                      << ex_.what() << std::endl
#   endif

#   ifdef NDEBUG
#     define OPENVRML_PRINT_MESSAGE_(message_)
#   else
#     define OPENVRML_PRINT_MESSAGE_(message_) \
            std::cerr << __FILE__ << ":" << __LINE__ << ": " \
                      << message_ << std::endl
#   endif

#   include <assert.h>
#   include <math.h>
#   include <stddef.h>

namespace {
    namespace OpenVRML_ {

        const float fptolerance = 1.0e-6;

        inline bool fpzero(const float f) { return (fabs(f) <= fptolerance); }

        inline bool fpequal(const float a, const float b)
        {
            return fpzero(a - b);
        }

        inline bool fpless(const float a, const float b)
        {
            return (b - a) > fptolerance;
        }

        template <typename T>
            class SharedPtr {
                T * data;
                size_t * count;

            public:
                explicit SharedPtr(T * data = 0);
                SharedPtr(const SharedPtr<T> & sharedPtr);
                ~SharedPtr();

                SharedPtr<T> & operator=(const SharedPtr<T> & sharedPtr);

                T & operator*() const;
                T * operator->() const;
                T * get() const;

                void reset(T * data = 0);

            private:
                void dispose();
                void share(T * data, size_t * count);
            };

        template <typename T>
            SharedPtr<T>::SharedPtr(T * data): data(data) {
                try {
                    this->count = new size_t(1);
                } catch (...) {
                    delete data;
                    throw;
                }
            }

        template <typename T>
            SharedPtr<T>::SharedPtr(const SharedPtr<T> & sharedPtr):
                    data(sharedPtr.data) {
                ++*(this->count = sharedPtr.count);
            }

        template <typename T>
            SharedPtr<T>::~SharedPtr() { this->dispose(); }

        template <typename T>
            SharedPtr<T> &
                    SharedPtr<T>::operator=(const SharedPtr<T> & sharedPtr) {
                this->share(sharedPtr.data, sharedPtr.count);
                return *this;
            }

        template <typename T>
            T & SharedPtr<T>::operator*() const { return *this->data; }

        template <typename T>
            T * SharedPtr<T>::operator->() const { return this->data; }

        template <typename T>
            T * SharedPtr<T>::get() const { return this->data; }

        template <typename T>
            void SharedPtr<T>::reset(T * data) {
                assert(data != this->data); // fail on self-assignment
                if (--*this->count == 0) {
                    delete this->data;
                } else {
                    try {
                        this->data = new size_t;
                    } catch (...) {
                        ++*this->count;
                        delete data;
                        throw;
                    }
                }
                *this->count = 1;
                this->data = data;
            }

        template <typename T>
            void SharedPtr<T>::dispose() {
                if (--*this->count == 0) {
                    delete this->data;
                    delete this->count;
                }
            }

        template <typename T>
            void SharedPtr<T>::share(T * data, size_t * count) {
                if (this->count != count) {
                    ++*count;
                    this->dispose();
                    this->data = data;
                    this->count = count;
                }
            }

        template <typename T, typename U>
            inline bool operator==(const SharedPtr<T> & a,
                                   const SharedPtr<U> & b) {
                return a.get() == b.get();
            }
    }
}

# endif
