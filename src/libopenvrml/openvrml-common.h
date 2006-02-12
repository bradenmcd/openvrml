// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 2005  Braden McDaniel
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

# ifndef OPENVRML_COMMON_H
#   define OPENVRML_COMMON_H

#   include <openvrml-config.h>

#   if OPENVRML_ENABLE_EXCEPTION_SPECS
#     define OPENVRML_NOTHROW throw ()
#     define OPENVRML_THROW1(ex_) throw (ex_)
#     define OPENVRML_THROW2(ex1_, ex2_) throw (ex1_, ex2_)
#     define OPENVRML_THROW3(ex1_, ex2_, ex3_) throw (ex1_, ex2_, ex3_)
#     define OPENVRML_THROW4(ex1_, ex2_, ex3_, ex4_) throw (ex1_, ex2_, ex3_, ex4_)
#     define OPENVRML_THROW5(ex1_, ex2_, ex3_, ex4_, ex5_) throw (ex1_, ex2_, ex3_, ex4_, ex5_)
#   else
#     define OPENVRML_NOTHROW
#     define OPENVRML_THROW1(ex_)
#     define OPENVRML_THROW2(ex1_, ex2_)
#     define OPENVRML_THROW3(ex1_, ex2_, ex3_)
#     define OPENVRML_THROW4(ex1_, ex2_, ex3_, ex4_)
#     define OPENVRML_THROW5(ex1_, ex2_, ex3_, ex4_, ex5_)
#   endif

# endif // ifndef OPENVRML_COMMON_H
