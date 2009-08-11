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

# ifndef OPENVRML_LOCAL_FLOAT_H
#   define OPENVRML_LOCAL_FLOAT_H

#   include <openvrml-common.h>
#   include <limits>

namespace openvrml {

    namespace local {

        const double pi     = 3.14159265358979323846;
        const double pi_2   = 1.57079632679489661923;
        const double pi_4   = 0.78539816339744830962;
        const double inv_pi = 0.31830988618379067154;

        template <typename Float>
        OPENVRML_LOCAL inline Float fabs(const Float f)
        {
            return f < 0.0 ? -f : f;
        }

        struct OPENVRML_LOCAL fequal_t {
            template <typename Float>
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

        const fequal_t fequal = fequal_t();

        struct OPENVRML_LOCAL fless_equal_t {
            template <typename Float>
            bool operator()(Float a, Float b) const
            {
                return a < b || fequal(a, b);
            }
        };

        const fless_equal_t fless_equal = fless_equal_t();

        struct OPENVRML_LOCAL fgreater_equal_t {
            template <typename Float>
            bool operator()(Float a, Float b) const
            {
                return a > b || fequal(a, b);
            }
        };

        const fgreater_equal_t fgreater_equal = fgreater_equal_t();
    }
}

# endif // ifndef OPENVRML_LOCAL_FLOAT_H
