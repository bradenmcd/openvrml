// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
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

# ifndef OPENVRML_IMG_H
#   define OPENVRML_IMG_H

#   include <cstddef>
#   include <string>
#   include <vector>
#   include <openvrml/common.h>

namespace openvrml {

    class doc;
    class doc2;

    class OPENVRML_SCOPE img {
        doc * url_;
        int w_, h_, nc_, nframes_;
        unsigned char * pixels_;
        unsigned char ** frame_;

    public:
        img();
        ~img();

        bool set_url(const char * url, const doc2 * relative = 0);

        bool try_urls(const std::vector<std::string> & urls,
                      const doc2 * relative = 0);

        const char * url() const;

        int w() const;
        int h() const;
        int nc() const;
        int nframes() const;
        unsigned char * pixels() const;
        unsigned char * pixels(int frame) const;

        void resize(int w, int h);
    };

    inline int img::w() const
    {
        return this->w_;
    }

    inline int img::h() const
    {
        return this->h_;
    }

    inline int img::nc() const
    {
        return this->nc_;
    }

    inline int img::nframes() const
    {
        return this->nframes_;
    }

    inline unsigned char * img::pixels() const
    {
        return this->pixels_;
    }

    inline void img::resize(const int w, const int h)
    {
        this->w_ = w;
        this->h_ = h;
    }
}

# endif // OPENVRML_IMG_H
