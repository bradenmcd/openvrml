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

# ifndef OPENVRML_DOC_H
#   define OPENVRML_DOC_H

#   include <cstdio>
#   include <iosfwd>
#   include <string>
#   include <openvrml/common.h>

namespace openvrml {

    class doc2;

    class doc {
        char * url_;
        std::ostream * out_;
        FILE * fp_;
        char * tmpfile_; // Local copy of http: files

    public:
        explicit doc(const std::string & url = std::string(),
                     const doc * relative = 0);
        doc(const std::string & url, const doc2 * relative);
        ~doc();

        void seturl(const char * url, const doc * relative = 0);
        void seturl(const char * url, const doc2 * relative = 0);

        const char * url() const;          // "http://www.foo.com/dir/file.xyz#Viewpoint"
        const char * url_base() const;      // "file" or ""
        const char * url_ext() const;       // "xyz" or ""
        const char * url_path() const;      // "http://www.foo.com/dir/" or ""
        const char * url_protocol() const;  // "http"
        const char * url_modifier() const;  // "#Viewpoint" or ""

        const char * local_name();    // "/tmp/file.xyz" or NULL
        const char * local_path();    // "/tmp/" or NULL


        FILE * fopen(const char * mode);
        void fclose();

        std::ostream & output_stream();

    private:
        // Non-copyable.
        doc(doc const &);
        doc & operator=(doc const &);

        bool filename(char * fn, int nfn);
    };

    class doc2 {
        std::string url_;
        char * tmpfile_;            // Local copy of http: files
        std::istream * istm_;
        std::ostream * ostm_;

    public:
        explicit doc2(const std::string & url = std::string(),
                      const doc2 * relative = 0);
        ~doc2();

        void seturl(const std::string & url, const doc2 * relative = 0);

        const std::string url() const;         // "http://www.foo.com/dir/file.xyz#Viewpoint"
        const std::string url_base() const;     // "file" or ""
        const std::string url_ext() const;      // "xyz" or ""
        const std::string url_path() const;     // "http://www.foo.com/dir/" or ""
        const std::string url_protocol() const; // "http"
        const std::string url_modifier() const; // "#Viewpoint" or ""

        const char * local_name();    // "/tmp/file.xyz" or NULL
        const char * local_path();    // "/tmp/" or NULL

        std::istream & input_stream();
        std::ostream & output_stream();

    private:
        // Non-copyable.
        doc2(const doc2 &);
        doc2 & operator=(const doc2 &);

        bool filename(char * fn, size_t nfn);
    };
}

# endif
