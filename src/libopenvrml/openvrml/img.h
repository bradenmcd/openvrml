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

    class img {
        doc * url_;
        size_t w_, h_, nc_, nframes_;
        unsigned char * pixels_;
        unsigned char ** frame_;

    public:
        img();
        ~img();

        bool set_url(const char * url, const doc2 * relative = 0);

        bool try_urls(const std::vector<std::string> & urls,
                      const doc2 * relative = 0);

        const char * url() const;

        size_t w() const;
        size_t h() const;
        size_t nc() const;
        size_t nframes() const;
        unsigned char * pixels() const;
        unsigned char * pixels(size_t frame) const;

        void resize(size_t w, size_t h);
    };

    inline size_t img::w() const
    {
        return this->w_;
    }

    inline size_t img::h() const
    {
        return this->h_;
    }

    inline size_t img::nc() const
    {
        return this->nc_;
    }

    inline size_t img::nframes() const
    {
        return this->nframes_;
    }

    inline unsigned char * img::pixels() const
    {
        return this->pixels_;
    }

    inline void img::resize(const size_t w, const size_t h)
    {
        this->w_ = w;
        this->h_ = h;
    }
}

# endif // OPENVRML_IMG_H
