// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2008  Braden McDaniel
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

# ifndef OPENVRML_BAD_URL_H
#   define OPENVRML_BAD_URL_H

#   include <openvrml-common.h>
#   include <stdexcept>
#   include <string>

namespace openvrml {

    class OPENVRML_API bad_url : public std::runtime_error {
    public:
        bad_url(const std::string & message);
        virtual ~bad_url() throw ();
    };


    class OPENVRML_API invalid_url : public bad_url {
    public:
        invalid_url();
        virtual ~invalid_url() throw ();
    };


    class OPENVRML_API bad_media_type : public bad_url {
    public:
        explicit bad_media_type(const std::string & received_type)
            OPENVRML_NOTHROW;
        virtual ~bad_media_type() throw ();
    };


    class OPENVRML_API unreachable_url : public bad_url {
    public:
        unreachable_url();
        virtual ~unreachable_url() throw ();
    };


    class OPENVRML_API no_alternative_url : public bad_url {
    public:
        no_alternative_url();
        virtual ~no_alternative_url() throw ();
    };
}

# endif // ifndef OPENVRML_BAD_URL_H
