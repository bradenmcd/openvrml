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

#   include <stdio.h>
#   include <iosfwd>
#   include <string>
#   include "common.h"

namespace OpenVRML {

    class Doc2;

    class OPENVRML_SCOPE Doc {
        char * d_url;
        std::ostream * d_ostream;
        FILE * d_fp;
        char * d_tmpfile; // Local copy of http: files

    public:
        explicit Doc(const std::string & url = std::string(),
                     const Doc * relative = 0);
        Doc(const std::string & url, const Doc2 * relative);
        ~Doc();

        void seturl(const char * url, const Doc * relative = 0);
        void seturl(const char * url, const Doc2 * relative = 0);

        const char * url() const;          // "http://www.foo.com/dir/file.xyz#Viewpoint"
        const char * urlBase() const;      // "file" or ""
        const char * urlExt() const;       // "xyz" or ""
        const char * urlPath() const;      // "http://www.foo.com/dir/" or ""
        const char * urlProtocol() const;  // "http"
        const char * urlModifier() const;  // "#Viewpoint" or ""

        const char * localName();    // "/tmp/file.xyz" or NULL
        const char * localPath();    // "/tmp/" or NULL


        FILE * fopen(const char * mode);
        void fclose();

        std::ostream & outputStream();

    private:
        // Non-copyable.
        Doc(Doc const &);
        Doc & operator=(Doc const &);

        bool filename(char * fn, int nfn);
        
#   ifdef macintosh
        char * convertCommonToMacPath(char * fn, int nfn);
#   endif
    };
}

# endif
