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

#   include <cassert>
#   include <cmath>
#   include <cstddef>
#   include <functional>
#   include <limits>

namespace {
    namespace openvrml_ {

        template <typename Float>
        inline Float fabs(const Float f)
        {
            return f < 0.0 ? -f : f;
        }

        template <typename Float>
        struct fequal : std::binary_function<Float, Float, bool> {
            bool operator()(Float a, Float b) const
            {
                const Float diff = fabs(a - b);
                if (diff == 0.0) { return true; }
                static const int rounding_errors = 10;
                static const Float e = std::numeric_limits<Float>::epsilon();
                static const Float tolerance = e * rounding_errors / 2;
                return diff / fabs(a) <= tolerance
                    && diff / fabs(b) <= tolerance;
            }
        };

        template <typename Float>
        struct fless_equal : std::binary_function<Float, Float, bool> {
            bool operator()(Float a, Float b) const
            {
                return a < b || fequal<Float>()(a, b);
            }
        };

        template <typename Float>
        struct fgreater_equal : std::binary_function<Float, Float, bool> {
            bool operator()(Float a, Float b) const
            {
                return a > b || fequal<Float>()(a, b);
            }
        };
    }
}

# endif
