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
# include "common.h"

class OPENVRML_SCOPE Doc2 {
    public:
        
        Doc2(const char *url = 0, Doc2 const *relative = 0);
        Doc2(Doc2 const *);
        ~Doc2();
        
        void seturl(const char *url, Doc2 const *relative = 0);
        
        char const * url() const;         // "http://www.foo.com/dir/file.xyz#Viewpoint"
        char const * urlBase() const;     // "file" or ""
        char const * urlExt() const;      // "xyz" or ""
        char const * urlPath() const;     // "http://www.foo.com/dir/" or ""
        char const * urlProtocol() const; // "http"
        char const * urlModifier() const; // "#Viewpoint" or ""
        
        const char *localName();    // "/tmp/file.xyz" or NULL
        const char *localPath();    // "/tmp/" or NULL
        
        ::istream & inputStream();
        ::ostream & outputStream();
        
    private:
        //
        // Non-copyable; copy ctor and operator= are declared private and not
        // defined.
        //
        Doc2(Doc2 const &);
        Doc2 & operator=(Doc2 const &);
        
        static char const * stripProtocol(char const * url);
        static bool isAbsolute(char const * url);
        
        bool filename(char * fn, int nfn);
        
# ifdef macintosh
        char * convertCommonToMacPath(char * fn, int nfn);
# endif
        
        char * url_;
        char * tmpfile_;            // Local copy of http: files
        istream * istm_;
        ostream * ostm_;
};

# endif
