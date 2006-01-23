// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML Mozilla plug-in
// Copyright 2006  Braden N. McDaniel
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

# ifndef OPENVRML_PLAYER_FLAG_H
#   define OPENVRML_PLAYER_FLAG_H

#   include <boost/thread/mutex.hpp>

namespace openvrml_player {

    class flag : boost::noncopyable {
        mutable boost::mutex mutex_;
        bool value_;

    public:
        flag(bool init = false);

        bool value() const;
        void value(bool val);
    };
}

# endif // ifndef OPENVRML_PLAYER_FLAG_H
