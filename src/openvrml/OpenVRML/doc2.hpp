//
// OpenVRML
//
// Copyright (C) 2000  Braden N. McDaniel
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

# ifndef OPENVRML_DOC2_HPP
#   define OPENVRML_DOC2_HPP

#   include <iostream>
#   include <string>
#   include "common.h"

namespace OpenVRML {

    class OPENVRML_SCOPE Doc2 {
        std::string url_;
        char * tmpfile_;            // Local copy of http: files
        std::istream * istm_;
        std::ostream * ostm_;

    public:
        explicit Doc2(const std::string & url = std::string(),
                      const Doc2 * relative = 0);
        ~Doc2();

        void seturl(const std::string & url, const Doc2 * relative = 0);

        const std::string url() const;         // "http://www.foo.com/dir/file.xyz#Viewpoint"
        const std::string urlBase() const;     // "file" or ""
        const std::string urlExt() const;      // "xyz" or ""
        const std::string urlPath() const;     // "http://www.foo.com/dir/" or ""
        const std::string urlProtocol() const; // "http"
        const std::string urlModifier() const; // "#Viewpoint" or ""

        const char *localName();    // "/tmp/file.xyz" or NULL
        const char *localPath();    // "/tmp/" or NULL

        std::istream & inputStream();
        std::ostream & outputStream();

    private:
        // Non-copyable.
        Doc2(const Doc2 &);
        Doc2 & operator=(const Doc2 &);

        bool filename(char * fn, int nfn);

#   ifdef macintosh
        char * convertCommonToMacPath(char * fn, int nfn);
#   endif
    };
}

# endif
