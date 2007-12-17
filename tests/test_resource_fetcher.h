// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// Copyright 2006, 2007  Braden McDaniel
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along
// with this library; if not, see <http://www.gnu.org/licenses/>.
//

# ifndef TEST_RESOURCE_FETCHER
#   define TEST_RESOURCE_FETCHER

#   include <openvrml/browser.h>

class test_resource_fetcher : public openvrml::resource_fetcher {
private:
    virtual std::auto_ptr<openvrml::resource_istream>
    do_get_resource(const std::string & uri);
};

# endif
