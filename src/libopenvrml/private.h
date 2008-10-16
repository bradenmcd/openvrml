// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007  Braden McDaniel
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

# ifndef OPENVRML_PRIVATE_H
#   define OPENVRML_PRIVATE_H

#   include <openvrml-common.h>

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
#     define OPENVRML_PRINT_EXCEPTION_(ex_) \
            boost::ignore_unused_variable_warning(ex_)
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

# endif
