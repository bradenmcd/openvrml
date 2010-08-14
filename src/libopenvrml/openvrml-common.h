// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2005, 2006, 2007, 2008, 2009  Braden McDaniel
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

# ifndef OPENVRML_COMMON_H
#   define OPENVRML_COMMON_H

#   ifdef _WIN32
#     include <openvrml-config-win32.h>
#   else
#     include <openvrml-config.h>
#   endif

#   ifndef BOOST_SPIRIT_THREADSAFE
#     define BOOST_SPIRIT_THREADSAFE
#   endif

#   ifndef BOOST_SPIRIT_CLOSURE_LIMIT
#     define BOOST_SPIRIT_CLOSURE_LIMIT 6
#   endif

#   ifndef PHOENIX_LIMIT
#     define PHOENIX_LIMIT 6
#   endif

#   if OPENVRML_ENABLE_NOTHROW_EXCEPTION_SPECS
#     define OPENVRML_NOTHROW throw ()
#   else
#     define OPENVRML_NOTHROW
#   endif

#   if OPENVRML_ENABLE_THROWING_EXCEPTION_SPECS
#     define OPENVRML_THROW1(ex_) throw (ex_)
#     define OPENVRML_THROW2(ex1_, ex2_) throw (ex1_, ex2_)
#     define OPENVRML_THROW3(ex1_, ex2_, ex3_) throw (ex1_, ex2_, ex3_)
#     define OPENVRML_THROW4(ex1_, ex2_, ex3_, ex4_) throw (ex1_, ex2_, ex3_, ex4_)
#     define OPENVRML_THROW5(ex1_, ex2_, ex3_, ex4_, ex5_) throw (ex1_, ex2_, ex3_, ex4_, ex5_)
#   else
#     define OPENVRML_THROW1(ex_)
#     define OPENVRML_THROW2(ex1_, ex2_)
#     define OPENVRML_THROW3(ex1_, ex2_, ex3_)
#     define OPENVRML_THROW4(ex1_, ex2_, ex3_, ex4_)
#     define OPENVRML_THROW5(ex1_, ex2_, ex3_, ex4_, ex5_)
#   endif

# endif // ifndef OPENVRML_COMMON_H
