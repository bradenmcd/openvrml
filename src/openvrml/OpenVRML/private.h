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
#     include <iostream.h>
#   endif

#   ifdef NDEBUG
#     define OPENVRML_VERIFY_(condition_)
#   else
#     define OPENVRML_VERIFY_(condition_) \
            if (!condition_) { \
                cerr << __FILE__ << ":" << __LINE__ << ": verification (" \
                     << ##condition_ << ") failed" << endl; \
            }
#   endif

#   ifdef NDEBUG
#     define OPENVRML_PRINT_EXCEPTION_(ex_)
#   else
#     define OPENVRML_PRINT_EXCEPTION_(ex_) \
            std::cerr << __FILE__ << ":" << __LINE__ << ": " \
                      << ex_.what() << std::endl
#   endif
            
#   include <assert.h>
#   include <math.h>

const float FPTOLERANCE(1.0e-6);

inline bool fpzero(const float f) {
    return (fabs(f) <= FPTOLERANCE);
}

inline bool fpequal(const float a, const float b) {
    return fpzero(a - b);
}

inline double length(const float vec[3]) {
    return sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
}

inline void normalize(float vec[3]) {
    const float len = static_cast<float>(length(vec));
    if (!fpzero(len)) {
        vec[0] /= len;
        vec[1] /= len;
        vec[2] /= len;
    }
}

# endif
