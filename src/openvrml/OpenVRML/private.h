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

# ifndef NDEBUG
#   include <iostream.h>
# endif

# ifdef NDEBUG
#   define OPENVRML_VERIFY_(condition_)
# else
#   define OPENVRML_VERIFY_(condition_) \
            if (!condition_) { \
                cerr << __FILE__ << ":" << __LINE__ << ": verification (" \
                     << ##condition_ << ") failed" << endl; \
            }
# endif
