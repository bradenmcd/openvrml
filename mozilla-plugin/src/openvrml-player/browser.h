// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML Mozilla plug-in
// Copyright 2004, 2005, 2006  Braden N. McDaniel
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

# ifndef OPENVRML_PLAYER_BROWSER_H
#   define OPENVRML_PLAYER_BROWSER_H

#   include <glib.h>
#   include <openvrml/browser.h>

namespace openvrml_player {

    class browser : public openvrml::browser {
        GIOChannel * request_channel_;

    public:
        explicit browser(GIOChannel * request_channel);

    private:
        virtual std::auto_ptr<openvrml::resource_istream>
        do_get_resource(const std::string & uri);
    };
}

# endif // ifndef OPENVRML_PLAYER_BROWSER_H
