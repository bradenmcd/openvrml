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

# ifndef DOC2_HPP
# define DOC2_HPP

# include <iostream.h>
# include <string>
# include "common.h"

namespace OpenVRML {

    class OPENVRML_SCOPE Doc2 {
        char * url_;
        char * tmpfile_;            // Local copy of http: files
        istream * istm_;
        ostream * ostm_;

    public:
        explicit Doc2(const std::string & url = std::string(),
                      const Doc2 * relative = 0);
        explicit Doc2(const Doc2 *);
        ~Doc2();

        void seturl(const char *url, const Doc2 * relative = 0);

        const char * url() const;         // "http://www.foo.com/dir/file.xyz#Viewpoint"
        const char * urlBase() const;     // "file" or ""
        const char * urlExt() const;      // "xyz" or ""
        const char * urlPath() const;     // "http://www.foo.com/dir/" or ""
        const char * urlProtocol() const; // "http"
        const char * urlModifier() const; // "#Viewpoint" or ""

        const char *localName();    // "/tmp/file.xyz" or NULL
        const char *localPath();    // "/tmp/" or NULL

        ::istream & inputStream();
        ::ostream & outputStream();

    private:
        //
        // Non-copyable.
        //
        Doc2(const Doc2 &);
        Doc2 & operator=(const Doc2 &);

        static const char * stripProtocol(const char * url);
        static bool isAbsolute(const char * url);

        bool filename(char * fn, int nfn);

    # ifdef macintosh
        char * convertCommonToMacPath(char * fn, int nfn);
    # endif
    };
}

# endif
